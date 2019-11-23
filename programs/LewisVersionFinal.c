#pragma config(Sensor, S1,     gyro,           sensorEV3_Gyro, modeEV3Gyro_RateAndAngle)
#pragma config(Sensor, S2,     rightSonar,     sensorEV3_Ultrasonic)
#pragma config(Sensor, S3,     lightSensor,    sensorI2CCustom)
#pragma config(Sensor, S4,     leftSonar,      sensorEV3_Ultrasonic)
#pragma config(Motor,  motorA,          rightMotor,    tmotorEV3_Large, PIDControl, encoder)
#pragma config(Motor,  motorC,          leftMotor,     tmotorEV3_Large, PIDControl, encoder)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//
#include "HitechnicColorSensor.h"

bool foundLine = false;
long avg;
long maxLight, minLight, cutoff;

long lightThreshold = 1100;

float errP, errI, errD, errPrev, maxCap, minCap, errDPrev, secondDeriv;
long setPt = 500;
float setPtTolerance = 50;

const int windowSize = 50;
float hiLightReadings[windowSize];
float loLightReadings[windowSize];
float lightLevels[windowSize];

//PID Coefficients
const float kP = 4.5;
const float kI = 0.000002;
const float kD = 1.1;

const float baseSpeed =8;
float leftSpeed = 0;
float rightSpeed = 0;
float spiralFactor = 0.2;

//for observe
long longestPath = 0;
bool newStraight = true;

float dt = 0;
const float sampleLength = 50;

/* Timers
	T1: Object avoidance
	T2: PID error monitor
	T3: Track time since lost line
	T4: Observe timer
*/

//Command request struct for behaviours to request motor control
typedef struct {
	float lSpeed;
	float rSpeed;
	bool broadcasting;
	string name;
} cmdRequest;

cmdRequest followCmd;
cmdRequest forageCmd;
cmdRequest avoidCmd;
cmdRequest observeCmd;

void wipeError(){ //Set all PID error values to 0
	errP = 0;
	errI = 0;
	errD = 0;
}

void display(){

		displayTextLine(0,"Current Light: %f",avg);
		displayTextLine(1,"Current Target: %f", setPt);
		displayTextLine(2,"Max: %f", maxLight);
		displayTextLine(3,"Min: %f", minLight);

		if (forageCmd.broadcasting) {
			displayTextLine(4,"Forage Behaviour: True,%i,%i", forageCmd.lSpeed, forageCmd.rSpeed);
		} else {
			displayTextLine(4,"Forage Behaviour: False,%i,%i", forageCmd.lSpeed, forageCmd.rSpeed);
		}

		if (followCmd.broadcasting) {
			displayTextLine(5,"Follow Behaviour: True,%i,%i", followCmd.lSpeed, followCmd.rSpeed);
		} else {
			displayTextLine(5,"Follow Behaviour: False,%i,%i", followCmd.lSpeed, followCmd.rSpeed);
		}

		if (avoidCmd.broadcasting) {
			displayTextLine(6,"Avoid Behaviour: True,%i,%i", avoidCmd.lSpeed, avoidCmd.rSpeed);
		} else {
			displayTextLine(6,"Avoid Behaviour: False,%i,%i", avoidCmd.lSpeed, avoidCmd.rSpeed);
		}

		if (observeCmd.broadcasting) {
			displayTextLine(7,"Observe Behaviour: True,%i,%i",  observeCmd.lSpeed, observeCmd.rSpeed);
		} else {
			displayTextLine(7,"Observe Behaviour: False,%i,%i",  observeCmd.lSpeed, observeCmd.rSpeed);
		}

		displayTextLine(9,"Current Longest Path: %i", longestPath);
		displayTextLine(10,"Program RunTime: %i", nPgmTime);
}

void tryCmd(cmdRequest cmd){

	//sets left and right speed depending on behaviour activity
	//overwrites values when higher level behaviours are checked
		if(cmd.broadcasting){
			leftSpeed = cmd.lSpeed;
			rightSpeed = cmd.rSpeed;
		}

}

task arbiter(){ //Behaviour arbitration
	while(true){
		//this is setting speed of motors depending which behaviour is active
		//level 0
		tryCmd(forageCmd);
		//level 1
		tryCmd(followCmd);
		//level 2
		tryCmd(avoidCmd);
		//level 3
		tryCmd(observeCmd);

		display();

		setMotorSpeed(leftMotor, leftSpeed);
		setMotorSpeed(rightMotor, rightSpeed);
	}
}

task forage(){ //forage behaviour
	forageCmd.name = "Forage";
	while(true){

		sleep(50);
		//always true as is the level 0 behaviour
		forageCmd.broadcasting = true;
		forageCmd.rSpeed = 50;

		//set motor speed to spiral around the inside of the track to find the track
		if (forageCmd.lSpeed <= 50){
			forageCmd.lSpeed += spiralFactor;
		}
		else if(forageCmd.lSpeed >= 50){
			forageCmd.lSpeed=0;
		}
    
    if (foundLine){
      forageCmd.lSpeed = 0; 
    }


	}
}

task adjustLightLevels(){ //Use max and min values from the two sliding windows
	while(true){
		sleep(100);

		if (foundLine){
			float sum = 0;
			for (int i = 0; i< windowSize-1; i++){
				lightLevel[i] = lightLevel[i+1];
				sum += lightLevel[i+1];
				if (lightLevel[i+1] < min){
					min = lightLevel[i+1];	
				} else if (lightLevel[i+1] > max){
					max = lightLevel[i+1];
				}
				
			}
			lightLevel[windowSize-1] = avg;
			if (avg < min){
				min = avg;	
			} else if (avg > min){
				max = avg;	
			}
			sum += avg;
			float mean = sum / windowSize;
			setPt = mean;
			lightThreshold = (setPt + maxLight)/2;
		}
				
		maxCap = baseSpeed/maxLight;
		minCap = baseSpeed/minLight;
	}

}

task follow() { //follow behaviour

	followCmd.name = "Follow";
	clearTimer(T2);
	while(true){

		HTCS2readRawWhite(S3, true, avg);

		//Get time since previous cycle
		dt = time1(T2);
		clearTimer(T2);


		errD = (avg - errPrev) / dt; //Differential error
		errI += dt * errPrev; //Integral error
		errP = setPt - avg; //Proportional error

		//secondDeriv = (errD - errDPrev) / dt;

		//errDPrev = errD;
		errPrev = avg;

		if(errP < 0){ //If the light value is too dark
			followCmd.rSpeed = baseSpeed + maxCap * ( (kP * errP) + (kI * errI) + (kD * errD) );
			followCmd.lSpeed = baseSpeed - maxCap * ( (kP * errP) + (kI * errI) + (kD * errD) );
		}
		else if (errP >= 0){
			followCmd.rSpeed = baseSpeed + minCap * ( (kP * errP) + (kI * errI) + (kD * errD) );
			followCmd.lSpeed = baseSpeed - minCap * ( (kP * errP) + (kI * errI) + (kD * errD) );
		}

		if(kI * errI >= 20){ //Avoid integral wind-up
			errI = 0;
		}

		//Start tracking time since we lost the line
		if(avg >= lightThreshold && foundLine){
			//Start timer
			foundLine = false;
			clearTimer(T3);
		}

		//If we still haven't relocated the line after 2sec of losing it
		// the behaviour relinquishes motor control until it is found again.
		if(!foundLine && time1(T3) >= 2000){
			followCmd.lSpeed = 0;
			followCmd.rSpeed = 0;
			sleep(100);
			followCmd.broadcasting = false;
		}

		//Line has been relocated
		if((avg <= (setPt + setPtTolerance)) && (!foundLine)){
			followCmd.broadcasting = true; //Request motor control again
			foundLine = true;
			wipeError(); //Reset errors
			//Rotate on the spot to line up with track
			//repeatUntil(avg >= setPt){ /*needs help*/
			//	followCmd.lSpeed = 3;
			//	followCmd.rSpeed = -3;
			//}
		}

		sleep(sampleLength); //Aim for dt=sampleLength
	}
}
task avoid(){ //avoid behaviour
	while (true){

		//get left distance and right distance
		long leftDist = getUSDistance(leftSonar);
		long rightDist = getUSDistance(rightSonar);

  	//detect object at 15cm or less and set avoid to active
  	if (leftDist < 15){
    	avoidCmd.broadcasting = true;
    	avoidCmd.lSpeed = 0;
    	avoidCmd.rSpeed = 0;
  	}

  	//if we have seen an object run this section of code
  	//this will override every other behaviour
  	if (avoidCmd.broadcasting){
  		//clear timer1
  		//turn right until robot no longer sees the object
  		clearTimer(T1);
  		repeatUntil(leftDist >= 25){
  			leftDist = getUSDistance(leftSonar);
				rightDist = getUSDistance(rightSonar);
  			avoidCmd.lSpeed = baseSpeed;
				avoidCmd.rSpeed = -baseSpeed;
  		}
  		//calculate how long the robot turned right whilst trying to avoid object
  		long time = time1(T1);

  		//once clear of object drive straight for 8 seconds, set value for robot avoid
  		clearTimer(T1);
			repeatUntil(time1(T1) >= 8000){
  			avoidCmd.lSpeed = baseSpeed;
				avoidCmd.rSpeed = baseSpeed;
			}

			//turn left for 1.7 times the length of the time we took to avoid object originally
			clearTimer(T1);
			repeatUntil(time1(T1) >= time*1.67){
  			avoidCmd.lSpeed = -baseSpeed;
				avoidCmd.rSpeed = baseSpeed;
  		}

  		//drive straight for 9 seconds to get back to track, set value for robot avoid
  		clearTimer(T1);
			repeatUntil(time1(T1) >= 9000){
  			avoidCmd.lSpeed = baseSpeed;
				avoidCmd.rSpeed = baseSpeed;
			}

		//turn back to original direction
		clearTimer(T1);
			repeatUntil(time1(T1) >= time*0.67){
  			avoidCmd.lSpeed = baseSpeed;
				avoidCmd.rSpeed = -baseSpeed;
  		}

			//once this procedure of avoiding is done, stop broadcasting avoid
			avoidCmd.broadcasting = false;
			avoidCmd.lSpeed = 0;
			avoidCmd.rSpeed = 0;

		}
	}

}

task observe(){ //observe Behaviour
	while (true){
		//once found the line and on a new straight start timer to
		//see how long the robot is on this straight for
		if (foundLine && newStraight){
			clearTimer(T4);
			newStraight = false;
			resetGyro(gyro);
		}

		if (getGyroDegrees(gyro) >= 45){
			longestPath = time1(T4);
			newStraight = true;
			resetGyro(gyro);
		}

		//pause when your own the longest straight
		if ((longestPath != 0) && (time1(T4) >= longestPath/2) && (nPgmTime >= 60000)){
			observeCmd.broadcasting = true;
			observeCmd.lSpeed = 0;
			observeCmd.rSpeed = 0;
		}

	}
}

task main(){

	//get white value after button press
	displayTextLine(0, "Press for white");
  waitForButtonPress();
  
	HTCS2readRawWhite(S3, true, maxLight);
	sleep(300);

	//get black value after button press
	displayTextLine(0, "White: %f", maxLight);
	displayTextLine(2, "Press for Black");
	waitForButtonPress();

	HTCS2readRawWhite(S3, true, minLight);
	displayTextLine(2, "Black: %f", minLight);
	sleep(300);

	//get target light/set point after button press
	displayTextLine(4, "Press for set point");
	waitForButtonPress();

	HTCS2readRawWhite(S3, true, setPt);
	displayTextLine(4, "Set pt: %f", setPt);
	sleep(300);
  
	cutoff = setPt+200; //???
	//values to stop steering being too great or too little on extreme light changes
	maxCap = baseSpeed/maxLight;
	minCap = baseSpeed/minLight;

	for(int i=0; i<windowSize; i++){ 
		lightLevel[i] = setPt;
		//loLightReadings[i] = minLight;
		//hiLightReadings[i] = maxLight;
	}

	//start robot after button press
  	displayTextLine(6, "Press to Start Robot");
	waitForButtonPress();
	sleep(200);


	//start behaviours
	startTask(arbiter);
	startTask(forage);
	startTask(follow);
	startTask(avoid);
	startTask(observe);
	startTask(adjustLightLevels);
	eraseDisplay();

	while (true){

	}
  
}

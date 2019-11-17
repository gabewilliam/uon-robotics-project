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
long maxLight, minLight;

long lightThreshold = 1100;

float errP, errI, errD, errPrev, maxCap, minCap;
long setPt = 500;
float setPtTolerance = 50;

//PID Coefficients
const float kP = 1.4;
const float kI = 0.000002;
const float kD = 0.5;

const float baseSpeed =30;
float leftSpeed = 0;
float rightSpeed = 0;
float spiralFactor = 0.2;
float timeSinceLostLine = 0;

//bool for 
bool paused = false;

float dt = 0;
const float sampleLength = 50;

/* Timers
	T1: Object avoidance
	T2: PID error monitor
	T3: Track time since lost line
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

void wipeError(){
	errP = 0;
	errI = 0;
	errD = 0;
}

void display(cmdRequest cmd){

		displayTextLine(0, cmd.name);
}

void tryCmd(cmdRequest cmd){
	if (!paused){
		if(cmd.broadcasting){
			leftSpeed = cmd.lSpeed;
			rightSpeed = cmd.rSpeed;
			display(cmd);
		}
	} else {
		leftSpeed = 0;
		rightSpeed = 0;
		displayTextLine(0,"Current Light: %f",avg);
		displayTextLine(1,"Current Behaviour: %s", cmd.name);
		displayTextLine(2,"Current Left Speed: %f", cmd.lSpeed);
		displayTextLine(3,"Current Right Speed: %f", cmd.rSpeed);
		displayTextLine(5,"Current Target: %f", setPt);
		displayTextLine(6,"Current errP,errI,errD: %f,%f,%f", errP,errI,errD);
	}
}

task arbiter(){ //Behaviour arbitration
	while(true){
		tryCmd(forageCmd);
		tryCmd(followCmd);
		tryCmd(avoidCmd);
		tryCmd(observeCmd);
		setMotorSpeed(leftMotor, leftSpeed);
		setMotorSpeed(rightMotor, rightSpeed);
	}
}

task forage(){ //forage behaviour
	forageCmd.name = "Forage";
	while(true){

		sleep(100);

		forageCmd.broadcasting = true;
		forageCmd.rSpeed = 30;

		if (leftSpeed < 30){
			forageCmd.lSpeed = leftSpeed + spiralFactor;
		}

	}
}

task follow() { //follow behaviour

	followCmd.name = "Follow";
	clearTimer(T2);
	while(true){

		HTCS2readRawWhite(S3, true, avg);

		dt = time1(T2);
		clearTimer(T2);

		errD = (avg - errPrev) / dt;
		errI += dt * errPrev;
		errP = setPt - avg;

		errPrev = avg;
		if(errP < 0){
			followCmd.rSpeed = baseSpeed + maxCap * ( (kP * errP) + (kI * errI) + (kD * errD) );
			followCmd.lSpeed = baseSpeed - maxCap * ( (kP * errP) + (kI * errI) + (kD * errD) );
		}
		else if (errP >= 0){
			followCmd.rSpeed = baseSpeed + minCap * ( (kP * errP) + (kI * errI) + (kD * errD) );
			followCmd.lSpeed = baseSpeed - minCap * ( (kP * errP) + (kI * errI) + (kD * errD) );
		}

		if(kI * errI >= 20){
			errI = 0;
		}

		if(avg >= lightThreshold && foundLine){
			//Start timer
			foundLine = false;
			clearTimer(T3);
		}

		if(!foundLine && time1(T3) >= 2000){
			followCmd.lSpeed = 0;
			followCmd.rSpeed = 0;
			sleep(100);
			followCmd.broadcasting = false;
		}

		if((avg <= ( setPt + setPtTolerance)) && (!foundLine)){
			followCmd.broadcasting = true;
			foundLine = true;
			wipeError();
		}

		sleep(sampleLength);
	}
}

task avoid(){ //avoid behaviour
	while (true){

	long leftDist = getUSDistance(leftSonar);
	long rightDist = getUSDistance(rightSonar);

  //detect object and start to reverse
  if (leftDist < 15){
    avoidCmd.broadcasting = true;
    avoidCmd.lSpeed = 0;
    avoidCmd.rSpeed = 0;
    sleep(1000);
  }

  if (avoidCmd.broadcasting){
  	clearTimer(T1);
  	repeatUntil(leftDist >= 25){
  		leftDist = getUSDistance(leftSonar);
		rightDist = getUSDistance(rightSonar);
  		avoidCmd.lSpeed = baseSpeed;
		avoidCmd.rSpeed = -baseSpeed;
  	}
  	long time = time1(T1)*1.7;

  	clearTimer(T1);
	repeatUntil(time1(T1) >= 8000){
  		avoidCmd.lSpeed = baseSpeed;
		avoidCmd.rSpeed = baseSpeed;
	}
	clearTimer(T1);
	repeatUntil(time1(T1) >= time){
  		avoidCmd.lSpeed = -baseSpeed;
		avoidCmd.rSpeed = baseSpeed;
  	}
  //	repeatUntil(leftDist <= 25){
  //			lSpeed = botSpeed;
		//		rSpeed = botSpeed;
		//}

  	clearTimer(T1);
	repeatUntil(time1(T1) >= 9000){
  		avoidCmd.lSpeed = baseSpeed;
		avoidCmd.rSpeed = baseSpeed;
	}

	avoidCmd.broadcasting = false;
	}
}

}

task observe(){ //observe Behaviour
	while (true){
		//make sure to follow line.
		//check wheels havent turned more than a given amount
		//if they have then stop timer and record time if bigger than last recorded time
		//reset timer
		//something something stop when you reach this timer value
	}
}

task calculateErrors(){

	dt = time1(T2);
	clearTimer(T2);

	errD = (errP - errPrev) / dt;
	errI += dt * errPrev;

	errPrev = errP;

	sleep(sampleLength);

}

task pause(){
	
	while(true){
		repeatUntil(getButtonPress(buttonEnter)==0){
			paused = true;	
		}
		
		repeatUntil(getButtonPress(buttonEnter)==0){
			paused = false;	
		}
	}
}

task main(){

	//startTask(calculateErrors);
	while(getButtonPress(buttonEnter)==0){
		displayTextLine(0, "Press for white");
	}

	HTCS2readRawWhite(S3, true, maxLight);
	sleep(500);

	while(getButtonPress(buttonEnter)==0){
		displayTextLine(0, "White: %f", maxLight);
		displayTextLine(2, "Press for Black");
	}

	HTCS2readRawWhite(S3, true, minLight);
	displayTextLine(2, "Black: %f", minLight);
	sleep(500);

	while(getButtonPress(buttonEnter)==0){
		displayTextLine(4, "Press for set point");
	}

	HTCS2readRawWhite(S3, true, setPt);
	displayTextLine(4, "Set pt: %f", setPt);
	sleep(500);

	lightThreshold = maxLight;
	//setPt = (black + white)/2;

	maxCap = baseSpeed/maxLight;
	minCap = baseSpeed/minLight;

	while(getButtonPress(buttonEnter)==0){
		displayTextLine(6, "Press to Start Robot");
	}

	sleep(200);


	startTask(arbiter);
	startTask(forage);
	startTask(follow);
	startTask(avoid);
	startTask(pause);
	startTask(observe);
	eraseDisplay();

	while (true){
	

	}
}

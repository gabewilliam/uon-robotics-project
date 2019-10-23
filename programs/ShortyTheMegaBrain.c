#pragma config(Sensor, S1,     gyro,           sensorEV3_Gyro, modeEV3Gyro_RateAndAngle)
#pragma config(Sensor, S2,     rightSonar,     sensorEV3_Ultrasonic)
#pragma config(Sensor, S3,     lightSensor,    sensorI2CCustom)
#pragma config(Sensor, S4,     leftSonar,      sensorEV3_Ultrasonic)
#pragma config(Motor,  motorA,          rightMotor,    tmotorEV3_Large, PIDControl, encoder)
#pragma config(Motor,  motorC,          leftMotor,     tmotorEV3_Large, PIDControl, encoder)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//
#include "HitechnicColorSensor.h"

bool foundLine = false;
long r,g,b,avg;

long lightThreshold = 2000;

float errP, errI, errD, errPrev;
float setPt = 480;
float setPtTolerance = 40;

//PID Coefficients
const float kP = 0.020;
const float kI = 0.012;
const float kD = 0.017;

const float baseSpeed = 13;
float leftSpeed = 0;
float rightSpeed = 0;
float spiralFactor = 0.5;
float timeSinceLostLine = 0;

float dt = 0;
const float sampleLength = 100;

/* Timers 
	T1: Object avoidance 
	T2: PID rrror monitor
	T3: Track time since lost line
*/

//Command request struct for behaviours to request motor control
typedef struct {
	float lSpeed;
	float rSpeed;
	bool broadcasting=false;
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

void tryCmd(cmdRequest cmd){
	if(cmd.broadcasting){
		leftSpeed = cmd.lSpeed;
		rightSpeed = cmd.rSpeed;
	}
}

long turnFixed(cmdRequest cmd, long angle, int dir){ //Turn by a fixed amount
	resetGyro(gyro); 
	repeatUntil(getgyroDegrees(gyro) >= angle){
		cmd.lSpeed = 20 * dir;
		cmd.rSpeed = -20 * dir;
	}
	return getgyroDegrees();
}

task arbiter(){ //Behaviour arbitration
	while(true){
		tryCmd(forageCmd); 
		tryCmd(followCmd); 
		tryCmd(avoidCmd);  
		tryCmd(observeCmd);
	}
}

task forage(){
	while(true){
		
		sleep(100);

		forageCmd.broadcasting = true;
		forageCmd.rSpeed = 30;

		if (leftSpeed < 30){
			forageCmd.lSpeed = leftSpeed + spiralFactor;
		}
		
	}
}

task follow() {
	while(true){
			
		HTCS2readRawRGB(S3, true, r,g,b);
		avg = (r + g + b)/3;

		errP = setPt - avg;

		followCmd.rSpeed = baseSpeed + ( (kP * errP) + (kI * errI) + (kD * errD) );
		followCmd.lSpeed = baseSpeed - ( (kP * errP) + (kI * errI) + (kD * errD) );
		
		if(avg >= lightThreshold && foundLine){
			//Start timer
			foundLine = false;
			clearTimer(T3);
		}
		
		if(foundLine = false && time1(T3) >= 3000){
			followCmd.lSpeed = 0;
			followCmd.rSpeed = 0;
			sleep(100);
			followCmd.broadcasting = false;
		}
		
		if((avg <= ( setPt + setPtTolerance)) && (!foundLine)){
			followCmd.broadcasting = true;
			followCmd.lSpeed = 0;
			followCmd.rSpeed = 0;
			sleep(100);
			foundLine = true;
			wipeError();
		}
	}
}

task avoid(){
	
	bool avoiding = false;
	avoidCmd.broadcasting = false;
	long time = 0;
	
	while (true){
  
		if (getUSDistance(leftSonar) <15 || getUSDistance(rightSonar) <15 ){
			avoiding = true;
			avoidCmd.broadcasting = true;
		}
		
		avoidCmd.lSpeed = 0;
		avoidCmd.rSpeed = 0;
    
		repeatUntil(!avoiding){
      
			resetGyro(gyro);
      
			repeatUntil(getgyroDegrees(gyro) >= 90){
				avoidCmd.lSpeed = 30;
				avoidCmd.rSpeed = 0;
			}
      
			clearTimer(T1);
			time = time + 1000;
			
			repeatUntil(time1(T1) >= 1000){
				avoidCmd.lSpeed = 30;
				avoidCmd.rSpeed = 30;
			}
      
			resetGyro(gyro);
      
			repeatUntil(getgyroDegrees(gyro) <= 270){
				avoidCmd.lSpeed = 0;
				avoidCmd.rSpeed = 30;
			}
      
			if (getUSDistance(leftSonar) >15 || getUSDistance(rightSonar) >15 ){
				avoiding = false;     
				avoidCmd.broadcasting = false;
			} 
		}
    
		clearTimer(T1);
    
		repeatUntil(time1(T1)>1500){
			avoidCmd.lSpeed = 30;
			avoidCmd.rSpeed = 30;
		}
    
		resetGyro(gyro);
      
		repeatUntil(getgyroDegrees(gyro) <= 270){
			avoidCmd.lSpeed = 0;
			avoidCmd.rSpeed = 30;
		}
      
		clearTimer(T1);
    
		repeatUntil(time1(T1) >= time){
			avoidCmd.lSpeed = 30;
			avoidCmd.rSpeed = 30;
		}
    
		resetGyro(gyro);
      
		repeatUntil(getgyroDegrees(gyro) >= 90){
			avoidCmd.lSpeed = 30;
			avoidCmd.rSpeed = 0;
		}
	}
}


task tPropagator(){

	dt = time1(T2);
	clearTimer(T2);

	errD = (errP - errPrev) / dt;
	errI += dt * errPrev;

	errPrev = errP;

	sleep(sampleLength);

}

task main(){
	
	startTask(tPropagator);
	startTask(arbiter);
	startTask(forage);
	startTask(follow);
	startTask(avoid);

	while (true){

		setMotorSpeed(leftMotor, leftSpeed);
		setMotorSpeed(rightMotor, rightSpeed);

	}
}

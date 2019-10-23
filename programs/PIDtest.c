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

void wipeError(){
	errP = 0;
	errI = 0;
	errD = 0;
}

task arbiter(){ //Behaviour arbitration
	while(true){
		
		if(forageCmd.broadcasting){
			leftSpeed = forageCmd.lSpeed;
			rightSpeed = forageCmd.rSpeed;
		}
		if(followCmd.broadcasting) { //Line following subsumes forage
			leftSpeed = followCmd.lSpeed;
			rightSpeed = followCmd.rSpeed;
		}
		
	}
}

task forage(){
	while(true){
		
		sleep(100);

		forageCmd.broadcasting = true;
		forageCmd.rSpeed = 30;

		if (leftSpeed<30){
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
			followCmd.broadcasting = false;
		}
		
		if(avg <= setPt && !foundLine){
			followCmd.broadcasting = true;
			foundLine = true;
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

	while (true){

		setMotorSpeed(leftMotor, leftSpeed);
		setMotorSpeed(rightMotor, rightSpeed);

	}
}

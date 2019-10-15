#pragma config(Sensor, S1,     gyro,           sensorEV3_Gyro, modeEV3Gyro_RateAndAngle)
#pragma config(Sensor, S2,     rightSonar,     sensorEV3_Ultrasonic)
#pragma config(Sensor, S3,     lightSensor,    sensorI2CCustom)
#pragma config(Sensor, S4,     leftSonar,      sensorEV3_Ultrasonic)
#pragma config(Motor,  motorA,          rightMotor,    tmotorEV3_Large, PIDControl, encoder)
#pragma config(Motor,  motorC,          leftMotor,     tmotorEV3_Large, PIDControl, encoder)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//
#include "HitechnicColorSensor.h"

bool foundLine = false;
long r,g,b,avg,delta;
float leftSpeed = 0;
float rightSpeed = 0;
float spiralFactor = 0.5;
const float target = 480;
const float steeringMod = 0.03;
const float botSpeed = 10;

typedef struct {
	float lSpeed;
	float rSpeed;
	int priority;
} cmdRequest;

cmdRequest followCmd;
cmdRequest forageCmd;

task arbiter(){
	
	
	while(true){
		
		if(followCmd.priority > forageCmd.priority){
			leftSpeed = followCmd.lSpeed;
			rightSpeed = followCmd.rSpeed;
		}
		else if(followCmd.priority < forageCmd.priority){
			leftSpeed = forageCmd.lSpeed;
			rightSpeed = forageCmd.rSpeed;
		}
		
	}
	
	
}

task forage(){
	
	while(true){
		if(!foundLine){
			
			forageCmd.priority = 3;
			forageCmd.rSpeed = 40;
			
			sleep(300);
			
			if (leftSpeed<40){
				forageCmd.lSpeed = leftSpeed + spiralFactor;
			}

			HTCS2readRawRGB(S3, true, r,g,b);
			avg = (r+g+b)/3;

			if (avg <=  target){
				foundLine = true;
			}
		}
		else{
			
			forageCmd.lSpeed = 0;
			forageCmd.rSpeed = 0;
			forageCmd.priority = 1;
			
			if(avg >= 2000){
				foundLine = false;
			}
			
			sleep(250);
			
		}
	}
	
}

task follow() {
	
	while(true){
		
		followCmd.priority = 2;
		
		while (foundLine){

			HTCS2readRawRGB(S3, true, r,g,b);
			avg = (r + g + b)/3;
			delta = target - avg;

			followCmd.rSpeed = botSpeed + (delta * steeringMod);
			followCmd.lSpeed = botSpeed - (delta * steeringMod);
			
		}
		sleep(250);
	}
	
}

task main(){
	
	startTask(arbiter);
	startTask(forage);
	startTask(follow);

	while (true){

		setMotorSpeed(leftMotor, leftSpeed);
		setMotorSpeed(rightMotor, rightSpeed);

	}

}
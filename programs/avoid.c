#pragma config(Sensor, S1,     gyro,           sensorEV3_Gyro, modeEV3Gyro_RateAndAngle)
#pragma config(Sensor, S2,     rightSonar,     sensorEV3_Ultrasonic)
#pragma config(Sensor, S3,     lightSensor,    sensorI2CCustom)
#pragma config(Sensor, S4,     leftSonar,      sensorEV3_Ultrasonic)
#pragma config(Motor,  motorA,          rightMotor,    tmotorEV3_Large, PIDControl, encoder)
#pragma config(Motor,  motorC,          leftMotor,     tmotorEV3_Large, PIDControl, encoder)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//
#include "HitechnicColorSensor.h"

long botSpeed = 10;
long lSpeed = 10;
long rSpeed = 10;
bool avoidRight = false;
bool avoidLeft = false;

task avoid(){
	while(true){
		if (getUSDistance(leftSonar) <= 15){
			lSpeed = botSpeed;
			rSpeed = botSpeed-(getUSDistance(leftSonar)-15);
			avoidLeft = true;
		} else if (getUSDistance(rightSonar) <= 15){
			lSpeed = botSpeed-(getUSDistance(rightSonar)-15;
			rSpeed = botSpeed;
			avoidRight = true;
		} 
		if (getUSDistance(leftSonar) < 5){
			lSpeed = botSpeed;
			rSpeed = -botSpeed;
		} else if (getUSDistance(rightSonar) < 5){
			lSpeed = -botSpeed;
			rSpeed = botSpeed;
		}
			
		if (avoidRight && getUSDistance(rightSonar) > 15){
			clearTimer(T1);
			repeatUntil(Time1(T1){
				lSpeed = botSpeed;
				rSpeed = botSpeed;
			}
				    
			lSpeed = 0;
			rSpeed = 0;
		}
				
		if (avoidLeft && getUSDistance(leftSonar) > 15){
			clearTimer(T1);
			repeatUntil(Time1(T1){
				lSpeed = botSpeed;
				rSpeed = botSpeed;
			}
				    
			lSpeed = 0;
			rSpeed = 0;
		}
	}
}

task main(){
	
	while(true){
		setMotorSpeed(leftMotor, lSpeed);
		setMotorSpeed(leftMotor, rSpeed);
	}
}

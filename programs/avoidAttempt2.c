#pragma config(Sensor, S1,     gyro,           sensorEV3_Gyro, modeEV3Gyro_RateAndAngle)
#pragma config(Sensor, S2,     rightSonar,     sensorEV3_Ultrasonic)
#pragma config(Sensor, S3,     lightSensor,    sensorI2CCustom)
#pragma config(Sensor, S4,     leftSonar,      sensorEV3_Ultrasonic)
#pragma config(Motor,  motorA,          rightMotor,    tmotorEV3_Large, PIDControl, encoder)
#pragma config(Motor,  motorC,          leftMotor,     tmotorEV3_Large, PIDControl, encoder)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//
#include "HitechnicColorSensor.h"

long botSpeed = 10;
bool foundLine = true;
bool avoidActive = false;
long leftSpeed = botSpeed;
long rightSpeed = botSpeed;

task avoid() {
	bool avoidDone = false;
	while (true){
		sleep(200);
		//change this
		while(foundLine || avoidActive){	
			
			if (!avoidDone){
			
				if (getUSDistance(leftSonar) < 15){
					
					displayTextLine(1, "avoid");
					avoidActive = true;
					leftSpeed = botSpeed +(30 - getUSDistance(leftSonar));
					rightSpeed = botSpeed - (15- getUSDistance(leftSonar));
				}
			//add repatuntil for time to avoid
				if (getUSDistance(leftSonar) > 15 && avoidActive){
					avoidDone = true;
					clearTimer(T1);
					leftSpeed = botSpeed;
					rightSpeed = botSpeed;
					
				}
			
			} else {
				if (time1(T1) >= 1500){
					leftSpeed=botSpeed;
					rightSpeed = botSpeed+10;
				}
			//turn back onto track and use code to check on track.	

			}
		}
	}
}


task main(){
	startTask(avoid);
	while (true){
		     setMotorSpeed(leftMotor, leftSpeed);
    	  setMotorSpeed(rightMotor, rightSpeed);
		
}
}

#pragma config(Sensor, S1,     gyro,           sensorEV3_Gyro, modeEV3Gyro_RateAndAngle)
#pragma config(Sensor, S2,     rightSonar,     sensorEV3_Ultrasonic)
#pragma config(Sensor, S3,     lightSensor,    sensorI2CCustom)
#pragma config(Sensor, S4,     leftSonar,      sensorEV3_Ultrasonic)
#pragma config(Motor,  motorA,          rightMotor,    tmotorEV3_Large, PIDControl, encoder)
#pragma config(Motor,  motorC,          leftMotor,     tmotorEV3_Large, PIDControl, encoder)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//
#include "HitechnicColorSensor.h"

task main(){
	clearTimer(T1);
  while (true){

  	repeatUntil(time1(T1) >= 4500){
  		setMotorSpeed(leftMotor, 20);
    	setMotorSpeed(rightMotor, 20);
  	}

  	setMotorSpeed(leftMotor, 0);
    setMotorSpeed(rightMotor, 0);

    clearTimer(T1);

    repeatUntil(time1(T1) >= 965){
  		setMotorSpeed(leftMotor, 20);
    	setMotorSpeed(rightMotor, -20);
  	}

  	clearTimer(T1);
  }

}

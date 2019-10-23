task avoid(){
bool avoiding = false;
long time = 0;
  while (true){
  
    if (getUSDistance(leftSonar) <15 || getUSDistance(rightSonar) <15 ){
    
      avoiding = true;      
    }
    
    repeatUntil(!avoiding){
      
      resetGyro(gyro);
      
      repeatUntil(getGryoDegrees(gryo) >= 90){
        setMotorSpeed(leftMotor, 100);
    	  setMotorSpeed(rightMotor, 0);
      }
      
      clearTimer(T1);
      time = time + 1000;
      repeatUntil(time1(T1) >= 1000){
        setMotorSpeed(leftMotor, 100);
    	  setMotorSpeed(rightMotor, 100);
      }
      
      resetGyro(gyro);
      
      repeatUntil(getGryoDegrees(gryo) <= 270){
        setMotorSpeed(leftMotor, 0);
    	  setMotorSpeed(rightMotor, 100);
      }
      
      if (getUSDistance(leftSonar) >15 || getUSDistance(rightSonar) >15 ){
        avoiding = false;      
      } 
    }
    
    clearTimer(T1);
    
    repeatUntil(time1(T1)>1500){
      setMotorSpeed(leftMotor, 100);
    	setMotorSpeed(rightMotor, 100);
    }
    
    resetGyro(gyro);
      
    repeatUntil(getGryoDegrees(gryo) <= 270){
        setMotorSpeed(leftMotor, 0);
    	  setMotorSpeed(rightMotor, 100);
    }
      
    clearTimer(T1);
    
    repeatUntil(time1(T1) >= time){
      setMotorSpeed(leftMotor, 100);
    	setMotorSpeed(rightMotor, 100);
    }
    
     resetGyro(gyro);
      
      repeatUntil(getGryoDegrees(gryo) >= 90){
        setMotorSpeed(leftMotor, 100);
    	  setMotorSpeed(rightMotor, 0);
      }
      
  }

}

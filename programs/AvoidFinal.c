bool avoidBool = false;
bool backUp = false;

task avoid(){
while (true){

  //detect object and start to reverse
  if (getUSDistance(leftSonar) < 10 || getUSDistance(rightSonar) < 10){
    bool avoidBool = true;
    bool backUp = true;
  }

  //reverse until 20 cm away 
  if(backUp && (getUSDistance(leftSonar) < 20 || getUSDistance(rightSonar) < 20)){
    lSpeed = -botSpeed;
    rSpeed = -botSpeed;
  }
  
  //turn right 
  if(avoidBool && (getUSDistance(leftSonar) < 20 || getUSDistance(rightSonar) < 20)){
    lSpeed = botSpeed;
    rSpeed = -botSpeed;
  }
  
  //spiral around after the first spiral we no longer reverse to backUP becomes false
  if(avoidBool && (getUSDistance(leftSonar) > 20 || getUSDistance(rightSonar) > 20)){
    backUP = false;
    lSpeed = 8;
    rSpeed = 12;
  }
  
  //stop avoiding if we find the line
  if (FoundLine Code && avoidBool){
    avoidBool = false;
  }
    
}
}

bool avoidBool = false;
bool backUp = false;

task avoid(){
while (true){

  //detect object and start to reverse
  if (getUSDistance(leftSonar) < 10 || getUSDistance(rightSonar) < 10){
    bool avoidBool = true;
    bool backUp = true;
  }

  //reverse
  if(backUp && (getUSDistance(leftSonar) < 20 || getUSDistance(rightSonar) < 20)){
    lSpeed = -botSpeed;
    rSpeed = -botSpeed;
  }
  
  //turn right 
  if(avoidBool && (getUSDistance(leftSonar) < 20 || getUSDistance(rightSonar) < 20)){
    lSpeed = botSpeed;
    rSpeed = -botSpeed;
  }
  
  //spiral around
  if(avoidBool && (getUSDistance(leftSonar) > 20 || getUSDistance(rightSonar) > 20)){
    backUP = false;
    lSpeed = 8;
    rSpeed = 12;
  }
  
  if (FoundLine Code && avoidBool){
    avoidBool = false;
  }
    
}
}

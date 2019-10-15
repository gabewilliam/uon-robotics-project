task avoid() {
	while(true){
		if (getUSDistance(leftSonar) <10 || getUSDistance(rightSonar) <10 ){
			leftSpeed=0;
			rightSpeed=0;
		}
	} 
} 

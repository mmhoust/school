// Counter

class Counter {
private:
	int count; // limit to 0 through 4
	// 0 = strongly not taken 1 = weakly not taken 2 = weakly taken 3 = strongly taken
public:
	Counter(){
		count = 0;
	}
	const void increment(){
		if(count == 0){ 
			count = 1;
		}else if(count == 1){
			count = 2;
		}else if(count == 2){
			count = 3;
		}else if(count == 3){
			count = 3;
		}
	}	
	const void decrement(){
		if(count == 0){
			count = 0;
		}else if(count == 1){
			count = 0;
		}else if (count == 2){
			count = 1;
		}else if (count == 3){
			count = 2;
		}
	}
	const int getCount(){
		return count;
	}

};


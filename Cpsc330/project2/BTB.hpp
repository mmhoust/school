// BTB table
#include "Counter.hpp"
class BTB {
private:
	int BHSR;
	Counter table[16];
public:
	BTB(){
		BHSR = 0;
		for(int i = 0; i < 16; i++){
			Counter temp;
			table[i] = temp;
		}
	}
	int getBHSR(){
		return BHSR;
	}
	int getTaken(){
		Counter temp = table[BHSR];
		if(temp.getCount() == 0 || temp.getCount() == 1){
			return 0;
		}else{
			return 1;
		}
	}
	void setTaken(int taken){

		if(taken == 0) {
			table[BHSR].decrement();
		}else{
			table[BHSR].increment();
		}
	}
	void incrementBHSR(int taken){
		int mask = 15;
		BHSR = BHSR << 1;
		BHSR = BHSR | taken;
		BHSR = BHSR & mask;
	}
};
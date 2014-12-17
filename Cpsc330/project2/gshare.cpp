//gshare
#include "Counter.hpp"
#include <map>
#include <fstream>
#include <iostream>
#include <vector>

int main(int argc,char**argv){
	
	std::vector<unsigned int> addresses;
	std::vector<int> takens;
	unsigned int BHSR = 0;
	int taken;
	unsigned int address;
	int predictions, mispredicts;
	mispredicts = 0;
	predictions = 0;

	int count =0;
	
	//std::cout << mask << std::endl;
 	while(count < 4000000){
 		std::cin >> std::hex >> address;
 		addresses.push_back(address);
 		std::cin >> taken;
 		takens.push_back(taken);
 		count++;
 	}

	int isTaken = 0;
	for(int bits = 2; bits < 17; bits++){
		std::map<int,Counter> PHT;
		unsigned int mask = 0xffffffff  >>  (32-bits);
		std::cout << mask << std::endl;
		for(int j=0; j< count; j++){
			address = addresses[j];
			address = address & mask;
			address = address ^ BHSR;
			taken = takens[j];
			
			if (PHT[address].getCount() == 0 || PHT[address].getCount() == 1){
				isTaken = 0;
			}else if(PHT[address].getCount() == 2 || PHT[address].getCount() == 3){
				isTaken = 1;
			}
			if(taken != isTaken){
				//std::cout << taken << " " << isTaken << std::endl;
				mispredicts++;
			}
			if(taken == 0){
				PHT[address].decrement();
				BHSR = BHSR << 1;
				BHSR = (BHSR | 0) & mask;
			}else{
				PHT[address].increment();
				BHSR = BHSR << 1;
				BHSR = (BHSR | 1) & mask;
			}
			
			predictions++;
		
		}

		float percent = ((float)mispredicts / (float)predictions) * 100;
		std::cout << "Bits =" << bits << " mispredicts =" << mispredicts << " rate = ";
		std::cout << percent << '%' << std::endl ;
		mispredicts = 0;
		predictions = 0;
		BHSR = 0;
	}
	
}
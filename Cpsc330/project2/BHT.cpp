// 2 bit BHT counter 

#include "Counter.hpp"
#include <map>
#include <fstream>
#include <iostream>
#include <vector>

int main(int argc,char**argv){
	
	std::vector<unsigned int> addresses;
	std::vector<int> takens;
	
	int taken;
	unsigned int address;
	int predictions, mispredicts;
	mispredicts = 0;
	predictions = 0;

	int count = 0;
	
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
		unsigned int mask = 0xffff >> (16 - bits);
		std::map<int,Counter> BHT;
		for(int j=0; j< count; j++){
			address = addresses[j];
			address = address & mask;

			taken = takens[j];
			
			if (BHT[address].getCount() == 0 || BHT[address].getCount() == 1){
				isTaken = 0;
			}else if(BHT[address].getCount() == 2 || BHT[address].getCount() == 3){
				isTaken = 1;
			}
			if(taken != isTaken){
				//std::cout << taken << " " << isTaken << std::endl;
				mispredicts++;
			}
			if(taken == 0){
				BHT[address].decrement();
			}else{
				BHT[address].increment();
			}
			
			predictions++;
		
		}

		float percent = ((float)mispredicts / (float)predictions) * 100;
		std::cout << "Bits =" << bits << " mispredicts =" << mispredicts << " rate = ";
		std::cout << percent << '%' << std::endl ;
		mispredicts = 0;
		predictions = 0;
	}
	
}
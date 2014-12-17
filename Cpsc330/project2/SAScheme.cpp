// SAs scheme
#include "BTB.hpp"
#include <map>
#include <fstream>
#include <iostream>
#include <cmath>
#include <vector>
int main(int argc, char**argv){
	unsigned int address;
	int taken;
	std::vector<unsigned int> addresses;
	std::vector<int> takens;
	std::map<int,BTB> table;
	int mispredicts = 0;
	int predictions  = 0;
	int count = 0;
	
	while(count < 4000000){
 		std::cin >> std::hex >> address;
 		addresses.push_back(address);
 		std::cin >> taken;
 		takens.push_back(taken);
 		count++;
 	}
 	int bit = 9;
 	predictions = 0;
 	mispredicts = 0;
 	for(int i =0; i<pow(2,bit); i++){
 		BTB temp;
 		table[i] = temp;
 	}
 	unsigned int mask = 0xffffffff >> (32 -bit);
 	for(int i = 0; i<count; i++){
 		address = addresses[i];
 		taken = takens[i];
 		address = address & mask;
 		BTB temp= table[address];
 		int t = temp.getTaken();
 		if(t != taken){
 			mispredicts++;
 		}
 		temp.setTaken(taken);
 		temp.incrementBHSR(taken);
 		table[address] = temp;
 		predictions++;
 	}
	float percent = (float) mispredicts / (float) predictions * 100;
	std::cout << "mispredicts = " << mispredicts << "rate = " << percent << '%' << std::endl;
 	
}
// parser.h

#include <fstream>
#include <vector>

struct Record {
	int address;
	bool taken;
};

class Parser {
	void addToVector(std::Vector<Record>,std::ifstream);
}
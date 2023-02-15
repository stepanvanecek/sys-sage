#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <sstream>
#include "sys-sage.hpp"



void parseMusa(); 

class Musaparser {
public:
	int parseData(); 
	Musaparser(std::string datapath);

private:
	int readData(std::vector<std::string>);
	Memory* parseMemory();
	Cache* parseCache(std::string, Component* parent);
	std::map<std::string, std::map<std::string, std::string>> mapping;
	std::string datapath;
	//int parseGlobal_information();
	Chip* topo = new Chip(); 
	
};

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <sstream>
#include "sys-sage.hpp"
#include "trial.h"




void parseMusa() {

	std::string datapath = "mn4.config"; 

	Musaparser parse(datapath);
	int ret = parse.parseData(); 

	if (ret) {
		std::cout << " the file couldn't be parsed" << std::endl; 
	}
	else {
		std::cout << " the file was succesfully parsed " << std::endl; 
	}
}

Musaparser::Musaparser(std::string filelocation) {
	datapath = filelocation; 
}

std::ostream& operator<<(std::ostream& output, std::map<std::string, double>& map) {
	std::map<std::string, double>::iterator that = map.begin();


	// Iterate through the map and print the elements
	while (that != map.end())
	{
		output << "Key: " << that->first << ", Value: " << that->second << std::endl;
		++that;
	}
	return output; 
}

int Musaparser::readData(std::vector<std::string> search) {

	std::ifstream myfile(datapath);
	if (!myfile.good()) {
		std::cout << " Couldn't open data source path" << std::endl;
		return 1;
	}
	std::string line, subline, word;
	int offset;
	std::string value;
	for (const auto element : search) {
		myfile.clear();
		myfile.seekg(0, myfile.beg);
		while (!myfile.eof()) {
			std::getline(myfile, line);
			if ((offset = line.find(element, 0) != std::string::npos)) {
				std::map<std::string, std::string> input;
				do {
					std::getline(myfile, subline);
					subline.erase(remove(subline.begin(), subline.end(), '='), subline.end());
					std::istringstream ss(subline);
					ss >> word >> value;
					input.insert({ word,value });


				} while (!subline.empty());

				mapping.insert({ element, input });
			}

		}

	}
	
	return 0; 
}



int Musaparser::parseData() {
	std::vector<std::string> search = { "DL1Cache","Global", "Memory", "L2Cache", "L3Cache", "RAMULATOR"};
	int test = readData(search); 
	if (test != 0) {
		std::cout << "Was not able to parse information from file" << std::endl; 
		return 1; 
	}

	for (auto element : search) {
		if (mapping.find(element) == mapping.end()) {
			std::cout << " Couldn't find " << element << " information in file" << std::endl; 
			return 1; 
		}
	}
	double band, latency; 
	Memory* mem = parseMemory(); 
	std::map<std::string, std::string> data = mapping["Memory"];
	latency = stod(data["latency"]);
	band = stod(data["bandwidth"]);
	if (stod(data["latency"]) > -1) {
		
	}
	if (stod(data["bandwidth"]) > -1) {
		
	}

	Cache* l3cache = parseCache("L3Cache", mem);
	int ncpus = stoi(mapping["Global"]["ncpus"]); 
	int nthreads= stoi(mapping["Global"]["threads_per_cpu"]);
	for (int i = 0; i < ncpus; i++) {
		Cache* l2cache = parseCache("L2Cache", l3cache); 
		Cache* l1cache = parseCache("DL1Cache", l2cache);
		Core* core = new Core(l1cache); 
		for (int i = 0; i < nthreads; i++) {
			Thread* thread = new Thread(core, i); 
			DataPath(mem, thread, 1, band, latency); 
			DataPath(l1cache, thread, 1, band, latency);
			DataPath(l2cache, thread, 1, band, latency);
			DataPath(l3cache, thread, 1, band, latency);
		}

	}

	return 0; 

	

}

Memory* Musaparser::parseMemory() {
	static Memory* mem = new Memory(topo); 
	
    // parsing memory size from the 
	std::string input = mapping["RAMULATOR"]["org"]; 
	int pos1 = input.find("_");
	int pos2 = input.rfind("_");
	std::string output = input.substr(pos1 + 1, pos2 - pos1 - 1);
	std::string number_str = "";
	std::string letter_str = "";
	for (char c : output) {
		if (isdigit(c)) {
			number_str += c;
		}
		else {
			letter_str += c;
		}
	}
	int number = stoi(number_str);
	mem->SetSize(number); 
	std::string* suffix = new std::string(letter_str); 
	mem->attrib.insert({ " Size_Suffix", (void*)suffix }); 

	return mem; 
}

Cache* Musaparser::parseCache(std::string level, Component* parent) {
	static Cache* cache = new Cache(parent);
	std::map<std::string, std::string> data = mapping[level];
	cache->SetCacheSize(stol(data["size"])); 
	cache->SetCacheLineSize(stoi(data["line-size"]));
	double* assoc = new double(stod(data["assoc"]));
	cache->attrib.insert({ "assoc",assoc }); 
	double* type = new double(stod(data["level"]));
	cache->attrib.insert({ "level",type });
		
	return cache;
}

//int mn4topo::parseGlobal_information() {
//	if (mapping.find("Global") == mapping.end()) {
//		std::cout << "Could find Gloabl information" << std::endl;
//	}
//	else {
//		std::map<std::string, double> data = mapping["Global"];
//		std::cout << data << std::endl;
//	}
//
//	return 0; 
//}


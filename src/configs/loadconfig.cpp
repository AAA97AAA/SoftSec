#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iterator>
#include <sstream>

#include "loadconfig.h"

using namespace std;

ConfigManager::ConfigManager(string confPath): confPath(confPath) {
}

void ConfigManager::loadConfigFile() {
	clearArgs();
	ifstream confFile(confPath);
	vector<string> c_args;

	if (!confFile) {
		cerr << "Unable to open grass.conf" << endl;
		exit(1);
	}

	string line;
	string noSpaceLine;
	while(getline(confFile, line)) {
		if (line.size() > 0) {
			noSpaceLine = leftStrip(line);
			if (noSpaceLine.at(0) != '#') {
				c_args.push_back(noSpaceLine);
			}
		}
	}
	confFile.close();

	/* 
	 * Using auto with UL in the for loops gives i the type long
	 * unsigned int, which is the same type size() returns.
	 * */
	for (auto i=0UL; i<c_args.size();i++) {
		string val = c_args[i];
		vector<string> tmp_vec = getArgs(val);
		list_args[tmp_vec[0]].push_back(tmp_vec[1]);
	}
}

vector<string> ConfigManager::getArgs(string text) {
	vector<string> res;
	stringstream ss;
	string tmp;
	string tmp2;
	ss << text;
	ss >> tmp;
	getline(ss, tmp2);
	res.push_back(tmp);
	res.push_back(leftStrip(tmp2));
	return res;
}

string ConfigManager::leftStrip(string text) {
	string res;
	string tmp_str;
	stringstream ss;
	res = "";
	ss << text;
	while (!ss.eof()) {
		ss >> tmp_str;
		res = res+tmp_str+" ";
	}
	res.pop_back();
	return res;
}

vector<string> ConfigManager::getParams(string name) {
	if (list_args.find(name) == list_args.end()) {
		throw std::runtime_error("Parameter "+name+" not in config file.");
	}
	return list_args[name];
}

void ConfigManager::clearArgs() {
	list_args.clear();
}

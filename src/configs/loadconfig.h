#pragma once

#include <vector>
#include <unordered_map>

using namespace std;

class ConfigManager {
public:
	ConfigManager(string confPath);
	void loadConfigFile();
	vector<string> getParams(string name);

protected:
	string leftStrip(string text);
	vector<string> getArgs(string text);
	void clearArgs();

private:
	string confPath;
	unordered_map<string, vector<string>> list_args;
};

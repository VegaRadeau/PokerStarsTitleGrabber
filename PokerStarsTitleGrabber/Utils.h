#pragma once

#include <string>
#include <vector>

std::vector<std::string> strSplit(std::string stringToBeSplitted, std::string delimeter);
void LogMessage(const char* errStr, unsigned logLvl);
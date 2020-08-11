#include "stdafx.h"

#include "Utils.h"


std::string lastErrStr = "";


void LogMessage(const char* errStr, unsigned logLvl)
    {
    // TODO: tidy this mess up
    if (logLvl < 0 || logLvl > 1)
        return;

    if (!logLvl)
        {
        // TODO: tidy this up
        fprintf(stdout, errStr);
        fprintf(stdout, "\n");
        return;
        }

    std::string errStdStr(errStr);

    if (errStdStr == lastErrStr)
        return;

    fprintf(stderr, errStr);
    fprintf(stdout, "\n");

    lastErrStr = errStdStr;
    }


std::vector<std::string> strSplit(std::string stringToBeSplitted, std::string delimeter)
    {
    std::vector<std::string> splittedString;
    size_t startIndex = 0;
    size_t endIndex = 0;
    while ((endIndex = stringToBeSplitted.find(delimeter, startIndex)) < stringToBeSplitted.size())
        {
        std::string val = stringToBeSplitted.substr(startIndex, endIndex - startIndex);
        splittedString.push_back(val);
        startIndex = endIndex + delimeter.size();
        }

    if (startIndex < stringToBeSplitted.size())
        {
        std::string val = stringToBeSplitted.substr(startIndex);
        splittedString.push_back(val);
        }

    return splittedString;
    }

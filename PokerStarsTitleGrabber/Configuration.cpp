#include "stdafx.h"

#include <filesystem>
#include <iostream>
#include <string>
#include <windows.h>

#include "Configuration.h"


int maxTablesToList = 0;
int numMainStages = 0;
bool replayStageEnabled = false;
bool enableReplayPrefix = false;
int maxTitleLength = 0;
std::vector<std::pair<int, int>> mainStagePositions;
std::pair<int, int> replayStagePosition = std::make_pair(-32768, -32768); // bogus co-ords jic


bool LoadInitilisationFile()
    {
    LPCTSTR config_path = _T(".\\grabber-config.ini");
    std::cout << "Attempting to read grabber-config.ini...\n";

    if (!std::filesystem::exists(config_path))
        {
        std::cout << "\n!!! error: Could not find grabber-config.ini next to the executable, you must create one! \n";
        return false;
        }

    // numofmainstages
    numMainStages = GetPrivateProfileInt(_T("grabber"), _T("numofmainstages"), 8 /*default*/, config_path);
    std::cout << "Number of Main Stages: " << numMainStages << "\n";
    

    // maxtablelistnum
    maxTablesToList = GetPrivateProfileInt(_T("grabber"), _T("maxtablelistnum"), 3 /*default*/, config_path);
    std::cout << "Max number of Tables to list: " << maxTablesToList << "\n";


    // enablereplaystage
    TCHAR enableReplayStageConfig[32];
    GetPrivateProfileString(_T("grabber"), _T("enablereplaystage"), _T("false") /*default*/, enableReplayStageConfig, 32, config_path);
    if (std::string(enableReplayStageConfig).find("true") != std::string::npos)
        {
        replayStageEnabled = true;
        std::cout << "Replay Stage enabled\n";
        }
    else
        std::cout << "Replay Stage disabled\n";


    // enablereplayprefix
    if (replayStageEnabled)
        {
        TCHAR enableReplayPrefixConfig[32];
        GetPrivateProfileString(_T("grabber"), _T("enablereplayprefix"), _T("false") /*default*/, enableReplayPrefixConfig, 32, config_path);
        if (std::string(enableReplayPrefixConfig).find("true") != std::string::npos)
            {
            enableReplayPrefix = true;
            std::cout << "Replay Stage prefix, 'Replay: ', enabled\n";
            }
        else
            std::cout << "Replay Stage prefix disabled\n";
        }


    // maxtabletitlelength
    maxTitleLength = GetPrivateProfileInt(_T("grabber"), _T("maxtabletitlelength"), 21 /*default*/, config_path);
    std::cout << "Max Table Title Character Length: " << maxTitleLength << "\n";

    for (int i = 1; i <= numMainStages; ++i)
        {
        std::string stageSection = "stage" + std::to_string(i);

        int stageX = GetPrivateProfileInt(stageSection.c_str(), _T("x"), 21 /*default*/, config_path);
        int stageY = GetPrivateProfileInt(stageSection.c_str(), _T("y"), 21 /*default*/, config_path);
        
        mainStagePositions.push_back(std::make_pair(stageX, stageY));
        std::cout << "Stage " << i << ": x = " << stageX << ", y = " << stageY << "\n";
        }

    if (replayStageEnabled)
        {
        int replayStageX = GetPrivateProfileInt(_T("replaystage"), _T("x"), 21 /*default*/, config_path);
        int replayStageY = GetPrivateProfileInt(_T("replaystage"), _T("y"), 21 /*default*/, config_path);

        replayStagePosition = std::make_pair(replayStageX, replayStageY);
        std::cout << "Replay Stage: x = " << replayStageX << ", y = " << replayStageY << "\n";
        }

    std::cout << "\nConfiguration read successfully\n\n";

    return true;
    }


int GetTableListNum()
    {
    return maxTablesToList;
    }


int GetNumMainStages()
    {
    return numMainStages;
    }


bool IsReplayStageEnabled()
    {
    return replayStageEnabled;
    }


bool WantReplayStagePrefix()
    {
    return enableReplayPrefix;
    }


int GetMaxTitleLength()
    {
    return maxTitleLength;
    }


std::vector<std::pair<int, int>> GetMainStagePositions()
    {
    return mainStagePositions;
    }


std::pair<int, int> GetReplayStagePosition()
    {
    return replayStagePosition;
    }


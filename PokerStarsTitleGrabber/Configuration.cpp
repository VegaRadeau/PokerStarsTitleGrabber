#include "stdafx.h"

#include <windows.h>

#include "Configuration.h"


int numTotalStages = 0;
int numMainStages = 0;
bool replayStageEnabled = false;
bool wantReplayPrefix = false;
int maxTitleLength = 0;
std::vector<std::pair<int, int>> mainStagePositions;
std::pair<int, int> replayStagePosition = std::make_pair(-32768, -32768); // bogus co-ords jic


bool LoadInitilisationFile()
    {
    // faked up configuration settings
    numTotalStages = 10;
    numMainStages = 6;
    replayStageEnabled = true;
    wantReplayPrefix = false;
    maxTitleLength = 21;

    mainStagePositions.push_back(std::make_pair(-7, 0));		// stage 1
    mainStagePositions.push_back(std::make_pair(635, 0));		// stage 2
    mainStagePositions.push_back(std::make_pair(1275, 0));		// stage 3
    mainStagePositions.push_back(std::make_pair(-7, 480));
    mainStagePositions.push_back(std::make_pair(635, 480));
    mainStagePositions.push_back(std::make_pair(1275, 480));

    replayStagePosition = std::make_pair(1275, 490);

    return true; // TODO: temp

    LPCTSTR config_path = _T(".\\grabber-config.ini");

    // TODO: add test for ini file existence

    numMainStages = GetPrivateProfileInt(_T("stages"), _T("numberoftablestolist"), 3 /*default*/, config_path);
    numTotalStages = GetPrivateProfileInt(_T("stages"), _T("numberofstages"), 6 /*default*/, config_path);

    TCHAR excludeReplayStage[32];
    int enablereplaystage = GetPrivateProfileString(_T("stages"), _T("enablereplaystage"), _T("true") /*default*/, excludeReplayStage, 32, config_path);

    //TCHAR excludeReplayStage[32];
    //int chars = GetPrivateProfileString(_T("replay stage"), _T("exludefromtablecount"), _T("true"), protocolChar, 32, path);
    //int chars = GetPrivateProfileString(_T("Stages"), _T("NumberOfStagesToList"), _T(""), protocolChar, 32, path);

    //if (!chars)
        // TODO: error handling

    return true;
    }


int GetStageListNum()
    {
    return numTotalStages;
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
    return wantReplayPrefix;
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


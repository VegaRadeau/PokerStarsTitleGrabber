#include "stdafx.h"

#include <windows.h>
#include <string>
#include <vector>
#include <regex>
#include <iostream>
#include <fstream>
#include <psapi.h>

#include "Configuration.h"
#include "PokerStarsTitleParsers.h"
#include "Utils.h"


#define VERSION_MAJOR 0
#define VERSION_MINOR 0
#define VERSION_PATCH 17


BOOL CALLBACK GetPokerStarsTableInfo(HWND hwnd, LPARAM lParam);


struct TableInfo
    {
    TableInfo():
        title(""),
        id(0)
        {}

    TableInfo(std::string title, int64_t id) :
        title(title),
        id(id)
        {}

    std::string title;
    int64_t id;
    };


bool compareTableId(const TableInfo &a, const TableInfo &b)
    {
    // descending order
    return a.id > b.id;
    }

struct CallBackTableParam
    {
    CallBackTableParam(int numMainStages):
        mReplayStageTitle("")
        {
        // we don't need to initialize the stage list
        // because the order isn't important

        for (int j = 0; j < numMainStages; ++j)
            mMainStagesTables.push_back(TableInfo());
        }


    // called after writing out the main stage txt files, so we don't
    // duplicate watching other tables in the stage list
    void EraseDuplicateTournyTitles()
        {
        std::vector<int64_t> testIDs;
        std::vector<TableInfo>::iterator it;

        for (it = mMainStagesTables.begin(); it != mMainStagesTables.end();)
            {
            if (!testIDs.size())
                {
                // first time through
                if ((*it).id != 0) // not a cash game
                    testIDs.push_back((*it).id);

                ++it;
                continue;
                }

            if ((*it).id != 0) // not a cash game
                {
                if (std::count(testIDs.begin(), testIDs.end(), (*it).id))
                    {
                    it = mMainStagesTables.erase(it);
                    continue;
                    }
                else
                    {
                    testIDs.push_back((*it).id);
                    }
                }

            ++it;
            }

        // first order the other tables in descending ID
        std::sort(mOtherTables.begin(), mOtherTables.end(), compareTableId);

        for (it = mOtherTables.begin(); it != mOtherTables.end();)
            {
            if (!testIDs.size())
                {
                // first time through
                if ((*it).id != 0) // not a cash game
                    testIDs.push_back((*it).id);

                ++it;
                continue;
                }

            if ((*it).id != 0) // not a cash game
                {
                if (std::count(testIDs.begin(), testIDs.end(), (*it).id))
                    {
                    it = mOtherTables.erase(it);
                    continue;
                    }
                }

            ++it;
            }
        }


    int NumTablesTotal()
        {
        int mainStageCnt = 0;

        for (size_t i = 0; i < mMainStagesTables.size(); ++i)
            if (mMainStagesTables[i].title.size())
                ++mainStageCnt;

        // should be called after EraseDuplicateTournyTitles() to avoid duplicates
        return mainStageCnt + static_cast<int>(mOtherTables.size());
        }


    std::vector<TableInfo> mMainStagesTables;
    std::vector<TableInfo> mOtherTables;
    std::string mReplayStageTitle; // no ID recorded for this
    };


int main()
    {
    std::cout << "PokerStars Title Grabber v" << VERSION_MAJOR << "." << VERSION_MINOR << "." << VERSION_PATCH << "\n";
    //std::cout << "Created by Dylan. Contact email: drasburn@gmail.com; PokerStars: Hitmonlee436\n";
    std::cout << "master commit: XXXXXXX\n\n";

    std::cout << "This program is intended to enchance the viewing experience of the poker streamer and fully complies\n";
    std::cout << "with the rules and guidelines set out in the Third Party Tools Reference Guide and at:\n";
    std::cout << "https://www.pokerstars.com/poker/room/prohibited/\n\n";
    

    if (!LoadInitilisationFile())
        {
        // TODO: there is probably a nicer way to do this
        std::string dummmy;
        std::cin >> dummmy;
        return 1;
        }

    ///////////////
    // Table Break Countdown Timer initialization
    //
    std::string currentChatScreenStr = "";
    std::string newChatScreenStr;
    int dotCnt = 0;
    bool breakCountdownStarted = false;
    int lastDotSecond = -1;

    ///////////////
    // Table Titles initialization
    //
    int currentTableCount(0);
    std::vector<std::string> currentTableListTitles;
    std::vector<std::string> currentMainStagesTitles;
    std::string currentReplayStageTitle;

    for (int i = 0; i < GetNumMainStages(); ++i)
        currentMainStagesTitles.push_back("");

    std::ofstream txtFileStream;
    std::string tableListTxtFile = "Table_List.txt";
    std::string replayStageTxtFile = "Replay_Stage.txt";
    std::string breakStageTxtFile = "Chat_Stage_Break_Message.txt";
    std::string tableCountTxtFile = "Table_Count.txt";
    std::vector<std::string> mainStagesTxtFiles;

    for (int i = 0; i < GetNumMainStages(); ++i)
        mainStagesTxtFiles.push_back("Stage_" + std::to_string(i + 1) + ".txt");

    // reset files to empty
    for (int i = 0; i < GetNumMainStages(); ++i)
        {
        // Reset all stages to empty
        txtFileStream.open(mainStagesTxtFiles[i]);
        txtFileStream << "";
        txtFileStream.close();
        std::cout << "Clearing " + mainStagesTxtFiles[i] + "\n";
        }

    // Reset stage list to empty
    txtFileStream.open(tableListTxtFile);
    txtFileStream << "";
    txtFileStream.close();
    std::cout << "Clearing " + tableListTxtFile + "\n";

    // Reset replay stage to empty
    txtFileStream.open(replayStageTxtFile);
    txtFileStream << "";
    txtFileStream.close();
    std::cout << "Clearing " + replayStageTxtFile + "\n";

    // Reset chat stage break message to empty
    txtFileStream.open(breakStageTxtFile);
    txtFileStream << "";
    txtFileStream.close();
    std::cout << "Clearing " + breakStageTxtFile + "\n";

    // clear table count text file
    txtFileStream.open(tableCountTxtFile);
    txtFileStream << "";
    txtFileStream.close();
    std::cout << "Clearing " + tableCountTxtFile + "\n";

    // main loop
    while (true)
        {
        Sleep(50); // sleep 50ms

        // TODO: finish this
        // ScanPayouts(&payouts);

        ///////////////
        // Table Break Countdown Timer
        //
        SYSTEMTIME time;
        GetSystemTime(&time);

        // break is at 55 minutes on the hour
        if (time.wMinute >= 55)
            {
            // tables are on break - countdown timer
            if (time.wMinute == 55 && time.wSecond == 0 && !breakCountdownStarted)
                {
                std::cout << "Tables are on break, starting countdown...\n";
                breakCountdownStarted = true;
                }

            int minCountdown = 59 - time.wMinute;
            int secCountdown = 59 - time.wSecond;

            std::string minCntdownStr = std::to_string(minCountdown);
            std::string secCntdownStr;

            if (secCountdown <= 9)
                secCntdownStr = "0" + std::to_string(secCountdown);
            else
                secCntdownStr = std::to_string(secCountdown);

            newChatScreenStr = "Tables currently on break. Action will resume in " + minCntdownStr + ":" + secCntdownStr;

            if (minCountdown == 0 && secCountdown == 0)
                {
                newChatScreenStr = "The action will resume momentarily.";
                dotCnt = 1;
                breakCountdownStarted = false;
                }
            }
        else if (time.wMinute <= 54)
            {
            if (lastDotSecond != time.wSecond)
                {
                lastDotSecond = time.wSecond;
                // The action will resume momentarily...
                newChatScreenStr = "The action will resume momentarily";

                if (dotCnt >= 3)
                    dotCnt = 1;
                else
                    ++dotCnt;

                for (int i = 0; i < dotCnt; ++i)
                    newChatScreenStr += ".";
                }
            }
        else
            {
            // empty string
            newChatScreenStr = "";
            dotCnt = 0;
            }

        if (newChatScreenStr != currentChatScreenStr)
            {
            txtFileStream.open(breakStageTxtFile);
            txtFileStream << newChatScreenStr;
            txtFileStream.close();
            currentChatScreenStr = newChatScreenStr;
            }

        ///////////////
        // Table Titles
        //
        CallBackTableParam cbTables(GetNumMainStages());

        EnumWindows(GetPokerStarsTableInfo, reinterpret_cast<LPARAM>(&cbTables));

        // write out new main stage table names if different
        for (size_t i = 0; i < cbTables.mMainStagesTables.size(); ++i)
            {
            if (cbTables.mMainStagesTables[i].title != currentMainStagesTitles[i])
                {
                //re-write individual stagename file
                txtFileStream.open(mainStagesTxtFiles[i]);
                txtFileStream << cbTables.mMainStagesTables[i].title;
                txtFileStream.close();

                currentMainStagesTitles[i] = cbTables.mMainStagesTables[i].title;

                std::cout << "Stage " << (i + 1) << ": ";
                std::cout << currentMainStagesTitles[i] << "\n";
                }
            }

        cbTables.EraseDuplicateTournyTitles();

        // Generate stage list
        std::vector<std::string> newTableListTitles;

        for (size_t i = 0; i < cbTables.mMainStagesTables.size(); i++)
            if (cbTables.mMainStagesTables[i].title.size())
                newTableListTitles.push_back(cbTables.mMainStagesTables[i].title);

        for (size_t i = 0; i < cbTables.mOtherTables.size(); ++i)
            newTableListTitles.push_back(cbTables.mOtherTables[i].title); // will not be empty

        int listSize = std::min<int>(static_cast<int>(newTableListTitles.size()), GetTableListNum());

        if (newTableListTitles != currentTableListTitles)
            {
            txtFileStream.open(tableListTxtFile);

            for (int i = 0; i < listSize; ++i)
                txtFileStream << newTableListTitles[i] << std::endl;

            if (!newTableListTitles.size())
                txtFileStream << ""; // clear stage list

            txtFileStream.close();
            currentTableListTitles = newTableListTitles;
            }

        // write out replay stage if different
        if (cbTables.mReplayStageTitle != currentReplayStageTitle)
            {
            txtFileStream.open(replayStageTxtFile);

            if (cbTables.mReplayStageTitle.size())
                txtFileStream << cbTables.mReplayStageTitle << std::endl;
            else
                txtFileStream << ""; // clear replay stage title

            txtFileStream.close();
            currentReplayStageTitle = cbTables.mReplayStageTitle;
            std::cout << "Replay Stage: " << currentReplayStageTitle << "\n";
            }


        // write out new table current if different
        if (cbTables.NumTablesTotal() != currentTableCount)
            {
            txtFileStream.open(tableCountTxtFile);
            txtFileStream << cbTables.NumTablesTotal();
            txtFileStream.close();

            currentTableCount = cbTables.NumTablesTotal();
            std::cout << "Table Count: " << currentTableCount << "\n";
            }
        } // main loop

    return 0;
    }


BOOL CALLBACK GetPokerStarsTableInfo(HWND hwnd, LPARAM lParam)
    {
    char wnd_title[MAX_PATH];
    GetWindowText(hwnd, wnd_title, sizeof(wnd_title));

    int length = ::GetWindowTextLength(hwnd);
    std::string wndTitle(wnd_title);

    if (!IsWindowVisible(hwnd) || length == 0 || wndTitle == "Program Manager")
        return TRUE;

    // test if pokerstar.exe
    DWORD dwPID;
    GetWindowThreadProcessId(hwnd, &dwPID);

    HANDLE Handle = OpenProcess(
        PROCESS_QUERY_LIMITED_INFORMATION,
        FALSE,
        dwPID
    );

    if (Handle == nullptr)
        return TRUE;

    char processBuffer[MAX_PATH];

    if (!GetModuleFileNameEx(Handle, 0, processBuffer, MAX_PATH))
        return TRUE;

    std::string process = std::string(processBuffer);
    std::string pokerStarsProcess = "PokerStars.exe";

    if (process.find(pokerStarsProcess) == std::string::npos)
        return TRUE;

    // Retrieve the pointer passed into this callback, and re-'type' it.
    // The only way for a C API to pass arbitrary data is by means of a void*.
    CallBackTableParam &tableInfo = *reinterpret_cast<CallBackTableParam*>(lParam);

    RECT windowPos = { NULL };
    GetWindowRect(hwnd, &windowPos);

    std::string tableTitle = GenerateTableTitle(wndTitle);
    int64_t tournyId = GetTournamentID(wndTitle);

    if (!tableTitle.size())
        return TRUE;

    std::vector<std::pair<int, int>> mainStagePoss = GetMainStagePositions();
    std::pair<int, int> replayStagePos = GetReplayStagePosition();

    for (size_t tableInd = 0; tableInd < mainStagePoss.size(); ++tableInd)
        {
        if (windowPos.left == mainStagePoss[tableInd].first
            && windowPos.top == mainStagePoss[tableInd].second)
            {
            if (tableInfo.mMainStagesTables[tableInd].title.size())
                {
                // then this table is stacked behind the top one
                // WS_EX_TOPMOST is set on the first one to pass through this callback
                }
            else
                {
                tableInfo.mMainStagesTables[tableInd] = TableInfo(tableTitle, tournyId);
                return TRUE;
                }
            }
        }

    if (windowPos.left == replayStagePos.first
            && windowPos.top == replayStagePos.second)
        {
        if (tableInfo.mReplayStageTitle.size())
            {
            // then this table is stacked behind the top one
            // WS_EX_TOPMOST is set on the first one to pass through this callback

             // there shouldn't be more than 1 here, but if there is we don't want to count them
            return TRUE;
            }
        else
            {
            tableInfo.mReplayStageTitle = tableTitle;

            if (WantReplayStagePrefix())
                tableInfo.mReplayStageTitle = "Replay: " + tableTitle;

            return TRUE;
            }
        }

    // not main stage or relay stage, but still open
    tableInfo.mOtherTables.push_back(TableInfo(tableTitle, tournyId));

    return TRUE;
    }

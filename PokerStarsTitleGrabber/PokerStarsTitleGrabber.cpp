// PokerStarsTitleGrabber.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <windows.h>
#include <string>
#include <vector>
#include <regex>
#include <iostream>
#include <fstream>
#include <psapi.h>

#define VERSION_MAJOR 0
#define VERSION_MINOR 0
#define VERSION_PATCH 9

// Toms current setup is to
// AHK three 'main stage' tables
// and three offscreen tables
// 1: -7, 0
// 2: 635, 0
// 3: 1275, 0
// 4: -7, 480
// 5: 635, 480
// 6: 1275, 480

// example tournament table title strings
// $7.00 NL Hold'em [27 - 180 Players, Turbo] - $75/$150 ante $15 - Tournament 2600843591 Table 4"
// Hot $109, $30K Gtd - $700/$1,400 ante $175 - Tournament 2600280240 Table 9
// $27 NLHE [6-Max, Hyper-Turbo], $10K Gtd - $800/$1,600 ante $400 - Tournament 2600272945 Table 43
// $2.20 Micro Monday 6-Max, $7.5K Gtd - $1,200/$2,400 ante $240 - Tournament 2600281666 Table 22
// $5.50 NLHE [Turbo, Progressive KO], $7.5K Gtd - $30,000/$60,000 ante $7,500 - Tournament 2600272547 Table 127
// $2.20 NLHE [Bubble Rush], $750 Gtd - $2,000/$4,000 ante $500 - Tournament 2600272628 Table 26
// $22 Mini Monday 6-Max, $80K Gtd - $600/$1,200 ante $120 - Tournament 2600281664 Table 3
// $4.40 NLHE [Progressive KO, Deep Stacks], $7.5K Gtd - $700/$1,400 ante $175 - Tournament 2600272689 Table 55
// Bounty Builder $33, $75K Gtd - $350/$700 ante $110 - Tournament 2600267926 Table 18
// $16.50 NLHE [Heads-Up, Turbo, Zoom, Progressive Total KO]
// $3.50+R NL Hold'em [180 Players, Turbo, Rebuy] - $4,000/$8,000 ante $800 - Tournament 2602906034 Table 7

// example cash game table title strings
// TODO:

struct winPos {
	int x;
	int y;
};

std::string GetActiveWindowTitle();
std::string GetActiveWindowProcess();
std::string GenerateTableTitle(std::string &title, bool isTournament);
std::string GenerateTournamentTitle(std::string &title);
std::string GenerateCashTitle(std::string &title);
std::string GetCashStakes(std::string stakesSearch);
std::vector<std::string> strSplit(std::string stringToBeSplitted, std::string delimeter);
bool AppendNewTitle(std::string &newTitle, std::string appendage);
winPos GetWindowPos();
int GetNumStages();
int GetNumMainStages();
int GetReplayStageNum(); // TODO: use somehow

int main()
{
	std::cout << "PokerStars Stream Helper v" << VERSION_MAJOR << "." << VERSION_MINOR << "." << VERSION_PATCH << "\n\n";

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
	int numStages = GetNumStages();
	int numMainStages = GetNumMainStages();

	std::ofstream txtFileStream;

	std::string mainStageTxtFile = "Main_Stages.txt";
	std::string breakStageTxtFile = "Chat_Stage_Break_Message.txt";

	std::vector<std::string> currentTableTitles;
	std::vector<std::string> currentTableTxtFiles;

	for (size_t i = 0; i < numStages; ++i)
	{
		currentTableTitles.push_back("");
		currentTableTxtFiles.push_back("Stage_" + std::to_string(i + 1) + ".txt");
	}

	// reset files to empty
	for (size_t i = 0; i < numStages; ++i)
	{
		// Reset all stages to empty
		txtFileStream.open(currentTableTxtFiles[i]);
		txtFileStream << "";
		txtFileStream.close();
		std::cout << "Clearing " + currentTableTxtFiles[i] + "\n";
	}

	// Reset main stage to empty
	txtFileStream.open(mainStageTxtFile);
	txtFileStream << "";
	txtFileStream.close();
	std::cout << "Clearing " + mainStageTxtFile + "\n";

	// Reset chat stage break message to empty
	txtFileStream.open(breakStageTxtFile);
	txtFileStream << "";
	txtFileStream.close();
	std::cout << "Clearing " + breakStageTxtFile + "\n";

	while (true)
	{
		Sleep(1); // sleep 1ms

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
		else if (time.wMinute <= 2)
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
			#if _DEBUG
			std::cout << currentChatScreenStr << "\n";
			#endif
		}

		///////////////
		// Table Titles
		//
		std::string activeWindowProcess = GetActiveWindowProcess();
		std::string pokerStarsProcess = "PokerStars.exe";

		if ((activeWindowProcess.find(pokerStarsProcess) == std::string::npos))
			continue;

		std::string activeWindowtitle = GetActiveWindowTitle();

		bool isTournament = false;
		std::string tournamentStr = "Tournament";

		if (activeWindowtitle.find(tournamentStr) != std::string::npos)
			isTournament = true;

		// check it's a cash game and not another pokerstars window
		if (!isTournament)
		{
			// All cash games mostly follow the format:
			// "Unique table name - big blind / small blind etc - game type"
			// game type can sometime contain more hyphens but they aren't usually seperated by spaces
			// e.g. '5-Card Draw'  
			// so we do a simple check for 2 hyphens inbetween 2 spaces

			std::vector<std::string> splitTitle;
			splitTitle = strSplit(activeWindowtitle, " - ");

			if (splitTitle.size() < 2)
				continue;
		}

		std::vector<std::string> newTableTitles;

		for (size_t i = 0; i < numStages; ++i)
			newTableTitles.push_back("");

		winPos windowPos;
		// found a pokerstars table
		windowPos = GetWindowPos();

		// stage 1
		if (windowPos.x == -7 && windowPos.y == 0)
			newTableTitles[0] = GenerateTableTitle(activeWindowtitle, isTournament);

		// stage 2
		if (windowPos.x == 635 && windowPos.y == 0)
			newTableTitles[1] = GenerateTableTitle(activeWindowtitle, isTournament);

		// stage 3
		if (windowPos.x == 1275 && windowPos.y == 0)
			newTableTitles[2] = GenerateTableTitle(activeWindowtitle, isTournament);

		// stage 4
		if (windowPos.x == -7 && windowPos.y == 480)
			newTableTitles[3] = GenerateTableTitle(activeWindowtitle, isTournament);

		// stage 5
		if (windowPos.x == 635 && windowPos.y == 480)
			newTableTitles[4] = GenerateTableTitle(activeWindowtitle, isTournament);

		// stage 6 - replay stage
		if (windowPos.x == 1275 && windowPos.y == 480)
			newTableTitles[5] = "Replay: " + GenerateTableTitle(activeWindowtitle, isTournament);

		for (size_t i = 0; i < currentTableTitles.size(); ++i)
		{
			if (!newTableTitles[i].empty() && (newTableTitles[i] != currentTableTitles[i]))
			{
				//re-write individual stagename file
				txtFileStream.open(currentTableTxtFiles[i]);
				txtFileStream << newTableTitles[i];
				txtFileStream.close();

				currentTableTitles[i] = newTableTitles[i];
				
				std::cout << "Stage " << (i+1) << ": ";
				std::cout << newTableTitles[i] << "\n";

				if (i < numMainStages)
				{
					txtFileStream.open(mainStageTxtFile);

					for (size_t j = 0; j < numMainStages; ++j)
						txtFileStream << currentTableTitles[j] << std::endl;

					txtFileStream.close();
				}
			}
		}
	}

    return 0;
}

std::string GetActiveWindowTitle()
{
	char wnd_title[256];
	HWND hwnd = GetForegroundWindow(); // get handle of currently active window
	GetWindowText(hwnd, wnd_title, sizeof(wnd_title));
	return wnd_title;
}

std::string GetActiveWindowProcess()
{
	char wnd_title[MAX_PATH];
	HWND hwnd = GetForegroundWindow(); // get handle of currently active window
	GetWindowText(hwnd, wnd_title, sizeof(wnd_title));

	DWORD dwPID;
	GetWindowThreadProcessId(hwnd, &dwPID);

	HANDLE Handle = OpenProcess(
		PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
		FALSE,
		dwPID
	);

	char processBuffer[MAX_PATH];

	if (Handle)
		if (!GetModuleFileNameEx(Handle, 0, processBuffer, MAX_PATH))
			return "";

	return processBuffer;
}

winPos GetWindowPos()
{
	RECT rect = { NULL };
	winPos windowPos;

	HWND hwnd = GetForegroundWindow(); // get handle of currently active window
	GetWindowRect(hwnd, &rect);

	windowPos.x = rect.left;
	windowPos.y = rect.top;

	return windowPos;
}

std::string GenerateTableTitle(std::string &title, bool isTournament)
{
	if (isTournament)
		return GenerateTournamentTitle(title);
	else
		return GenerateCashTitle(title);
}

std::string GenerateTournamentTitle (std::string &title)
{

	// homegames
	if (title.find("The Palace Open") != std::string::npos)
		return "The Palace Open";
	if (title.find("Slumdog Championship") != std::string::npos)
		return "Slumdog Championship";

	std::string newTitle = "";

	// TODO: handle freerolls

	const std::regex buyinRegex("([$])([0-9]+).?([0-9]+)");
	std::smatch buyinMatch;
	std::regex_search(title, buyinMatch, buyinRegex);

	if (buyinMatch.size() == 0)
		return newTitle;

	std::string buyin = buyinMatch[0].str();

	// chop off ".00" if it's there
	if (buyin.substr(buyin.length() - 3) == ".00")
		buyin = buyin.substr(0, buyin.length() - 3);

	// rebuy is special because we append it to buyin
	if (title.find("Rebuy") != std::string::npos || title.find("+R") != std::string::npos) { AppendNewTitle(buyin, "+R"); }

	// event tournaments
	if (title.find("SCOOP") != std::string::npos) { buyin += " SCOOP"; }
	if (title.find("UFC KO") != std::string::npos) { buyin += " UFC"; }
	if (title.find("MicroMillions") != std::string::npos) { buyin += " uMilly"; }

	// regular names
	if (title.find("Big ") != std::string::npos)
		return "Big " + buyin;
	if (title.find("Bigger ") != std::string::npos)
		return "Bigger " + buyin;
	if (title.find("Hot ") != std::string::npos)
		return "Hot " + buyin;
	if (title.find("Hotter ") != std::string::npos)
		return "Hotter " + buyin;
	
	// prefixes for special names
	std::string prefix = "";

	//if (title.find("Mini") != std::string::npos)
	//	prefix = " Mini";
	//if (title.find("Micro") != std::string::npos)
	//	prefix = " Micro";
	//if (title.find("High Roller Club:") != std::string::npos)
	//	prefix = " HRC:";

	// special daily names
	if (title.find("Bounty Builder") != std::string::npos)
		return buyin + " Bounty Builder";
	if (title.find("Jumbo Shrimp") != std::string::npos)
		return buyin + " Jumbo Shrimp";
	if (title.find("Battle Royale") != std::string::npos)
		return buyin + prefix + " Battle Royale";
	if (title.find("Double Deuce") != std::string::npos)
		return buyin + " Double Deuce";
	if (title.find("Micro Madness") != std::string::npos)
		return buyin + " Micro Madness";
	if (title.find("Daily Pacific Rim") != std::string::npos)
		return buyin + prefix + " Pacific Rim";
	if (title.find("Triple Threat") != std::string::npos)
		return buyin + prefix + " Triple Threat";
	if (title.find("Daily Cooldown") != std::string::npos)
		return buyin + " Cooldown";
	if (title.find("Daily Supersonic") != std::string::npos)
		return buyin + " Supersonic";
	if (title.find("Splash") != std::string::npos)
		return buyin + " Splash";

	// Sundays
	if (title.find("Sunday Storm") != std::string::npos)
		return buyin + " Sunday Storm";
	if (title.find("Sunday Million") != std::string::npos || title.find("SUNDAY MILLION") != std::string::npos)
		return buyin + " Sunday Million";
	if (title.find("Sunday Starter") != std::string::npos)
		return buyin + " Sunday Starter";
	if (title.find("Sunday Kickoff") != std::string::npos)
		return buyin + " Sunday Kickoff";
	if (title.find("Sunday Marathon") != std::string::npos)
		return buyin + " Sunday Marathon";
	if (title.find("Sunday Stack") != std::string::npos)
		return buyin + " Sunday Stack";
	if (title.find("Sunday Wrap-Up") != std::string::npos)
		return buyin + " Sunday Wrap-Up";
	if (title.find("Sunday Pacific Rim") != std::string::npos)
		return buyin + " Pacific Rim";

	// Mondays
	if (title.find("Monday 6-Max") != std::string::npos)
		return buyin + " Monday 6-Max";

	// Tuesdays
	if (title.find("Super Tuesday") != std::string::npos)
		return buyin + " Super Tuesday";

	// Wednesdays

	// Thursdays
	if (title.find("Fat Thursday") != std::string::npos)
		return buyin + " Fat Thursday";
	if (title.find("Thursday Thrill") != std::string::npos)
		return buyin + " Thursday Thrill";

	// Fridays
	if (title.find("Fast Friday") != std::string::npos)
		return buyin + " Fast Friday";

	// Saturdays
	if (title.find("Saturday KO") != std::string::npos)
		return buyin + " Saturday KO";
	if (title.find("Saturday Slam") != std::string::npos)
		return buyin + " Saturday Slam";
	if (title.find("Saturday Duel") != std::string::npos)
		return buyin + " Saturday Duel";
	if (title.find("Saturday Eliminator") != std::string::npos)
		return buyin + " Saturday Eliminator";
	if (title.find("Saturday Rebuy") != std::string::npos)
		return buyin + " Saturday Rebuy";
	if (title.find("Saturday Scuffle") != std::string::npos)
		return buyin + " Saturday Scuffle";
	if (title.find("Saturday Spider") != std::string::npos)
		return buyin + " Saturday Spider";
	if (title.find("Saturday Speedway") != std::string::npos)
		return buyin + " Saturday Speedway";

	//special SNGs
	if (title.find("Spin & Go") != std::string::npos)
		return buyin + " Spin & Go";

	// for piecing together generic tournaments
	newTitle = buyin;

	// special prizes
	if (title.find(" Sat ") != std::string::npos || title.find(" Satellite ") != std::string::npos) { if (!AppendNewTitle(newTitle, " Sat")) { return newTitle; }; }

	// KO's
	if (title.find("Progressive Total KO") != std::string::npos) { if (!AppendNewTitle(newTitle, " PTKO")) { return newTitle; }; }
	if (title.find("Progressive KO") != std::string::npos) { if (!AppendNewTitle(newTitle, " PKO")) { return newTitle; }; }
	else if (title.find("Progressive KO") != std::string::npos) { if (!AppendNewTitle(newTitle, " KO")) { return newTitle; }; }

	// table sizes
	if (title.find("8-Max") != std::string::npos) { if (!AppendNewTitle(newTitle, " 8-Max")) { return newTitle; }; }
	if (title.find("6-Max") != std::string::npos) { if (!AppendNewTitle(newTitle, " 6-Max")) { return newTitle; }; }
	if (title.find("4-Max") != std::string::npos) { if (!AppendNewTitle(newTitle, " 4-Max")) { return newTitle; }; }
	if (title.find("3-Max") != std::string::npos) { if (!AppendNewTitle(newTitle, " 3-Max")) { return newTitle; }; }
	if (title.find("Heads-Up") != std::string::npos) { if (!AppendNewTitle(newTitle, " HU")) { return newTitle; }; }

	// formats
	if (title.find("Zoom") != std::string::npos) { if (!AppendNewTitle(newTitle, " Zoom")) { return newTitle; }; }
	if (title.find("Time Tourney") != std::string::npos) { if (!AppendNewTitle(newTitle, " Timebomb")) { return newTitle; }; }
	if (title.find("Shootout") != std::string::npos) { if (!AppendNewTitle(newTitle, " Shootout")) { return newTitle; }; }
	if (title.find("Bubble Rush") != std::string::npos) { if (!AppendNewTitle(newTitle, " Bubble Rush")) { return newTitle; }; }
	if (title.find("Win the Button") != std::string::npos) { if (!AppendNewTitle(newTitle, " Win the Button")) { return newTitle; }; }
	if (title.find("1R1A") != std::string::npos) { if (!AppendNewTitle(newTitle, " 1R1A")) { return newTitle; }; }
	if (title.find("Re-Entry") != std::string::npos) { if (!AppendNewTitle(newTitle, " Re-Entry")) { return newTitle; }; }
	if (title.find("Deep Stacks") != std::string::npos) { if (!AppendNewTitle(newTitle, " Deep Stacks")) { return newTitle; }; }
	if (title.find("Deep Payouts") != std::string::npos) { if (!AppendNewTitle(newTitle, " Deep Payouts")) { return newTitle; }; }
	//if (title.find("Deep]") != std::string::npos) { if (!AppendNewTitle(newTitle, " Deep")) { return newTitle; }; }
	if (title.find("Big Antes") != std::string::npos) { if (!AppendNewTitle(newTitle, " Big Antes")) { return newTitle; }; }
	else if (title.find("Antes") != std::string::npos) { if (!AppendNewTitle(newTitle, " Antes")) { return newTitle; }; }

	// speeds
	if (title.find("Hyper-Turbo") != std::string::npos) { if (!AppendNewTitle(newTitle, " Hyper")) { return newTitle; }; }
	else if (title.find("Turbo") != std::string::npos) { if (!AppendNewTitle(newTitle, " Turbo")) { return newTitle; }; }

	// Got this far means it might be a SitNG and Player Caps
	if (title.find(" Player") != std::string::npos)
	{
		std::string playerStr = title.substr(0, title.find(" Player"));

		const std::regex playersRegex("([0-9]+)");
		std::smatch playerCntMatch;

		std::string::const_iterator searchStart (playerStr.cbegin());

		std::string playerCnt;

		while (std::regex_search(searchStart, playerStr.cend(), playerCntMatch, playersRegex))
		{
			playerCnt = playerCntMatch[0].str();
			searchStart = playerCntMatch.suffix().first;
		}

		std::string playerAppendage = " " + playerCnt + "-Man";
		AppendNewTitle(newTitle, playerAppendage);
	}

	// no special names
	if (title.find("NLHE") != std::string::npos) { if (!AppendNewTitle(newTitle, " NL")) { return newTitle; }; }
	if (title.find("NL Hold'em") != std::string::npos) { if (!AppendNewTitle(newTitle, " NL")) { return newTitle; }; }

	return newTitle;
}

std::string GenerateCashTitle(std::string &title)
{
	std::vector<std::string> splitTitle;
	splitTitle = strSplit(title, " - ");

	std::string newTitle = "";
	std::string stakes = "";
	std::string gameType = "";

	// handle typical cases - order matters
	if (splitTitle.back().find("Limit Hold'em") != std::string::npos) { gameType = "Limit HE"; }
	if (splitTitle.back().find("No Limit Hold'em") != std::string::npos) { gameType = "NLHE"; }
	if (splitTitle.back().find("Pot Limit Omaha") != std::string::npos) { gameType = "PLO"; }
	if (splitTitle.back().find("Limit Omaha Hi/Lo") != std::string::npos) { gameType = "Limit Omaha"; }
	if (splitTitle.back().find("No Limit Omaha Hi/Lo") != std::string::npos) { gameType = "NL Omaha"; }
	if (splitTitle.back().find("Pot Limit Omaha Hi/Lo") != std::string::npos) { gameType = "PLO Hi/Lo"; }
	if (splitTitle.back().find("Pot Limit 5 Card Omaha") != std::string::npos) { gameType = "PLO 5-Card"; }
	if (splitTitle.back().find("Limit 5 Card Omaha Hi/Lo") != std::string::npos) { gameType = "Limit Omaha 5-Card"; }
	if (splitTitle.back().find("Pot Limit Courchevel") != std::string::npos) { gameType = "PL Courchevel"; }
	if (splitTitle.back().find("Limit Stud") != std::string::npos) { gameType = "Limit Stud"; }
	if (splitTitle.back().find("Limit Razz") != std::string::npos) { gameType = "Limit Razz"; }
	if (splitTitle.back().find("Limit 5-Card Draw") != std::string::npos) { gameType = "Limit 5-Card Draw"; }
	if (splitTitle.back().find("Pot Limit 5-Card Draw") != std::string::npos) { gameType = "PL 5-Card Draw"; }
	if (splitTitle.back().find("No Limit 5-Card Draw") != std::string::npos) { gameType = "NL 5-Card Draw"; }
	if (splitTitle.back().find("No Limit 2-7 Single Draw") != std::string::npos) { gameType = "NL 2-7 Single Draw"; }
	if (splitTitle.back().find("Pot Limit Triple Draw 2-7 Lowball") != std::string::npos) { gameType = "PL 2-7 Triple Draw"; }
	if (splitTitle.back().find("Limit Triple Draw 2-7 Lowball") != std::string::npos) { gameType = "Limit 2-7 Triple Draw"; }
	if (splitTitle.back().find("Limit Badugi") != std::string::npos) { gameType = "Limit Badugi"; }

	// heads up cash tables
	if (splitTitle.back().find("1 on 1") != std::string::npos) 
	{ 
		if (splitTitle.end()[-2].find("Limit Hold'em") != std::string::npos) { gameType = "Limit HE HU"; }
		if (splitTitle.end()[-2].find("No Limit Hold'em") != std::string::npos) { gameType = "NLHE HU"; }
		if (splitTitle.end()[-2].find("Pot Limit Omaha") != std::string::npos) { gameType = "PLO HU"; }
	}

	if (gameType.length())
	{
		stakes = GetCashStakes(splitTitle[1]);
		newTitle = stakes;
		if (AppendNewTitle(newTitle, gameType))
			return newTitle;
		else
			return gameType;
	}

	// into the edge case cash game tables: TODO:

	newTitle = "Cash Game";

	return newTitle;
}

std::string GetCashStakes(std::string stakesSearch)
{
	std::string stakes = "Cash";
	const std::regex stakesRegex("([$])([0-9]+).?([0-9]+)?[/]([$])([0-9]+).?([0-9]+)?");
	std::smatch stakesMatch;
	std::regex_search(stakesSearch, stakesMatch, stakesRegex);
	stakes = stakesMatch[0].str();

	//TODO: figure out a better way of removing trailing whitespace
	if (stakes.length())
		if (stakes.back() == ' ')
			stakes = stakes.substr(0, stakes.length() - 1);

	stakes += " ";

	return stakes;
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

bool AppendNewTitle(std::string &newTitle, std::string appendage)
{
	std::string testTile = newTitle + appendage;
	int maxTitleLen = 21;

	if (testTile.length() > maxTitleLen)
		return false;

	newTitle = testTile;

	return true;
}

int GetNumStages() 
{
	return 6;
}

int GetNumMainStages()
{
	return 3;
}

int GetReplayStageNum()
{
	return 6;
}
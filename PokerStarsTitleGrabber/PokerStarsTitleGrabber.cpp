// PokerStarsTitleGrabber.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <windows.h>
#include <string>
#include <vector>
#include <regex>
#include <iostream>
#include <fstream>

// Toms current setup is to
// AHK three 'main stage' tables
// and three offscreen tables
// 1: -7, 0
// 2: 635, 0
// 3: 1275, 0
// 4: -7, 480
// 5: 635, 480
// 6: 1275, 480

// example strings
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

struct winPos {
	int x;
	int y;
};

std::string GetActiveWindowTitle();
std::string GenerateTableTitle(std::string &title);
winPos GetWindowPos();

int main()
{
	int numStages = 6;
	int numMainStages = 3;

	std::vector<std::string> currentTableTitles;
	std::vector<std::string> currentTableTxtFiles;
	
	for (size_t i = 0; i < numStages; ++i)
	{
		currentTableTitles.push_back("");
		currentTableTxtFiles.push_back("Stage_" + std::to_string(i + 1) + ".txt");
	}

	std::ofstream currentTablesTxtFileStream;

	while (true)
	{
		Sleep(5); // sleep 5ms

		std::string activeWindowtitle = GetActiveWindowTitle();

		std::string tournament = "Tournament";
		std::string table = "Table";

		std::vector<std::string> newTableTitles;

		for (size_t i = 0; i < numStages; ++i)
			newTableTitles.push_back("");

		winPos windowPos;

		if ((activeWindowtitle.find(tournament) == std::string::npos) || (activeWindowtitle.find(table) == std::string::npos))
			continue;

		// found a pokerstars table

		windowPos = GetWindowPos();

		// stage 1
		if (windowPos.x = -7 && windowPos.y == 0)
			newTableTitles[0] = GenerateTableTitle(activeWindowtitle);

		// stage 2
		if (windowPos.x = 635 && windowPos.y == 0)
			newTableTitles[1] = GenerateTableTitle(activeWindowtitle);

		// stage 3
		if (windowPos.x = 1275 && windowPos.y == 0)
			newTableTitles[2] = GenerateTableTitle(activeWindowtitle);

		// stage 1
		if (windowPos.x = -7 && windowPos.y == 480)
			newTableTitles[3] = GenerateTableTitle(activeWindowtitle);

		// stage 2
		if (windowPos.x = 635 && windowPos.y == 480)
			newTableTitles[4] = GenerateTableTitle(activeWindowtitle);

		// stage 3
		if (windowPos.x = 1275 && windowPos.y == 480)
			newTableTitles[5] = GenerateTableTitle(activeWindowtitle);

		for (size_t i = 0; i < currentTableTitles.size(); ++i)
		{
			if (!newTableTitles[i].empty() && (newTableTitles[i] != currentTableTitles[i]))
			{
				//re-write individual stagename file
				currentTablesTxtFileStream.open(currentTableTxtFiles[i]);
				currentTablesTxtFileStream << newTableTitles[i];
				currentTablesTxtFileStream.close();

				currentTableTitles[i] = newTableTitles[i];
				
				if (i < numMainStages)
				{
					// TODO: put this somewhere more sensible
					// re-write mainstage list
					currentTablesTxtFileStream.open("3_Main_Stages.txt");

					for (size_t j = 0; j < currentTableTitles.size(); ++j)
						currentTablesTxtFileStream << currentTableTitles[j] << std::endl;

					currentTablesTxtFileStream.close();
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

std::string GenerateTableTitle(std::string &title)
{
	std::string newTitle = "";

	// TODO: regex all possiblities
	const std::regex dollarRegex("([0-9]+).?([0-9]+)");

	//const std::regex dollarRegex(("([0-9])+"));
	std::smatch dollarMatch;

	std::regex_search(title, dollarMatch, dollarRegex);

	if (dollarMatch.size() == 0)
		return newTitle;

	std::string buyin = "$" + dollarMatch[0].str();

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

	if (title.find("Mini") != std::string::npos)
		prefix = " Mini";
	if (title.find("Micro") != std::string::npos)
		prefix = " Micro";
	if (title.find("High Roller Club:") != std::string::npos)
		prefix = " HRC:";

	// special names
	if (title.find("Bounty Builder") != std::string::npos)
		return "Bounty Builder " + buyin;
	if (title.find("Jumbo Shrimp") != std::string::npos)
		return buyin + " Jumbo Shrimp";
	if (title.find("Battle Royale") != std::string::npos)
		return buyin + prefix + " Battle Royale";
	if (title.find("Double Deuce") != std::string::npos)
		return buyin + " Double Deuce";
	if (title.find("Micro Madness") != std::string::npos)
		return buyin + " Micro Madness";
	if (title.find("Daily Pacific Rim") != std::string::npos)
		return buyin + prefix + " Daily Pacific Rim";

	bool tripleThreat = title.find("Triple Threat") != std::string::npos;
	bool dailyCooldown = title.find("Daily Cooldown") != std::string::npos;
	bool dailySupersonic = title.find("Daily Supersonic") != std::string::npos;

	// special prizes
	bool satelite = title.find(" Sat ") != std::string::npos;
	bool scoop = title.find("SCOOP") != std::string::npos;

	// Sundays
	bool sundayStorm = title.find("Sunday Storm") != std::string::npos;
	bool sundayMillion = title.find("Sunday Million") != std::string::npos;
	bool sundayStart = title.find("Sunday Starter") != std::string::npos;
	bool sundayKickoff = title.find("Sunday Kickoff") != std::string::npos;
	bool sundayMarathon = title.find("Sunday Marathon") != std::string::npos;
	bool sundayStack = title.find("Sunday Stack") != std::string::npos;
	bool sundayWrapUp = title.find("Sunday Wrap-Up") != std::string::npos;
	bool sundayPacificRim = title.find("Sunday Pacific Rim") != std::string::npos;

	// Mondays
	bool Monday6max = title.find("Monday 6-Max") != std::string::npos;

	// Tuesdays
	bool superTuesday = title.find("Super Tuesday") != std::string::npos;

	// Wednesdays

	// Thursdays
	bool fatThursday = title.find("Fat Thursday") != std::string::npos;
	bool thursdayThrill = title.find("Thursday Thrill") != std::string::npos;

	// Fridays
	bool fastFriday = title.find("Fast Friday") != std::string::npos;

	// Saturdays
	bool saturdayKO = title.find("Saturday KO") != std::string::npos;
	bool saturdaySlam = title.find("Saturday Slam") != std::string::npos;
	bool saturdayDuel = title.find("Saturday Duel") != std::string::npos;
	bool saturdayEliminator = title.find("Saturday Eliminator") != std::string::npos;
	bool saturdayRebuy = title.find("Saturday Rebuy") != std::string::npos;

	// table sizes
	bool HU = title.find("Heads-Up") != std::string::npos;
	bool sixMax = title.find("6-Max") != std::string::npos;
	bool eightmax = title.find("8-Max") != std::string::npos;

	// speeds
	bool hyperTurbo = title.find("Hyper-Turbo") != std::string::npos;
	bool turbo = title.find("Turbo") != std::string::npos;

	// formats
	bool zoom = title.find("Zoom") != std::string::npos;
	bool deepStacks = title.find("Deep Stacks") != std::string::npos;
	bool bubbleRush = title.find("Bubble Rush") != std::string::npos;
	bool winTheButton = title.find("Win The Button") != std::string::npos;
	bool rebuy = title.find("Rebuy") != std::string::npos;
	bool oneR1A = title.find("1R1A") != std::string::npos;
	bool spinAndGo = title.find("Spin & Go") != std::string::npos;
	//bool reentry = title.find("Re-entry") != std::string::npos;

	// KO's
	//bool progTotalKO = title.find("Progressive Total KO") != std::string::npos;
	bool progKO = title.find("Progressive KO") != std::string::npos;
	bool KO = title.find("KO") != std::string::npos;

	// no special names
	bool NLHE = title.find("NLHE") != std::string::npos;
	bool NLHoldem = title.find("NL Hold'em") != std::string::npos;

	// generate titles

	return newTitle;
}
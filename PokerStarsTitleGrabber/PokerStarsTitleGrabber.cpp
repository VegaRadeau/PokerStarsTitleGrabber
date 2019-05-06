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
std::string GenerateTableTitle(std::string title);
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
					currentTablesTxtFileStream.open("StageList.txt");

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

std::string GenerateTableTitle(std::string title)
{
	std::string newTitle = "";

	// TODO: regex all possiblities
	const std::regex dollarRegex("\\+([0-9]+)\\.([0-9]+)");

	//const std::regex dollarRegex(("([0-9])+"));
	std::smatch dollarMatch;

	std::regex_match(title, dollarMatch, dollarRegex);

	// table sizes
	bool HU = false;
	bool sixMax = false;
	bool eightmax = false;

	// forats
	bool zoom = false;
	bool deepStacks = false;
	bool bubbleRush = false;
	bool winTheButton = false;
	bool rebuy = false;
	bool reentry = false;

	// KO's
	bool bountyBuilder = false;
	bool progKO = false;
	bool KO = false;

	// names
	bool big = false;
	bool bigger = false;
	bool hot = false;
	bool hotter = false;
	
	// special names
	bool scoop = false;
	bool satelite = false;
	bool jumboShrimp = false;
	bool sundayStorm = false;

	// speeds
	bool hyperTurbo = false;
	bool turbo = false;
	bool regular = false;
	bool slow = false;


	//std::size_t bracketPos = title.find("[");
	//std::size_t commaPos = title.find(",");

	//newTitle = title.substr(0, bracketPos-1);

	//if (bracketPos > 256)
	//	newTitle = title.substr(0, commaPos);

	newTitle = "test";

	return newTitle;
}
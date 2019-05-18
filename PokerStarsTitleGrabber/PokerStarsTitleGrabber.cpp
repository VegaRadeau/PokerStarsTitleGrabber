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
bool AppendNewTitle(std::string &newTitle, std::string appendage);
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
		Sleep(1); // sleep 1ms

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
		if (windowPos.x == -7 && windowPos.y == 0)
			newTableTitles[0] = GenerateTableTitle(activeWindowtitle);

		// stage 2
		if (windowPos.x == 635 && windowPos.y == 0)
			newTableTitles[1] = GenerateTableTitle(activeWindowtitle);

		// stage 3
		if (windowPos.x == 1275 && windowPos.y == 0)
			newTableTitles[2] = GenerateTableTitle(activeWindowtitle);

		// stage 4
		if (windowPos.x == -7 && windowPos.y == 480)
			newTableTitles[3] = GenerateTableTitle(activeWindowtitle);

		// stage 5
		if (windowPos.x == 635 && windowPos.y == 480)
			newTableTitles[4] = GenerateTableTitle(activeWindowtitle);

		// stage 6
		if (windowPos.x == 1275 && windowPos.y == 480)
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
				
				std::cout << newTableTitles[i] << "\n";

				if (i < numMainStages)
				{
					// TODO: put this somewhere more sensible
					// re-write mainstage list
					currentTablesTxtFileStream.open("Main_Stages.txt");

					for (size_t j = 0; j < numMainStages; ++j)
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

	// TODO: handle freerolls, cashgames

	const std::regex buyinRegex("([$])([0-9]+).?([0-9]+)");
	std::smatch buyinMatch;
	std::regex_search(title, buyinMatch, buyinRegex);

	if (buyinMatch.size() == 0)
		return newTitle;

	std::string buyin = buyinMatch[0].str();

	// chop off ".00" if it's there
	if (buyin.substr(buyin.length() - 3) == ".00")
		buyin = buyin.substr(0, buyin.length() - 3);

	// special tournaments
	if (title.find("SCOOP") != std::string::npos) { buyin += " SCOOP"; }

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

	// Sundays
	if (title.find("Sunday Storm") != std::string::npos)
		return buyin + " Sunday Storm";
	if (title.find("Sunday Million") != std::string::npos)
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

	// rebuy is special because we append it to buyin
	if (title.find("Rebuy") != std::string::npos || title.find("+R") != std::string::npos) { if (!AppendNewTitle(newTitle, "+R")) { return newTitle; }; }

	// special prizes
	if (title.find(" Sat ") != std::string::npos || title.find(" Satellite ") != std::string::npos) { if (!AppendNewTitle(newTitle, " Sat")) { return newTitle; }; }

	// KO's
	if (title.find("Progressive Total KO") != std::string::npos) { if (!AppendNewTitle(newTitle, " PTKO")) { return newTitle; }; }
	if (title.find("Progressive KO") != std::string::npos) { if (!AppendNewTitle(newTitle, " PKO")) { return newTitle; }; }
	else if (title.find("Progressive KO") != std::string::npos) { if (!AppendNewTitle(newTitle, " KO")) { return newTitle; }; }

	// table sizes
	if (title.find("6-Max") != std::string::npos) { if (!AppendNewTitle(newTitle, " 6-Max")) { return newTitle; }; }
	if (title.find("8-Max") != std::string::npos) { if (!AppendNewTitle(newTitle, " 8-Max")) { return newTitle; }; }
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

bool AppendNewTitle(std::string &newTitle, std::string appendage)
{
	std::string testTile = newTitle + appendage;

	if (testTile.length() > 21)
		return false;

	newTitle = testTile;

	return true;
}
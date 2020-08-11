#include "stdafx.h"

#include <regex>

#include "Configuration.h"
#include "Utils.h"

#include "PokerStarsTitleParsers.h"


std::string GenerateTableTitle(std::string title)
    {
    std::string tournamentStr = "Tournament";
    std::string tableStr = "Table";

    if (title.find(tournamentStr) != std::string::npos)
        if (title.find(tableStr) != std::string::npos)
            return GenerateTournamentTitle(title);
        else
            return ""; // table closing

    // All cash games mostly follow the format:
    // "Unique table name - big blind / small blind etc - game type"
    // game type can sometime contain more hyphens but they aren't usually seperated by spaces
    // e.g. '5-Card Draw'  
    // so we do a simple check for 2 hyphens inbetween 2 spaces

    std::vector<std::string> splitTitle;
    splitTitle = strSplit(title, " - ");

    if (splitTitle.size() < 2)
        return "";

    std::string newTitle = GenerateCashTitle(title);
    return newTitle;
    }


std::string GenerateTournamentTitle(std::string title)
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
    if (title.find("WCOOP") != std::string::npos) { buyin += " WCOOP"; }
    if (title.find("SCOOP") != std::string::npos) { buyin += " SCOOP"; }
    if (title.find("UFC KO") != std::string::npos) { buyin += " UFC"; }
    if (title.find("MicroMillions") != std::string::npos) { buyin += " uMilly"; }
    if (title.find("Bounty Builder Series") != std::string::npos) { buyin += " BBS"; }
    if (title.find("BSOP ") != std::string::npos) { buyin += " BSOP"; }
    if (title.find("Stadium Series ") != std::string::npos) { buyin += " Stadium"; }

    // regular names
    if (title.find("Big ") != std::string::npos)
        return "Big " + buyin;
    if (title.find("Bigger ") != std::string::npos)
        return "Bigger " + buyin;
    if (title.find("Hot ") != std::string::npos)
        {
        if (title.find("Progressive KO") != std::string::npos)
            AppendNewTitle(buyin, " PKO");

        return "Hot " + buyin;
        }
    if (title.find("Hotter ") != std::string::npos)
        {
        if (title.find("Progressive KO") != std::string::npos)
            AppendNewTitle(buyin, " PKO");

        return "Hotter " + buyin;
        }

    // prefixes for special names
    std::string prefix = "";

    //if (title.find("Mini") != std::string::npos)
    //	prefix = " Mini";
    //if (title.find("Micro") != std::string::npos)
    //	prefix = " Micro";
    //if (title.find("High Roller Club:") != std::string::npos)
    //	prefix = " HRC:";

    if (title.find("Bounty Builder") != std::string::npos)
        return "Bounty Builder " + buyin;
    if (title.find("Jumbo Shrimp") != std::string::npos)
        return buyin + " Jumbo Shrimp";
    if (title.find("Battle Royale") != std::string::npos)
        return buyin + " Battle Royale";
    if (title.find("Double Deuce") != std::string::npos)
        return buyin + " Double Deuce";
    if (title.find("Micro Madness") != std::string::npos)
        return buyin + " Micro Madness";
    if (title.find("Daily Pacific Rim") != std::string::npos)
        return buyin + " Pacific Rim";
    if (title.find("Triple Threat") != std::string::npos)
        return buyin + " Triple Threat";

    // dailys
    if (title.find("Daily Cooldown") != std::string::npos)
        return buyin + " Cooldown";
    if (title.find("Daily Supersonic") != std::string::npos)
        return buyin + " Supersonic";
    if (title.find("Daily Kickoff") != std::string::npos)
        return buyin + " Kickoff";
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
    else if (title.find("Knockout") != std::string::npos) { if (!AppendNewTitle(newTitle, " KO")) { return newTitle; }; }

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

        std::string::const_iterator searchStart(playerStr.cbegin());

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


std::string GenerateCashTitle(std::string title)
    {
    std::vector<std::string> splitTitle;
    splitTitle = strSplit(title, " - ");

    std::string newTitle = "";
    std::string stakes = "";
    std::string gameType = "";

    // remove "Logged In as X" typical cases - order matters
    if (splitTitle.back().find("Logged") != std::string::npos) { splitTitle.pop_back(); }

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
    if (splitTitle.back().find("No Limit 6+ Hold'em") != std::string::npos)
        {
        gameType = "6+ NLHE";
        return gameType;

        const std::regex stakesRegex("button blind([$])([0-9]+).?([0-9]+)?ante([$])([0-9]+).?([0-9]+)?");
        std::smatch stakesMatch;
        std::regex_search(splitTitle[1], stakesMatch, stakesRegex);
        stakes = stakesMatch[0].str();

        //TODO: figure out a better way of removing trailing whitespace
        if (stakes.length())
            if (stakes.back() == ' ')
                stakes = stakes.substr(0, stakes.length() - 1);

        stakes += " ";

        // TODO: figure out a more elegant way to do this
        if (AppendNewTitle(newTitle, gameType))
            return newTitle;
        else
            return gameType;
        }

    if (splitTitle[5].find("Tempest"))
        {
        const std::regex stakesRegex("([$])([0-9]+) Cap");
        std::smatch stakesMatch;
        std::regex_search(splitTitle[3], stakesMatch, stakesRegex);
        stakes = stakesMatch[0].str();
        return stakes.substr(0, stakes.length() - 4) + " Tempest";
        }

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


int64_t GetTournamentID(std::string title)
    {
    const std::regex idRegex("Tournament ([0-9]+) Table");
    std::smatch idMatch;
    std::regex_search(title, idMatch, idRegex);

    if (idMatch.size() == 0)
        return 0;

    return stoull(idMatch[1]);
    //return atoi(idMatch[1].str().c_str());
    }


bool AppendNewTitle(std::string& newTitle, std::string appendage)
    {
    std::string testTile = newTitle + appendage;
    int maxTitleLen = GetMaxTitleLength();

    if (testTile.length() > static_cast<size_t>(maxTitleLen))
        return false;

    newTitle = testTile;

    return true;
    }

// TODO: this needs to be smarter, knocking off last word
bool PrefixNewTitle(std::string& newTitle, std::string prefix)
    {
    std::string testTile = prefix + newTitle;
    int maxTitleLen = GetMaxTitleLength();

    if (testTile.length() > static_cast<size_t>(maxTitleLen))
        return false;

    newTitle = testTile;

    return true;
    }
#pragma once

#include <string>
#include <vector>

std::string GenerateTableTitle(std::string title);
std::string GenerateTournamentTitle(std::string title);
int64_t GetTournamentID(std::string title);
std::string GenerateCashTitle(std::string title);
std::string GetCashStakes(std::string stakesSearch);
std::vector<std::string> strSplit(std::string stringToBeSplitted, std::string delimeter);
bool AppendNewTitle(std::string& newTitle, std::string appendage);
bool PrefixNewTitle(std::string& newTitle, std::string prefix);
#pragma once

#include <vector>

bool LoadInitilisationFile();
int GetStageListNum();
int GetNumMainStages();
bool IsReplayStageEnabled();
bool WantReplayStagePrefix();
int GetMaxTitleLength();
std::vector<std::pair<int, int>> GetMainStagePositions();
std::pair<int, int> GetReplayStagePosition();

#include "stdafx.h"

// #include "Shlwapi.h"
// #include <tesseract/baseapi.h>
// #include <leptonica/allheaders.h>

// TODO: make PayoutsOCR.h


// list of payouts for each main stage table
//std::vector<std::vector<std::string>> payouts;
// void ScanPayouts(std::vector<std::vector<std::string>> *payoutsList);


// void ScanPayouts(std::vector<std::vector<std::string>> *payoutsList)
// {

// 	int numOfConfigs = 1;
// 	char **configs = new char *[numOfConfigs];
// 	configs[0] = (char *) "bazaar";

// 	tesseract::TessBaseAPI *api = new tesseract::TessBaseAPI();
// 	if (api->Init(NULL, "eng", tesseract::OEM_DEFAULT, nullptr, 0, nullptr, nullptr, false))
// 	{
// 		fprintf(stderr, "Could not initialize tesseract.\n");
// 		return;
// 	}

// 	for (int stageInd = 0; stageInd < 3 /*numMainStages*/; ++stageInd)
// 	{
// 		std::string imageName = std::to_string(stageInd + 1) + ".tiff";
// 		std::filesystem::path screenPath = std::filesystem::current_path().append(imageName);

// 		if (!std::filesystem::exists(screenPath))
// 			continue;

// 		//TODO: replace with MD5 of the screenshot and cache that, do the check here
// 		if (payoutsList->at(stageInd).size())
// 			continue; // we've already processed this

// 		char *outText;
// 		// Open input image with leptonica library
// 		Pix *image = pixRead(screenPath.string().c_str());
// 		if (!image)
// 		{
// 			//TODO: error
// 			continue;
// 		}

// 		api->SetImage(image);

// 		// Get OCR result
// 		outText = api->GetUTF8Text();

// 		std::string payoutsTxt(outText);
// 		std::vector<std::string> splitPayouts = strSplit(payoutsTxt, "\n");

// 		delete[] outText;
// 		pixDestroy(&image);
// 	}

// 	// Destroy used object and release memory
// 	//delete configs;
// 	api->End();
// }
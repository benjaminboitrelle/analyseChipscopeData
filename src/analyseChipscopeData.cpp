//
//  analyseChipscopeData
//
//  Created by Benjamin BOITRELLE on 29/11/2017.
//

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <memory>
#include <algorithm>
#include <utility>

#include "TFile.h"

#include "readChipscope.hpp"
#include "plotHistogram.hpp"

#include "xmlReader/tinyxml2.h"

using namespace tinyxml2;

int main (int argc, char **argv){
  
  XMLDocument configFileXML;
  XMLError errorResult = configFileXML.LoadFile("/Users/ben/PostDoc/ChipscopeSOLEIL/analyseChipscopeData/test.xml");
  if (errorResult != XML_SUCCESS){
    std::cerr << "ERROR! Could not find XML file to read." << std::endl;
    return 1;
  }
  
  const std::string INPUT_PATH = configFileXML.FirstChildElement("FILE")->FirstChildElement("INPUT")->GetText();
  if (INPUT_PATH == nullptr){
    std::cerr << "Field not found." << std::endl;
    return 1;
  }
  
  const std::string FILENAME = configFileXML.FirstChildElement("FILE")->FirstChildElement("FILENAME")->GetText();
  if (FILENAME == nullptr){
    std::cerr << "Field not found." << std::endl;
    return 1;
  }
  
  const std::string EXTENSION = configFileXML.FirstChildElement("FILE")->FirstChildElement("EXTENSION")->GetText();
  if (EXTENSION == nullptr){
    std::cerr << "Field not found." << std::endl;
    return 1;
  }
  
  const std::string NB_FILES_XML = configFileXML.FirstChildElement("FILE")->FirstChildElement("NUMBER_OF_FILES")->GetText();
  if (NB_FILES_XML == nullptr){
    std::cerr << "Field not found." << std::endl;
    return 1;
  }
  
  const std::string OUTPUT_ROOT = configFileXML.FirstChildElement("FILE")->FirstChildElement("OUTPUT_ROOT")->GetText();
  if (OUTPUT_ROOT == nullptr){
    std::cerr << "Field not found." << std::endl;
    return 1;
  }
  
  const std::string OUTPUT_PNG = configFileXML.FirstChildElement("FILE")->FirstChildElement("OUTPUT_PNG")->GetText();
  if (OUTPUT_PNG == nullptr){
    std::cerr << "Field not found." << std::endl;
    return 1;
  }
  
  const std::string DATA_OFFSET_XML = configFileXML.FirstChildElement("ANALYSIS")->FirstChildElement("DATA_OFFSET")->GetText();
  if (DATA_OFFSET_XML == nullptr){
    std::cerr << "Field not found." << std::endl;
    return 1;
  }
  
  const std::string DATA_LENGTH_XML = configFileXML.FirstChildElement("ANALYSIS")->FirstChildElement("DATA_LENGTH")->GetText();
  if (DATA_LENGTH_XML == nullptr){
    std::cerr << "Field not found." << std::endl;
    return 1;
  }
  
  const int DATA_OFFSET = stoi(DATA_OFFSET_XML);
  const int DATA_LENGTH = stoi(DATA_LENGTH_XML);
  const int NB_FILES = stoi(NB_FILES_XML);
  int minNbFiles = 1;
  
  std::vector<std::vector<int>> imageCoarse, imageGain, imageFine;
  ReadChipscope chipscopeData;
  chipscopeData.SetDataStart(DATA_OFFSET);
  chipscopeData.SetDataLength(DATA_LENGTH);
  
  for (auto file = minNbFiles; file <= NB_FILES; file++){
    std::stringstream inputFile;
    inputFile << INPUT_PATH << FILENAME << file << EXTENSION;
    //    std::cout << "Processing file: " << inputFile.str() << std::endl;
    std::fstream myfile(inputFile.str());
    std::vector<std::vector<int>> rawData = chipscopeData.ReadPrnFile(myfile);
    for (auto groupNumber = 2; groupNumber <= 14; groupNumber += 2){
      std::vector<std::vector<int>> rowGroup = chipscopeData.GetChipscopeDisplay(groupNumber, rawData);
      std::vector<std::vector<int>> gainBits = chipscopeData.GetGainBits(rowGroup);
      std::vector<std::vector<int>> gainDecimals = chipscopeData.GetGainDecimals(gainBits);
      std::vector<std::vector<int>> fineBits = chipscopeData.GetFineBits(rowGroup);
      std::vector<std::vector<int>> fineDecimals = chipscopeData.GetFineDecimals(fineBits);
      std::vector<std::vector<int>> coarseBits = chipscopeData.GetCoarseBits(rowGroup);
      std::vector<std::vector<int>> coarseDecimals = chipscopeData.GetCoarseDecimals(coarseBits);
      std::vector<std::vector<int>> rowGroupVecGain = chipscopeData.PrepareVectorisedImage(gainDecimals);
      std::vector<std::vector<int>> rowGroupVecFine = chipscopeData.PrepareVectorisedImage(fineDecimals);
      std::vector<std::vector<int>> rowGroupVecCoarse = chipscopeData.PrepareVectorisedImage(coarseDecimals);
      imageGain.insert(imageGain.begin(), rowGroupVecGain.begin(), rowGroupVecGain.end());
      imageFine.insert(imageFine.begin(), rowGroupVecFine.begin(), rowGroupVecFine.end());
      imageCoarse.insert(imageCoarse.begin(), rowGroupVecCoarse.begin(), rowGroupVecCoarse.end());
    }
  }
  //  for(auto& row : imageCoarse){
  //    for(auto& col : row){
  //      std::cout << col << ";";
  //    }
  //    std::cout << std::endl;
  //  }
  //
  int spy = 0;
  for (auto row = 0; row < imageCoarse.size(); row++){
    //    std::cout << "Swaping position..." << std::endl;
    auto newColPosition = imageCoarse[row].size() - 1;
    while (newColPosition > 8){
//      std::cout << "Swaping col " << newColPosition << " with " << newColPosition - 8 << std::endl;
      std::swap(imageCoarse[row][newColPosition], imageCoarse[row][newColPosition - 8]);
      std::cout << "Col position: " << newColPosition << std::endl;
      spy++;
      if (3 == 0){
        newColPosition  += 23;
        std::cout << "If Col position: " << newColPosition << std::endl;
      }
      else{
        newColPosition -= 8;
        std::cout << "Col position -- : " << newColPosition << std::endl;
      }
    }
  }
  int nbOfLines = imageCoarse.size();
  int nbOfColumns = imageCoarse[0].size();
  //  std::cout << "Size of image -> number of columns: " << imageCoarse[0].size() << ", number of lines: " << imageCoarse.size() << std::endl;
  
  auto outputRootFile = TFile::Open(OUTPUT_ROOT.c_str(), "RECREATE");
  PlotHistogram gainPlot, coarsePlot, finePlot;
  gainPlot.CreateScatterPlot("Gain response", "Gain response", "/Users/ben/PostDoc/ChipscopeSOLEIL/analyseChipscopeData/output/gain.png", nbOfLines, 0, nbOfLines, nbOfColumns, 0, nbOfColumns, imageGain);
  coarsePlot.CreateScatterPlot("Coarse response", "Coarse response", "/Users/ben/PostDoc/ChipscopeSOLEIL/analyseChipscopeData/output/coarse.png", nbOfLines, 0, nbOfLines, nbOfColumns, 0, nbOfColumns, imageCoarse);
  finePlot.CreateScatterPlot("Fine response", "Fine response", "/Users/ben/PostDoc/ChipscopeSOLEIL/analyseChipscopeData/output/fine.png", nbOfLines, 0, nbOfLines, nbOfColumns, 0, nbOfColumns, imageFine);
  
  outputRootFile->Close();
  
  return 0;
}

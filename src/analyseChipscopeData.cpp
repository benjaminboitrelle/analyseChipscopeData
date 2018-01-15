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
  
  std::vector<std::vector<int>> image;
  ReadChipscope chipscopeData;
  chipscopeData.SetDataStart(DATA_OFFSET);
  chipscopeData.SetDataLength(DATA_LENGTH);
  
  for (auto file = minNbFiles; file <= NB_FILES; file++){
    std::stringstream inputFile;
    inputFile << INPUT_PATH << FILENAME << file << EXTENSION;
    std::cout << "Processing file: " << inputFile.str() << std::endl;
    std::fstream myfile(inputFile.str());
    std::vector<std::vector<int>> rawData = chipscopeData.ReadPrnFile(myfile);
    for (auto i = 2; i <= 14; i+=2){
      std::vector<std::vector<int>> rowGroup = chipscopeData.GetChipscopeDisplay(i, rawData);
      std::vector<std::vector<int>> coarseBits = chipscopeData.GetCoarseBits(rowGroup);
      std::vector<std::vector<int>> coarseDecimals = chipscopeData.GetCoarseDecimals(coarseBits);
      std::vector<std::vector<int>> rowGroupVec = chipscopeData.PrepareVectorisedImage(coarseDecimals);
      image.insert(image.begin(), rowGroupVec.begin(), rowGroupVec.end());
    }
  }
  
  int nbOfLines = image.size();
  int nbOfColumns = image[0].size();
  std::cout << "Size of image -> number of columns: " << image[0].size() << ", number of lines: " << image.size() << std::endl;
  
  auto outputRootFile = TFile::Open(OUTPUT_ROOT.c_str(), "RECREATE");
  PlotHistogram coarsePlot;
  coarsePlot.CreateScatterPlot("Coarse response", "Coarse response", OUTPUT_PNG, nbOfLines, 0, nbOfLines, nbOfColumns, 0, nbOfColumns, image);
  outputRootFile->Close();
  
  return 0;
}

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
  XMLError errorResult = configFileXML.LoadFile("test.xml");
  if (errorResult != XML_SUCCESS){
    std::cerr << "ERROR! Could not find XML file to read." << std::endl;
    return 1;
  }
  
  const std::string INPUT_FILE = configFileXML.FirstChildElement("FILE")->FirstChildElement("INPUT")->GetText();
  if (INPUT_FILE == nullptr){
    std::cerr << "Field not found." << std::endl;
    return 1;
  }

  const std::string OUTPUT_ROOT = configFileXML.FirstChildElement("FILE")->FirstChildElement("OUTPUT_ROOT")->GetText();
  if (OUTPUT_ROOT == nullptr){
    std::cerr << "Field not found." << std::endl;
    return 1;
  }

//  const std::string OUTPUT_PNG = configFileXML.FirstChildElement("FILE")->FirstChildElement("OUTPUT_PNG")->GetText();
//  if (OUTPUT_PNG == nullptr){
//    std::cerr << "Field not found." << std::endl;
//    return 1;
//  }
//
//  const std::string OUTPUT_ROOT = configFileXML.FirstChildElement("FILE")->FirstChildElement("OUTPUT_ROOT")->GetText();
//  if (OUTPUT_ROOT == nullptr){
//    std::cerr << "Field not found." << std::endl;
//    return 1;
//  }
  std::fstream myfile(INPUT_FILE);
  //std::vector<std::vector<int>> rawData;

  ReadChipscope chipscopeData;
 // chipscopeData.readPrnFile(myfile, rawData);
  std::vector<std::vector<int>> rawData = chipscopeData.readPrnFile(myfile);

  //std::cout << "Size of raw Data: number of columns: " << rawData[0].size() << ", number of lines: " << rawData.size() << std::endl;

  //std::vector<std::vector<int>> tmp(&rawData[0], &rawData[3360]);
  std::vector<std::vector<int>> tmp = chipscopeData.getChipscopeDisplay(2, rawData);
  std::vector<std::vector<int>> gainBits = chipscopeData.getGainBits(tmp);
  std::vector<std::vector<int>> fineBits = chipscopeData.getFineBits(tmp);
  std::vector<std::vector<int>> coarseBits = chipscopeData.getCoarseBits(tmp);
  std::vector<std::vector<int>> gainDecimals = chipscopeData.getGainDecimals(gainBits);
  std::vector<std::vector<int>> fineDecimals = chipscopeData.getFineDecimals(fineBits);
  std::vector<std::vector<int>> coarseDecimals = chipscopeData.getCoarseDecimals(coarseBits);
  std::cout << "Size of gainBits: " << gainBits.size() << ", size of fineBits: " << fineBits.size() << " and size of coarseBits: " << coarseBits.size() << std::endl;
  std::cout << "Size of gainDecimals: " << gainDecimals.size() << std::endl;
  std::cout << "Size of fineDecimals: " << fineDecimals.size() << std::endl;
  std::cout << "Size of coarseDecimals: " << coarseDecimals.size() << std::endl;

  //for (auto row : coarseBits){
  //  for (auto column : row){
  //  std::cout << column << " ";
  //  }
  //  std::cout << std::endl;  
  //}

  std::vector<std::vector<int>> tmpVec = chipscopeData.prepareVectorisedImage(coarseDecimals);
  std::cout << "Size of image: " << tmpVec.size() << "; " << tmpVec[0].size() << std::endl;

 // for (unsigned int row = 0; row < tmpVec.size(); row++){
 //   for (unsigned int column = 0; column < tmpVec[row].size(); column++){
 //    std::cout << tmpVec[row][column] << " ";  
 //   }
 //   std::cout << std::endl;  
 // }

  //auto outputRootFile = TFile::Open("/home/ben/analyseChipscopeData/output/test.root", "RECREATE");
  auto outputRootFile = TFile::Open(OUTPUT_ROOT.c_str(), "RECREATE");
  PlotHistogram test;
  test.CreateScatterPlot("Test", "Test", 7, 0, 7, 1408, 0, 1408, tmpVec);
  outputRootFile->Close();



  return 0;
}

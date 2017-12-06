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

int main (int argc, char **argv){

// This part will be included in an external XML file
  const std::string INPUT_FILE = "/home/ben/Documents/001/0,003,004_P2Mchp.prn";
  std::fstream myfile(INPUT_FILE);
  char delimiterChar = '\t';
  std::vector<int> dataRange ={3, 47};
  const int dataLength = 3360;
  const int dataOffset = 9962;
  const int sampleToRead = 2;
  const int numberOfGainBits = 2;
  const int numberOfFineBits = 8;
  const int numberOfCoarseBits = 5;
  const int rowGroupDataLength = 224;
  const int gainBegin = 0;
  const int gainEnd = rowGroupDataLength * numberOfGainBits; 
  const int fineBegin = gainEnd ;
  const int fineEnd = fineBegin + (rowGroupDataLength * numberOfFineBits);
  const int coarseBegin = fineEnd ;
  const int coarseEnd = coarseBegin + (rowGroupDataLength * numberOfCoarseBits);

// Beginning of the analysis
  std::vector<std::vector<int>> rawData; 

  ReadChipscope chipscopeData;
  chipscopeData.readPrnFile(myfile, delimiterChar, dataRange.at(0), dataRange.at(1), rawData);
  std::vector<std::vector<int>> rowGroup = chipscopeData.getChipscopeDisplay(sampleToRead, dataOffset, dataLength, rawData);
  std::vector<std::vector<int>> gainDecimals = chipscopeData.getADCInfo(rowGroup, gainBegin, gainEnd, numberOfGainBits, rowGroupDataLength);
  std::vector<std::vector<int>> fineDecimals = chipscopeData.getADCInfo(rowGroup, fineBegin, fineEnd, numberOfFineBits, rowGroupDataLength);
  std::vector<std::vector<int>> coarseDecimals = chipscopeData.getADCInfo(rowGroup, coarseBegin, coarseEnd, numberOfCoarseBits, rowGroupDataLength);

  std::vector<std::vector<int>> gainBits = chipscopeData.getGainBits(rowGroup);
  std::vector<std::vector<int>> fineBits = chipscopeData.getFineBits(rowGroup);
  std::vector<std::vector<int>> coarseBits = chipscopeData.getCoarseBits(rowGroup);
  std::vector<std::vector<int>> gainDec = chipscopeData.getGainDecimals(gainBits);
  std::vector<std::vector<int>> fineDec = chipscopeData.getFineDecimals(fineBits);
  std::vector<std::vector<int>> coarseDec = chipscopeData.getCoarseDecimals(coarseBits);

  std::cout << "Size of gainDecimals: " << gainDecimals.size() << std::endl;
  std::cout << "Size of gainDec: " << gainDec.size() << std::endl;
  std::cout << "Size of fineDecimals: " << fineDecimals.size() << std::endl;
  std::cout << "Size of fineDec: " << fineDec.size() << std::endl;
  std::cout << "Size of coarseDecimals: " << coarseDecimals.size() << std::endl;
  std::cout << "Size of coarseDec: " << coarseDec.size() << std::endl;
 
  std::vector<std::vector<int>> tmpVec = chipscopeData.prepareVectorisedImage(coarseDecimals);
  std::vector<std::vector<int>> tmpVec2 = chipscopeData.prepareVectorisedImage(coarseDec);
  std::cout << "Size of image: " << tmpVec.size() << "; " << tmpVec[0].size() << std::endl;

  auto outputRootFile = TFile::Open("test.root", "RECREATE");
  PlotHistogram test;
  test.CreateScatterPlot("Test", "Test", 7, 0, 7, 1408, 0, 1408, tmpVec);
  test.CreateScatterPlot("Test2", "Test2", 7, 0, 7, 1408, 0, 1408, tmpVec2);
  outputRootFile->Close(); 
  return 0;
}

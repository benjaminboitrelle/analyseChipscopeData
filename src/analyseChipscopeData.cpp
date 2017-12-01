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

#include "readChipscope.hpp"

int main (int argc, char **argv){
  //const std::string INPUT_FILE  = "/home/ben/Documents/P2M_lightResponse/OD0.0.prn";
  const std::string INPUT_FILE  = "/home/ben/Documents/P2M_lightResponse/referencePSVolt.prn";
  const int dataLength  = 3360;

  std::fstream myfile(INPUT_FILE);
  std::vector<std::vector<int>> rawData;

  ReadChipscope chipscopeData;
  chipscopeData.readPrnFile(myfile, rawData);

  std::cout << "Size of raw Data: number of columns: " << rawData[0].size() << ", number of lines: " << rawData.size() << std::endl;
  
  int numberOfChipscopeGroups = int(rawData.size()) / dataLength;

  std::cout << "Number of Chipscope groups: " << numberOfChipscopeGroups << std::endl;

  //std::vector<std::vector<int>> tmp(&rawData[0], &rawData[3360]);
  std::vector<std::vector<int>> tmp = chipscopeData.getChipscopeDisplay(0, rawData);

  std::cout << "Size of tmp vector: number of columns: " << tmp[0].size() << ", number of lines: " << tmp.size() << std::endl; 
  for (unsigned int i = 0; i < tmp.size(); i++){
    for (unsigned int  j = 0; j < tmp[i].size(); j++){
      std::cout << tmp[i][j] << " ";
    }
    std::cout << std::endl;  
  }

  //std::vector<std::vector<int>> gainBits = chipscopeData.getGainBits(tmp);
  //std::vector<std::vector<int>> fineBits = chipscopeData.getFineBits(tmp);
  //std::vector<std::vector<int>> coarseBits = chipscopeData.getCoarseBits(tmp);
  //std::vector<std::vector<int>> gainDecimals = chipscopeData.getGainDecimals(gainBits);
  //std::vector<std::vector<int>> fineDecimals = chipscopeData.getFineDecimals(fineBits);
  //std::vector<std::vector<int>> coarseDecimals = chipscopeData.getCoarseDecimals(coarseBits);
  //std::cout << "Size of gainBits: " << gainBits.size() << ", size of fineBits: " << fineBits.size() << " and size of coarseBits: " << coarseBits.size() << std::endl;
  //std::cout << "Size of gainDecimals: " << gainDecimals.size() << std::endl;
  //std::cout << "Size of fineDecimals: " << fineDecimals.size() << std::endl;
  //std::cout << "Size of coarseDecimals: " << coarseDecimals.size() << std::endl;

  //for (auto row : coarseDecimals){
  //  for (auto col : row){
  //    std::cout << col << std::endl;  
  //  }  
  //}
  return 0;
}

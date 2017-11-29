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
  const std::string INPUT_FILE  = "/home/ben/Documents/P2M_lightResponse/OD0.0.prn";

  std::fstream myfile(INPUT_FILE);
  std::vector<std::vector<int>> rawData;

  ReadChipscope chipscopeData;
  chipscopeData.readPrnFile(myfile, rawData);

  std::cout << "Size of raw Data: " << rawData[0].size() << " and " << rawData.size() << std::endl;

  std::cout << "hello world" << std::endl;
  
  return 0;
}

//
//  readChipscope.hpp
//  analyseChipscopeData
//
//  Created by Benjamin BOITRELLE on 09/11/2017.
//

#include "readChipscope.hpp"

#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <string>
#include <memory>
#include <algorithm>
#include <map>
#include <utility>
#include <math.h>

ReadChipscope::ReadChipscope(){
  // CONSTRUCTOR
  m_dataLength = 3360; // Number of bit sent by a rowgroup: 32 columns times 7 rows times 15 bits ADC
  m_dataStart = 9962; // Default value given by Chipscope according to referencePSVolt.prn
  m_chipscopeLength = 0;
  m_numberOfGainBits = 2; // Default value according to P2M architecture
  m_numberOfFineBits = 8; // Default value according to P2M architecture
  m_numberOfCoarseBits = 5; // Default value according to P2M architecture
  m_rowGroupLength = 224;
  m_gainBegin = 0;
  m_gainEnd = m_rowGroupLength * m_numberOfGainBits;
  m_fineBegin = m_gainEnd ;
  m_fineEnd = m_fineBegin + (m_rowGroupLength * m_numberOfFineBits);
  m_coarseBegin = m_fineEnd ;
  m_coarseEnd = m_coarseBegin + (m_rowGroupLength * m_numberOfCoarseBits);
}

ReadChipscope::~ReadChipscope(){
  // DESTRUCTOR
}

std::vector<std::vector<int>> ReadChipscope::getChipscopeDisplay(int dataToRead, std::vector<std::vector<int>> input){
  // The different data (SAMPLE or RESET) are 3360 bits long but there is an offset in chipscope 

  int begin = m_dataStart + dataToRead * (m_dataLength); 
  int end  = begin + m_dataLength;
  std::vector<std::vector<int>> chipscopeDisplay(input.begin() + begin, input.begin() + end);
  return chipscopeDisplay;
}

std::vector<std::vector<int>> ReadChipscope::getGainBits(std::vector<std::vector<int>> input){
  // From a block (Sample or Reset) get the 448 first elements corresponding to the gain

  std::vector<std::vector<int>> gainBits(input.begin() + m_gainBegin, input.begin() + m_gainEnd);   
  return gainBits;  
}

std::vector<std::vector<int>> ReadChipscope::getGainDecimals(std::vector<std::vector<int>> input){

  std::vector<std::vector<int>> gainDecimals;
  for (unsigned int bit = 0; bit < (input.size() / m_numberOfGainBits); bit++){
    std::vector<int> tmpGainDecimals;
    for (unsigned int column = 0; column < input[0].size(); column++){
      std::vector<int> pixelGainBits = {input[bit][column], input[bit + m_rowGroupLength][column]};
      int pixelGainDecimal = convertBitsToDecimals(pixelGainBits);
      tmpGainDecimals.push_back(pixelGainDecimal);
    }  
    gainDecimals.push_back(tmpGainDecimals);
  }
  return gainDecimals;
}

std::vector<std::vector<int>> ReadChipscope::getFineBits(std::vector<std::vector<int>> input){

  std::vector<std::vector<int>> fineBits(input.begin() + m_fineBegin, input.begin() + m_fineEnd);
  return fineBits; 
}

std::vector<std::vector<int>> ReadChipscope::getFineDecimals(std::vector<std::vector<int>> input){

  std::vector<std::vector<int>> fineDecimals;
  for (unsigned int bit = 0; bit < (input.size() / m_numberOfFineBits); bit++){
    std::vector<int> tmpFineDecimals;
    for (unsigned int column = 0; column < input[0].size(); column++){
      std::vector<int> pixelFineBits;
      for (auto numberOfFineBits = 0; numberOfFineBits < m_numberOfFineBits; numberOfFineBits++){
        pixelFineBits.push_back(input[bit + (numberOfFineBits * m_rowGroupLength)][column]);
      }
      //std::vector<int> pixelFineBits(input[bit][column], input[bit + m_rowGroupLength][column]);
      int pixelFineDecimal = convertBitsToDecimals(pixelFineBits);
      tmpFineDecimals.push_back(pixelFineDecimal);
    }  
    fineDecimals.push_back(tmpFineDecimals);
  }
  return fineDecimals;
}

std::vector<std::vector<int>> ReadChipscope::getCoarseBits(std::vector<std::vector<int>> input){

  std::vector<std::vector<int>> coarseBits(input.begin() + m_coarseBegin, input.begin() + m_coarseEnd);
  return coarseBits;
}

std::vector<std::vector<int>> ReadChipscope::getCoarseDecimals(std::vector<std::vector<int>> input){

  std::vector<std::vector<int>> coarseDecimals;
  for (unsigned int bit = 0; bit < (input.size() / m_numberOfCoarseBits); bit++){
    std::vector<int> tmpCoarseDecimals;
    for (unsigned int column = 0; column < input[0].size(); column++){
      std::vector<int> pixelCoarseBits;
      for (auto numberOfCoarseBits = 0; numberOfCoarseBits < m_numberOfCoarseBits; numberOfCoarseBits++){
        pixelCoarseBits.push_back(input[bit + (numberOfCoarseBits * m_rowGroupLength)][column]);
      }
      //std::vector<int> pixelFineBits(input[bit][column], input[bit + m_rowGroupLength][column]);
      int pixelCoarseDecimal = convertBitsToDecimals(pixelCoarseBits);
      tmpCoarseDecimals.push_back(pixelCoarseDecimal);
    }  
    coarseDecimals.push_back(tmpCoarseDecimals);
  }
  return coarseDecimals;
}

int ReadChipscope::convertBitsToDecimals(std::vector<int> inputBits){

  int decimal = 0;
  for (unsigned int bit = 0; bit < inputBits.size(); bit++){
    decimal += inputBits.at(bit)*pow(2,inputBits.size() - ( 1 + bit)); 
  }
  return decimal;
}

std::vector<int> ReadChipscope::convertStringToInt(std::vector<std::string>& inputString, std::vector<int>& output){
  // Convert a vector of string into a vector of double

  output.resize(inputString.size());
  std::transform(inputString.begin(), inputString.end(), output.begin(), [](const std::string& val)
      {
      return std::stoi(val);
      });
  return output;
}

void ReadChipscope::readPrnFile(std::fstream& myFile, std::vector<std::vector<int>> &outputImageVectorised){
  // Open a .prn file produced by Chipscope and store its element in a 2D vector of int

  std::string outputLine;
  myFile.ignore ( std::numeric_limits<std::streamsize>::max(), '\n' );
  while (std::getline(myFile, outputLine)){
    std::vector<std::string> result;
    splitString(outputLine, '\t', result);
    std::vector<std::string> tmpResult(&result[3], &result[47]); // H0_0 starts at the 3rd position and H1_21 is at the 47 position
    std::vector<int> resultIntFormat(tmpResult.size());
    convertStringToInt(tmpResult, resultIntFormat);
    outputImageVectorised.push_back(resultIntFormat);
  }
  m_chipscopeLength = (int)outputImageVectorised.size();
}

void ReadChipscope::splitString(const std::string& inputString, char delimiterChar, std::vector<std::string> &outputVector){
  // Split a string according to a define delimiter and save it into a vector

  std::string::size_type i = 0;
  std::string::size_type j = inputString.find(delimiterChar);

  while (j != std::string::npos) {
    outputVector.push_back(inputString.substr(i, j-i));
    i = ++j;
    j = inputString.find(delimiterChar, j);
    if (j == std::string::npos) outputVector.push_back(inputString.substr(i, inputString.length()));
  }

}

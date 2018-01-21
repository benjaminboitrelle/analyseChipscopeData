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
#include <iterator>
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
  m_fineBegin = m_gainEnd;
  m_fineEnd = m_fineBegin + (m_rowGroupLength * m_numberOfFineBits);
  m_coarseBegin = m_fineEnd;
  m_coarseEnd = m_coarseBegin + (m_rowGroupLength * m_numberOfCoarseBits);
}

ReadChipscope::~ReadChipscope(){
  // DESTRUCTOR
}

std::vector<std::vector<int>> ReadChipscope::GetChipscopeDisplay(int dataToRead, std::vector<std::vector<int>> input){
  // The different data (SAMPLE or RESET) are 3360 bits long but there is an offset in Chipscope
  
  int begin = m_dataStart + dataToRead * (m_dataLength);
  int end  = begin + m_dataLength;
  std::vector<std::vector<int>> chipscopeDisplay(input.begin() + begin, input.begin() + end);
  //  std::cout << "Size of vector (data): " << chipscopeDisplay.size() << std::endl;
  return chipscopeDisplay;
}

std::vector<std::vector<int>> ReadChipscope::GetGainBits(std::vector<std::vector<int>> input){
  // From a block (Sample or Reset) get the 448 first elements corresponding to the gain
  
  int gainBegin = m_gainBegin;
  int gainEnd = m_gainEnd;
  std::vector<std::vector<int>> gainBits(input.begin() + gainBegin, input.begin() + gainEnd);
  //  std::cout << "Size of Gain (bits): " << gainBits.size() << std::endl;
  return gainBits;
}

std::vector<std::vector<int>> ReadChipscope::GetGainDecimals(std::vector<std::vector<int>> input){
  
  std::vector<std::vector<int>> gainDecimals;
  for (unsigned int bit = 0; bit < (input.size() / m_numberOfGainBits); bit++){
    std::vector<int> tmpGainDecimals;
    for (unsigned int column = 0; column < input[0].size(); column++){
      std::vector<int> pixelGainBits = {input[bit][column], input[bit + m_rowGroupLength][column]};
      int pixelGainDecimal = ConvertBitsToDecimals(pixelGainBits);
      tmpGainDecimals.push_back(pixelGainDecimal);
    }
    gainDecimals.push_back(tmpGainDecimals);
  }
  //  std::cout << "Size of Gain (decimals): " << gainDecimals.size() << std::endl;
  return gainDecimals;
}

std::vector<std::vector<int>> ReadChipscope::GetFineBits(std::vector<std::vector<int>> input){
  
  std::vector<std::vector<int>> fineBits(input.begin() + m_fineBegin, input.begin() + m_fineEnd);
  //  std::cout << "Size of Fine (bits): " << fineBits.size() << std::endl;
  return fineBits;
}

std::vector<std::vector<int>> ReadChipscope::GetFineDecimals(std::vector<std::vector<int>> input){
  
  std::vector<std::vector<int>> fineDecimals;
  for (unsigned int bit = 0; bit < (input.size() / m_numberOfFineBits); bit++){
    std::vector<int> tmpFineDecimals;
    for (unsigned int column = 0; column < input[0].size(); column++){
      std::vector<int> pixelFineBits;
      for (auto numberOfFineBits = 0; numberOfFineBits < m_numberOfFineBits; numberOfFineBits++){
        pixelFineBits.push_back(input[bit + (numberOfFineBits * m_rowGroupLength)][column]);
      }
      //std::vector<int> pixelFineBits(input[bit][column], input[bit + m_rowGroupLength][column]);
      int pixelFineDecimal = ConvertBitsToDecimals(pixelFineBits);
      tmpFineDecimals.push_back(pixelFineDecimal);
    }
    fineDecimals.push_back(tmpFineDecimals);
  }
  //  std::cout << "Size of Fine (decimals): " << fineDecimals.size() << std::endl;
  return fineDecimals;
}

std::vector<std::vector<int>> ReadChipscope::GetCoarseBits(std::vector<std::vector<int>> input){
  
  //  std::cout << "Size of input for coarse (bits) " << input.size() << " " << input[0].size() << std::endl;
  std::vector<std::vector<int>> coarseBits(input.begin() + m_coarseBegin, input.begin() + m_coarseEnd);
  //  std::cout << "Size of Coarse (bits): " << coarseBits.size() << " " << coarseBits[0].size() << std::endl;
  return coarseBits;
}

std::vector<std::vector<int>> ReadChipscope::GetCoarseDecimals(std::vector<std::vector<int>> input){
  
  std::vector<std::vector<int>> coarseDecimals;
  for (unsigned int bit = 0; bit < (input.size() / m_numberOfCoarseBits); bit++){
    std::vector<int> tmpCoarseDecimals;
    for (unsigned int column = 0; column < input[0].size(); column++){
      std::vector<int> pixelCoarseBits;
      for (auto numberOfCoarseBits = 0; numberOfCoarseBits < m_numberOfCoarseBits; numberOfCoarseBits++){
        pixelCoarseBits.push_back(input[bit + (numberOfCoarseBits * m_rowGroupLength)][column]);
      }
      //std::vector<int> pixelFineBits(input[bit][column], input[bit + m_rowGroupLength][column]);
      int pixelCoarseDecimal = ConvertBitsToDecimals(pixelCoarseBits);
      tmpCoarseDecimals.push_back(pixelCoarseDecimal);
    }
    coarseDecimals.push_back(tmpCoarseDecimals);
  }
  //  std::cout << "Size of Coarse (decimals): " << coarseDecimals[0].size() << std::endl;
  return coarseDecimals;
}

int ReadChipscope::ConvertBitsToDecimals(std::vector<int> inputBits){
  
  int decimal = 0;
  for (unsigned int bit = 0; bit < inputBits.size(); bit++){
    decimal += inputBits.at(bit)*pow(2,inputBits.size() - ( 1 + bit));
  }
  return decimal;
}

std::vector<int> ReadChipscope::ConvertStringToInt(std::vector<std::string>& inputString, std::vector<int>& output){
  // Convert a vector of string into a vector of double
  
  output.resize(inputString.size());
  std::transform(inputString.begin(), inputString.end(), output.begin(), [](const std::string& val)
                 {
                   return std::stoi(val);
                 });
  return output;
}
std::vector<std::vector<int>> ReadChipscope::ReadPrnFile(std::fstream& myFile){
  // Open a .prn file produced by Chipscope and store its element in a 2D vector of int
  
  std::string outputLine;
  myFile.ignore ( std::numeric_limits<std::streamsize>::max(), '\n' );
  std::vector<std::vector<int>> outputImageVectorised;
  while (std::getline(myFile, outputLine)){
    std::vector<std::string> result;
    SplitString(outputLine, '\t', result);
    std::vector<std::string> tmpResult(&result[25], &result[47]);
    tmpResult.insert(tmpResult.end(), &result[3], &result[25]);
    std::vector<int> resultIntFormat(tmpResult.size());
    ConvertStringToInt(tmpResult, resultIntFormat);
    for (auto &bit : resultIntFormat) (bit == 0)? bit = 1 : bit = 0;
    outputImageVectorised.push_back(resultIntFormat);
  }
  m_chipscopeLength = (int)outputImageVectorised.size();
  //  std::cout << "Size of data read: " << outputImageVectorised.size() << "x" << outputImageVectorised[0].size() << std::endl;
  return outputImageVectorised;
}

void ReadChipscope::SplitString(const std::string& inputString, char delimiterChar, std::vector<std::string> &outputVector){
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

std::vector<std::vector<int>> ReadChipscope::PrepareVectorisedImage(std::vector<std::vector<int>> input){
  // Transform the vector of data into a 2D matrix to get the response of a row group
  
  std::vector<std::vector<int>> outputImage;
  //  std::vector<int> outputTest;
  int nbOfLines = 7;
  int nbOfColumns = 1408; // 32 columns * 44 data lines
  outputImage.resize(nbOfLines);
  for (int i = 0; i < nbOfLines; ++i){
    outputImage[i].resize(nbOfColumns);
  }
  int nbOfColumnsPerDataLine  = 31;
  //  int tmpCol = nbOfColumnsPerDataLine + 1;
  //  int columnDecrement = 0;
  
  for (unsigned int column = 0; column < input[0].size(); column++){
//    int i = 0;
    int tmpRow = -1;
    int columnPositionInDataLine = nbOfColumnsPerDataLine;
//    std::cout << "Re-initialisation of column positionning " << columnPositionInDataLine << std::endl;
    int tmpCol =  column + columnPositionInDataLine * column;
    //      tmpCol =  nbOfColumnsPerDataLine + (nbOfColumnsPerDataLine  * column) - i;
    //    tmpCol = nbOfColumnsPerDataLine + columnPositionInDataLine * column;
    //    std::cout << "tmpCol in 1st loop: " << tmpCol <<  std::endl;
    
    for (unsigned int row = 0; row < input.size(); row++){
//      std::cout << "Reorganisation of row and column..." << std::endl;
      if (row % 32 == 0){
        tmpRow++;
        
        //                tmpCol = column + nbOfColumnsPerDataLine * column;
        //        tmpCol = nbOfColumnsPerDataLine + columnPositionInDataLine * column;

//        tmpCol = nbOfColumnsPerDataLine + nbOfColumnsPerDataLine * column;
//        if (columnPositionInDataLine < 0){
//          i++;
//          if (i > 7){
//            i = 0;
//          }
//          //          std::cout << "i: " << i << std::endl;
//          columnPositionInDataLine = nbOfColumnsPerDataLine - i;
          //          std::cout << "If pointed column is negative, reinitilisation: " << columnPositionInDataLine << std::endl;
//        }
        tmpCol = nbOfColumnsPerDataLine + columnPositionInDataLine * column;

        outputImage[tmpRow][tmpCol] = input[row][column];
        //        std::cout << "tmpCol in 2st loop: " << tmpCol <<  std::endl;
        
      }
      else{
        outputImage[tmpRow][tmpCol] = input[row][column];
//        columnPositionInDataLine -= 8;
//        std::cout << "Deincrementation of pointed column: " << columnPositionInDataLine << std::endl;
//        tmpCol = nbOfColumnsPerDataLine + columnPositionInDataLine * column;
//        if (columnPositionInDataLine < 0){
//          i++;
//          if (i > 7){
//            i = 0;
//          }
////          std::cout << "i: " << i << std::endl;
//          columnPositionInDataLine = nbOfColumnsPerDataLine - i;
//          std::cout << "If pointed column is negative, reinitilisation: " << columnPositionInDataLine << std::endl;
//        }
//        tmpCol = nbOfColumnsPerDataLine + columnPositionInDataLine * column;
//        tmpCol = columnPositionInDataLine;

        //        std::cout << " Temporary column ordering " << tmpCol << std::endl;
//                tmpCol++;
        //        columnPositionInDataLine -= 4;
        //        tmpCol = nbOfColumnsPerDataLine + columnPositionInDataLine * column;
        
        tmpCol--;
      }
    }
  }
  
  //  std::cout << "NUMBER OF COLUMNS CREATED: " << colCounter << std::endl;
  return outputImage;
}

void ReadChipscope::InverseBits(std::vector<int> input){
  // Translate the english bit convention to the european one
  
  for (auto &bit : input){
    bit == 0 ? bit = 1 : bit = 0;
  }
}

void ReadChipscope::SetDataLength(int dataLength){
  
  m_dataLength = dataLength;
}

void ReadChipscope::SetDataStart(int dataStart){
  
  m_dataStart = dataStart;
}

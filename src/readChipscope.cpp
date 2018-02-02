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
  m_numberOfColumnsPerDataLine = 32;
}

ReadChipscope::~ReadChipscope(){
  // DESTRUCTOR
}

std::vector<std::vector<int>> ReadChipscope::BitReordering(std::vector<std::vector<int>> input){
  /* Bits are reorganised according to the output given by P2M manual
   * Please notice that pixels output are ordered as follow:
   *
   * | 24 | 16 | 8  | 0 |
   * | 25 | 17 | 9  | 1 |
   * | 26 | 18 | 10 | 2 |
   * | 27 | 19 | 11 | 3 |   For row going from 0 to 6
   * | 28 | 20 | 12 | 4 |
   * | 29 | 21 | 13 | 5 |
   * | 30 | 22 | 14 | 6 |
   * | 31 | 23 | 15 | 7 |
   */
  
  std::vector<std::vector<int>> inputRemapped = Resize2DVector(input.size(), input[0].size());

  int dataShift = 28;
  for (unsigned int dataLine = 0; dataLine < input[0].size(); dataLine++){
//    std::cout << "Data line: " << dataLine << std::endl;
    int blockShift = 1;
    int blockCounter = 0;
    int tmpRow = 7;
    int offset = 0;
    int endOfColumn = 0;
    int bitGroup = m_rowGroupLength;
    int groupRead = 0;
    for (unsigned int bit = 0; bit < input.size(); bit++){
      if (bit % m_rowGroupLength == 0 && bit > 0){
        groupRead++;
        //        std::cout << "groupRead * bitGroup = " << groupRead * bitGroup <<  std::endl;
        tmpRow = 7;
        blockCounter = 0;
        endOfColumn = 0;
        offset = 0;
        blockShift = 2;
      }
      if (blockCounter == 3){
        blockShift = 0;
      }
      
      //      std::cout << "Bit = " << bit << " and groupRead * bitGroup + offset + tmpRow * dataShift + encule = " << groupRead * bitGroup + offset + tmpRow * dataShift + blockShift <<  std::endl;
      inputRemapped[bit][dataLine] = input[groupRead * bitGroup + offset + tmpRow * dataShift + blockShift][dataLine];
      tmpRow--;
      if (tmpRow < 0){
        tmpRow = 7;
        blockCounter++;
        blockShift++;
        //        std::cout << "---" << std::endl;
      }
      if (blockCounter > 3){
        endOfColumn++;
        blockCounter = 0;
        blockShift = 1;
        offset = 4 * endOfColumn;
        tmpRow = 7;
        //        std::cout << "-----" << std::endl;
      }
    }
  }
  return inputRemapped;
}

std::vector<std::vector<int>> ReadChipscope::GetChipscopeDisplay(int dataToRead, std::vector<std::vector<int>> input){
  // The different data (SAMPLE or RESET) are 3360 bits long but there is an offset in Chipscope
  
  int begin = m_dataStart + dataToRead * (m_dataLength);
  int end  = begin + m_dataLength;
  std::vector<std::vector<int>> chipscopeDisplay(input.begin() + begin, input.begin() + end);
  return BitReordering(chipscopeDisplay);
}

std::vector<std::vector<int>> ReadChipscope::GetGainBits(std::vector<std::vector<int>> input){
  // From a block (Sample or Reset) get the 448 first elements corresponding to the gain
  
  int gainBegin = m_gainBegin;
  int gainEnd = m_gainEnd;
  std::vector<std::vector<int>> gainBits(input.begin() + gainBegin, input.begin() + gainEnd);
  return gainBits;
}

std::vector<std::vector<int>> ReadChipscope::GetGainDecimals(std::vector<std::vector<int>> input){
  
  std::vector<std::vector<int>> gainDecimals = Resize2DVector(m_rowGroupLength, input[0].size());

  for (unsigned int dataLine = 0; dataLine < input[0].size(); dataLine++){
    int offset = m_rowGroupLength;
    int adcBit = 0;
    int counter = 0;
    int bibite = 0;
    int pixelGainDecimal = 0;
    std::vector<int> pixelGainBits;
    for (unsigned int bit = 0; bit < input.size(); bit++){
      adcBit++;
      pixelGainBits.push_back(input[bibite * offset + counter][dataLine]);
      if (adcBit % m_numberOfGainBits == 0 && adcBit > 0 ){
        adcBit = 0;
        pixelGainDecimal = ConvertBitsToDecimals(pixelGainBits);
        gainDecimals[counter][dataLine] = pixelGainDecimal;
        counter++;
        bibite = -1;
        pixelGainBits.clear();
      }
      bibite++;
    }
  }
  return gainDecimals;
}

std::vector<std::vector<int>> ReadChipscope::GetFineBits(std::vector<std::vector<int>> input){
  // From a block (Sample or Reset) get the 1792 elements corresponding to the fine
  
  std::vector<std::vector<int>> fineBits(input.begin() + m_fineBegin, input.begin() + m_fineEnd);
  return fineBits;
}

std::vector<std::vector<int>> ReadChipscope::GetFineDecimals(std::vector<std::vector<int>> input){
  
  std::vector<std::vector<int>> fineDecimals = Resize2DVector(m_rowGroupLength, input[0].size());

  for (unsigned int dataLine = 0; dataLine < input[0].size(); dataLine++){
    int offset = m_rowGroupLength;
    int adcBit = 0;
    int counter = 0;
    int bibite = 0;
    int pixelFineDecimal = 0;
    std::vector<int> pixelFineBits;
    for (unsigned int bit = 0; bit < input.size(); bit++){
      adcBit++;
      pixelFineBits.push_back(input[bibite * offset + counter][dataLine]);
      if (adcBit % m_numberOfFineBits == 0 && adcBit > 0 ){
        adcBit = 0;
        pixelFineDecimal = ConvertBitsToDecimals(pixelFineBits);
        fineDecimals[counter][dataLine] = pixelFineDecimal;
        counter++;
        bibite = -1;
        pixelFineBits.clear();
      }
      bibite++;
    }
  }
  return fineDecimals;
}

std::vector<std::vector<int>> ReadChipscope::GetCoarseBits(std::vector<std::vector<int>> input){
  // From a block (Sample or Reset) get the 1120 elements corresponding to the coarse
  
  std::vector<std::vector<int>> coarseBits(input.begin() + m_coarseBegin, input.begin() + m_coarseEnd);
  return coarseBits;
}

std::vector<std::vector<int>> ReadChipscope::GetCoarseDecimals(std::vector<std::vector<int>> input){
  
  std::vector<std::vector<int>> coarseDecimals = Resize2DVector(m_rowGroupLength, input[0].size());
  
  for (unsigned int dataLine = 0; dataLine < input[0].size(); dataLine++){
    int offset = m_rowGroupLength;
    int adcBit = 0;
    int counter = 0;
    int bibite = 0;
    int pixelCoarseDecimal = 0;
    std::vector<int> pixelCoarseBits;
    for (unsigned int bit = 0; bit < input.size(); bit++){
      adcBit++;
      pixelCoarseBits.push_back(input[bibite * offset + counter][dataLine]);
      if (adcBit % m_numberOfCoarseBits == 0 && adcBit > 0 ){
        adcBit = 0;
        pixelCoarseDecimal = ConvertBitsToDecimals(pixelCoarseBits);
        coarseDecimals[counter][dataLine] = pixelCoarseDecimal;
        counter++;
        bibite = -1;
        pixelCoarseBits.clear();
      }
      bibite++;
    }
  }
  return coarseDecimals;
}

int ReadChipscope::ConvertBitsToDecimals(std::vector<int> inputBits){
  // Conversion from bit to decimal
  
  int decimal = 0;
  for (unsigned int bit = 0; bit < inputBits.size(); bit++){
    decimal += inputBits.at(bit) * pow(2,inputBits.size() - (1 + bit));
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
  // The interresting data are in column 3 to 47 but the second half (35-47) should be in the first
  // position to get a continuous image
  
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

std::vector<std::vector<int>> ReadChipscope::Resize2DVector(int nbOfLines, int nbOfColumns){
  // Method for resizing a 2D vector according to a certain number of lines and columns
  
  std::vector<std::vector<int>> vectorResized;
  vectorResized.resize(nbOfLines);
  for (auto &bit : vectorResized){
    bit.resize(nbOfColumns);
  }
  return vectorResized;
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
  int nbOfLines = 7;
  int nbOfColumns = 1408; // 32 columns * 44 data lines
  outputImage = Resize2DVector(nbOfLines, nbOfColumns);
  
    for (unsigned int dataLine = 0; dataLine < input[0].size(); dataLine++){
    //    std::cout << "Data Line: " << dataLxine << std::endl;
    int tmpRow = 0;
    int tmpCol =  dataLine * (m_numberOfColumnsPerDataLine - 1);
    for (unsigned int bit = 0; bit < input.size(); bit++){
      if (bit % m_numberOfColumnsPerDataLine == 0 && bit > 0){
        tmpRow++;
        tmpCol = dataLine * (m_numberOfColumnsPerDataLine - 1);
        //        std::cout << "row, col: " << tmpRow << ", " << tmpCol << " bit: " << bit << std::endl;
      }
      outputImage[tmpRow][tmpCol] = input[bit][dataLine];
      //        std::cout << "row, col: " << tmpRow << ", " << tmpCol << " bit: " << bit << std::endl;
      tmpCol++;
    }
  }
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

void ReadChipscope::SetNumberPixels(int numberOfPixels){
  m_rowGroupLength = numberOfPixels;
}

void ReadChipscope::SetNumberGainBits(int numberOfGainBits){
  m_numberOfGainBits = numberOfGainBits;
}

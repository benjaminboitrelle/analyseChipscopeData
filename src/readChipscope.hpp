//
//  readChipscope.hpp
//  analyseChipscopeData
//
//  Created by Benjamin BOITRELLE on 29/11/2017.
//

#ifndef readChipscope_hpp
#define readChipscope_hpp

#include <fstream>
#include <string>
#include <vector>


class ReadChipscope{

  public:
    ReadChipscope();
    ~ReadChipscope();

    std::vector<int> ConvertStringToInt(std::vector<std::string>& inputString, std::vector<int>& output);
    std::vector<std::vector<int>> ReadPrnFile(std::fstream& myFile);

    //void readPrnFile(std::fstream& myFile, std::vector<std::vector<int>> &outputImageVectorised);
    void InverseBits(std::vector<int> input);
    void SplitString(const std::string &inputString, char delimiterChar, std::vector<std::string> &outputVector);
    std::vector<std::vector<int>> GetChipscopeDisplay(int dataToRead, std::vector<std::vector<int>> input);
    std::vector<std::vector<int>> GetGainBits(std::vector<std::vector<int>> input);
    std::vector<std::vector<int>> GetGainDecimals(std::vector<std::vector<int>> input);
    std::vector<std::vector<int>> GetFineBits(std::vector<std::vector<int>> input);
    std::vector<std::vector<int>> GetFineDecimals(std::vector<std::vector<int>> input);
    std::vector<std::vector<int>> GetCoarseBits(std::vector<std::vector<int>> input);
    std::vector<std::vector<int>> GetCoarseDecimals(std::vector<std::vector<int>> input);
    std::vector<std::vector<int>> PrepareVectorisedImage(std::vector<std::vector<int>> input);
    int ConvertBitsToDecimals(std::vector<int> inputBits);
  
  void SetDataLength(int dataLength);
  void SetDataStart(int dataStart);


  private:
    int m_dataLength;
    int m_dataStart;
    int m_chipscopeLength;
    int m_numberOfGainBits;
    int m_numberOfFineBits;
    int m_numberOfCoarseBits;
    int m_rowGroupLength;
    int m_gainBegin;
    int m_gainEnd;
    int m_fineBegin;
    int m_fineEnd;
    int m_coarseBegin;
    int m_coarseEnd;

};
#endif /* readChipscope_hpp */

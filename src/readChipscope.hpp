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

    std::vector<int> convertStringToInt(std::vector<std::string>& inputString, std::vector<int>& output);
    void readPrnFile(std::fstream& myFile, std::vector<std::vector<int>> &outputImageVectorised);
    void splitString(const std::string &inputString, char delimiterChar, std::vector<std::string> &outputVector);
    std::vector<std::vector<int>> getChipscopeDisplay(int dataToRead, std::vector<std::vector<int>> input);
    std::vector<std::vector<int>> getGainBits(std::vector<std::vector<int>> input);
    std::vector<std::vector<int>> getGainDecimals(std::vector<std::vector<int>> input);
    std::vector<std::vector<int>> getFineBits(std::vector<std::vector<int>> input);
    std::vector<std::vector<int>> getFineDecimals(std::vector<std::vector<int>> input);
    std::vector<std::vector<int>> getCoarseBits(std::vector<std::vector<int>> input);
    std::vector<std::vector<int>> getCoarseDecimals(std::vector<std::vector<int>> input);
    int convertBitsToDecimals(std::vector<int> inputBits);

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

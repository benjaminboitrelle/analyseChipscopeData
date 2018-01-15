//
//  plotHistogram.hpp
//  analyseChipscopeData
//
//  Created by Benjamin BOITRELLE on 01/12/2017.
//

#ifndef plotHistogram_hpp
#define plotHistogram_hpp

#include <fstream>
#include <string>
#include <vector>

#include "TH2F.h"

class PlotHistogram{

  public:
    PlotHistogram();
    ~PlotHistogram();

    void CreateScatterPlot(std::string title, std::string histoTitle, int rowBinning, int rowMin, int rowMax, int columnBinning, int columnMin, int columnMax, std::vector<std::vector<int>> inputData);

  private:
};
#endif /* plotHistogram_hpp */

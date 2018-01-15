//
//  plotHistogram.hpp
//  analyseChipscopeData
//
//  Created by Benjamin BOITRELLE on 01/12/2017.
//

#include "plotHistogram.hpp"

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

#include "TH2F.h"
#include "TCanvas.h"
#include "TStyle.h"

PlotHistogram::PlotHistogram(){
  // Constructor
}

PlotHistogram::~PlotHistogram(){
  // Destructor
}

void PlotHistogram::CreateScatterPlot(std::string title, std::string histoTitle, int rowBinning, int rowMin, int rowMax, int columnBinning, int columnMin, int columnMax, std::vector<std::vector<int>> inputData){
  // Plot an hitogramm from a vector of data set

  std::unique_ptr<TCanvas> canvas (new TCanvas("canvas", "canvas", 1200, 1100));
  auto histoToPlot = new TH2F(title.c_str(), histoTitle.c_str(), columnBinning, columnMin, columnMax, rowBinning, rowMin, rowMax);
  for (unsigned int row = 0; row < inputData.size(); row++){
    for (unsigned int column = 0; column < inputData[row].size(); column++){
      histoToPlot->Fill(column, row, inputData[row][column]);
    }
  }
  gStyle->SetPalette(kRainBow);
  histoToPlot->Draw("COLZ");
  histoToPlot->Write();  
  canvas->SaveAs("/Users/ben/PostDoc/ChipscopeSOLEIL/analyseChipscopeData/output/result.png");
}

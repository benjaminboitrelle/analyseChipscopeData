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

#include "TH2I.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TAxis.h"
#include "TGaxis.h"

PlotHistogram::PlotHistogram(){
  // Constructor
}

PlotHistogram::~PlotHistogram(){
  // Destructor
}

void PlotHistogram::CreateScatterPlot(std::string title, std::string histoTitle, std::string fileName, int rowBinning, int rowMin, int rowMax, int columnBinning, int columnMin, int columnMax, std::vector<std::vector<int>> inputData){
  // Plot an hitogramm from a vector of data set

  std::unique_ptr<TCanvas> canvas (new TCanvas("canvas", "canvas", 800, 800));
  TH2I *histoToPlot = new TH2I(title.c_str(), histoTitle.c_str(), columnBinning, columnMin, columnMax, rowBinning, rowMin, rowMax);
  for (unsigned int row = rowMin; row < rowMax; row++){
    for (unsigned int column = columnMin; column < columnMax; column++){
//      for (unsigned int row = 0; row < inputData.size(); row++){
//        for (unsigned int column = 0; column < inputData[row].size(); column++){

      histoToPlot->Fill(column, row, inputData[row][column]);
      //histoToPlot->SetBinContent(column, row, inputData[row][column]);
    }
  }
  gStyle->SetOptStat(kFALSE);
  gStyle->SetPalette(kRainBow);
  histoToPlot->Draw("COLZ");
  ReverseYAxis(histoToPlot);
  ReverseXAxis(histoToPlot);
  histoToPlot->Write();
  canvas->Update();
  canvas->SaveAs(fileName.c_str());
}

void PlotHistogram::ReverseXAxis(TH2I *histo2D){
  // Remove the current axis
  histo2D->GetXaxis()->SetLabelOffset(999);
  histo2D->GetXaxis()->SetTickLength(0);
  
  // Redraw the new axis
  gPad->Update();
  TGaxis *newaxis = new TGaxis(gPad->GetUxmax(),
                               gPad->GetUymin(),
                               gPad->GetUxmin(),
                               gPad->GetUymin(),
                               histo2D->GetXaxis()->GetXmin(),
                               histo2D->GetXaxis()->GetXmax(),
                               510,
                               "-");
  newaxis->SetLabelOffset(-0.03);
  newaxis->Draw();
}

void PlotHistogram:: ReverseYAxis(TH2I *histo2D){
  // Remove the current axis
  histo2D->GetYaxis()->SetLabelOffset(999);
  histo2D->GetYaxis()->SetTickLength(0);
  
  // Redraw the new axis
  gPad->Update();
  TGaxis *newaxis = new TGaxis(gPad->GetUxmin(),
                               gPad->GetUymax(),
                               gPad->GetUxmin()-0.001,
                               gPad->GetUymin(),
                               histo2D->GetYaxis()->GetXmin(),
                               histo2D->GetYaxis()->GetXmax(),
                               510,"+");
  newaxis->SetLabelOffset(-0.03);
  newaxis->Draw();
}

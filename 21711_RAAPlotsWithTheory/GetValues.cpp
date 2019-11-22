#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;

#include "TH1D.h"
#include "TGraph.h"
#include "TH2D.h"
#include "TCanvas.h"
#include "TPad.h"
#include "TFile.h"
#include "TGaxis.h"
#include "TLatex.h"
#include "TLegend.h"
#include "TColor.h"
#include "TBox.h"
#include "TGraphAsymmErrors.h"

#include "CommandLine.h"

#define R02 0
#define R03 1
#define R04 2
#define R06 3
#define R08 4
#define R10 5

#define C0 0
#define C1 1
#define C2 2
#define C3 3

int Color[5] = {-1, -1, -1, -1, -1};

int main(int argc, char *argv[]);
void GetRAA(TH1D &H, string FileName, int R, int C, bool Sys = false);
void Rebin(TH1D &H1, TH1D &H2);

int main(int argc, char *argv[])
{
   string File = "RAA_Smooth.root";
  
   double Bins[] = {200, 250, 300, 400, 500, 1000};
   const int NRadii = 6;
   TH1D HC00RAA[NRadii];
   HC00RAA[0] = TH1D("HR02C00RAA", "", 5, Bins);   GetRAA(HC00RAA[0], File, R02, C0, false);
   HC00RAA[1] = TH1D("HR03C00RAA", "", 5, Bins);   GetRAA(HC00RAA[1], File, R03, C0, false);
   HC00RAA[2] = TH1D("HR04C00RAA", "", 5, Bins);   GetRAA(HC00RAA[2], File, R04, C0, false);
   HC00RAA[3] = TH1D("HR06C00RAA", "", 5, Bins);   GetRAA(HC00RAA[3], File, R06, C0, false);
   HC00RAA[4] = TH1D("HR08C00RAA", "", 5, Bins);   GetRAA(HC00RAA[4], File, R08, C0, false);
   HC00RAA[5] = TH1D("HR10C00RAA", "", 5, Bins);   GetRAA(HC00RAA[5], File, R10, C0, false);
   
   TH1D HC00RAASys[NRadii];
   HC00RAASys[0] = TH1D("HR02C00RAASys", "", 5, Bins);   GetRAA(HC00RAASys[0], File, R02, C0, true);
   HC00RAASys[1] = TH1D("HR03C00RAASys", "", 5, Bins);   GetRAA(HC00RAASys[1], File, R03, C0, true);
   HC00RAASys[2] = TH1D("HR04C00RAASys", "", 5, Bins);   GetRAA(HC00RAASys[2], File, R04, C0, true);
   HC00RAASys[3] = TH1D("HR06C00RAASys", "", 5, Bins);   GetRAA(HC00RAASys[3], File, R06, C0, true);
   HC00RAASys[4] = TH1D("HR08C00RAASys", "", 5, Bins);   GetRAA(HC00RAASys[4], File, R08, C0, true);
   HC00RAASys[5] = TH1D("HR10C00RAASys", "", 5, Bins);   GetRAA(HC00RAASys[5], File, R10, C0, true);
   
   ofstream OutFile("RAA_0to10.txt");
   OutFile << "# <ptmin> <ptmax> <value> <stat> <sys> for R = 0.2, 0.3, 0.4, 0.6, 0.8, 1.0" << endl;
   OutFile << "if values are 0 there is no data for that momentum range" << endl;
   
   for(int i = 0; i < 5; i++)
   {
      OutFile << Bins[i] << " " << Bins[i+1] << " ";
      
      OutFile << HC00RAA[0].GetBinContent(i+1) << " " << HC00RAA[0].GetBinError(i+1) << " " << HC00RAASys[0].GetBinError(i+1) << " ";
      OutFile << HC00RAA[1].GetBinContent(i+1) << " " << HC00RAA[1].GetBinError(i+1) << " " << HC00RAASys[1].GetBinError(i+1) << " ";
      OutFile << HC00RAA[2].GetBinContent(i+1) << " " << HC00RAA[2].GetBinError(i+1) << " " << HC00RAASys[2].GetBinError(i+1) << " ";
      OutFile << HC00RAA[3].GetBinContent(i+1) << " " << HC00RAA[3].GetBinError(i+1) << " " << HC00RAASys[3].GetBinError(i+1) << " ";
      OutFile << HC00RAA[4].GetBinContent(i+1) << " " << HC00RAA[4].GetBinError(i+1) << " " << HC00RAASys[4].GetBinError(i+1) << " ";
      OutFile << HC00RAA[5].GetBinContent(i+1) << " " << HC00RAA[5].GetBinError(i+1) << " " << HC00RAASys[5].GetBinError(i+1) << " ";
      OutFile << endl;
   }
   
   OutFile.close();
   
   return 0;
}

void GetRAA(TH1D &H, string FileName, int R, int C, bool Sys)
{
   TFile File(FileName.c_str());

   string Rs[] = {"2", "3", "4", "6", "8", "10"};
   string Cs[] = {"0to10", "10to30", "30to50", "50to90"};

   string HistogramName = "";
   
   HistogramName = HistogramName + "akCs" + Rs[R] + "PU3PFFlowJetAnalyzer";
   HistogramName = HistogramName + "_Cent" + Cs[C];
   if(Sys == false)   HistogramName = HistogramName + "_1";
   else               HistogramName = HistogramName + "_2";

   TH1D *Histogram = (TH1D *)File.Get(HistogramName.c_str());
   Rebin(*Histogram, H);
   
   File.Close();
}

void Rebin(TH1D &H1, TH1D &H2)
{
   H2.Reset();

   for(int i = 1; i <= H1.GetNbinsX(); i++)
   {
      double V = H1.GetBinContent(i);
      double E = H1.GetBinError(i);

      int B = H2.FindBin(H1.GetBinCenter(i));

      V = V + H2.GetBinContent(B);
      E = sqrt(E * E + H2.GetBinError(B) * H2.GetBinError(B));

      H2.SetBinContent(B, V);
      H2.SetBinError(B, E);
   }
}

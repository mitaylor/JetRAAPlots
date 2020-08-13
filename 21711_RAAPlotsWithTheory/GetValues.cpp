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

#define VAL 0
#define STA 1
#define SYS 2

const int NRad = 6;
const string radius[NRad] = {"02", "03", "04", "06", "08", "10"};

const int NBin = 5;
const double bins[NBin + 1] = {200, 250, 300, 400, 500, 1000};

int main(int argc, char *argv[]);
void GetRAA(string FileName, double G[3][NBin], int R, int C);
void Rebin(TH1D &H1, TH1D &H2);

int main(int argc, char *argv[])
{
   string File = "RAA_Smooth.root";
   
   double GPbPbC0[NRad][3][NBin] = {0};   // [0] = value, [1] = stat, [2] = sys
   double GPbPbC1[NRad][3][NBin] = {0};
   double GPbPbC2[NRad][3][NBin] = {0};
   double GPbPbC3[NRad][3][NBin] = {0};

   for(int i = 0; i < NRad; i++)
   {
      GetRAA(FileName, GPbPbC0[i], i, C0);
      GetRAA(FileName, GPbPbC1[i], i, C1);
      GetRAA(FileName, GPbPbC2[i], i, C2);
      GetRAA(FileName, GPbPbC3[i], i, C3);
      
      string OutFileName = "R" + radius[i] + "_RAA.txt";
      ofstream OutFile(OutFileName.c_str());
      
      for(int j = 0; j < NBin; j++)
      {
         OutFile << bins[j] << "\t" << bins[j+1];
         OutFile << "\t" << GPbPbC0[i][VAL][j];
         OutFile << "\t" << GPbPbC1[i][VAL][j];
         OutFile << "\t" << GPbPbC2[i][VAL][j];
         OutFile << "\t" << GPbPbC3[i][VAL][j];
         OutFile << endl;
      }
      
      OutFile.close();
      
      OutFileName = "R" + radius[i] + "_RAA_Stat.txt";
      OutFile.open(OutFileName.c_str());
      
      for(int j = 0; j < NBin; j++)
      {
         OutFile << bins[j] << "\t" << bins[j+1];
         OutFile << "\t" << GPbPbC0[i][STA][j];
         OutFile << "\t" << GPbPbC1[i][STA][j];
         OutFile << "\t" << GPbPbC2[i][STA][j];
         OutFile << "\t" << GPbPbC3[i][STA][j];
         OutFile << endl;
      }
      
      OutFile.close();
      
      OutFileName = "R" + radius[i] + "_RAA_Sys.txt";
      OutFile.open(OutFileName.c_str());
      
      for(int j = 0; j < NBin; j++)
      {
         OutFile << bins[j] << "\t" << bins[j+1];
         OutFile << "\t" << GPbPbC0[i][SYS][j];
         OutFile << "\t" << GPbPbC1[i][SYS][j];
         OutFile << "\t" << GPbPbC2[i][SYS][j];
         OutFile << "\t" << GPbPbC3[i][SYS][j];
         OutFile << endl;
      }
      
      OutFile.close();
   }
   
   return 0;
}

void GetRAA(string FileName, double G[3][NBin], int R, int C)
{
   TFile File(FileName.c_str());
   TH1D H = TH1D("temp", "", 5, bins);

   string Rs[] = {"2", "3", "4", "6", "8", "10"};
   string Cs[] = {"0to10", "10to30", "30to50", "50to90"};

   string HistBase = "akCs" + Rs[R] + "PU3PFFlowJetAnalyzer_Cent" + Cs[C];

   // statistical error
   string HistName = HistBase + "_1";
   TH1D *Histogram = (TH1D *)File.Get(HistName.c_str());
   Rebin(*Histogram, H);
   
   for(int i = 0; i < NBin; i++)
   {
      G[VAL][bin] = H.GetBinContent(i+1);
      G[STA][bin] = H.GetBinError(i+1);
   }
   
   // systematic error
   HistName = HistBase + "_2";
   TH1D *Histogram = (TH1D *)File.Get(HistName.c_str());
   Rebin(*Histogram, H);
   
   for(int i = 0; i < NBin; i++)
   {
      G[SYS][bin] = H.GetBinError(i+1);
   }
   
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

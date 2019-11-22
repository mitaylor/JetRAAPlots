#include <iostream>
#include <fstream>
#include <cmath>

using namespace std;

#include "TFile.h"
#include "TH2D.h"
#include "TCanvas.h"
#include "TROOT.h"
#include "TPad.h"
#include "TGaxis.h"
#include "TLatex.h"
#include "TGraphAsymmErrors.h"
#include "TLegend.h"

#include "SetStyle.h"
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
#define PP 4

#define VAL 0
#define STA 1
#define SYS 2

const int NRad = 6;
const string radius[NRad] = {"02", "03", "04", "06", "08", "10"};

const int NBin = 5;
const double bins[NBin + 1] = {200, 250, 300, 400, 500, 1000};

int main(int argc, char *argv[]);
void GetGraph(string FileName, double G[3][NBin], int R, int Cent, double Shift);

int main(int argc, char *argv[])
{
   SetThesisStyle();

   CommandLine CL(argc, argv);

   string FileName = CL.Get("Input", "Spectra_Smooth.root");

   double GPP[NRad][3][NBin] = {0};       // [0] = value, [1] = stat, [2] = sys
   double GPbPbC0[NRad][3][NBin] = {0};
   double GPbPbC1[NRad][3][NBin] = {0};
   double GPbPbC2[NRad][3][NBin] = {0};
   double GPbPbC3[NRad][3][NBin] = {0};

   for(int i = 0; i < NRad; i++)
   {
      GetGraph(FileName, GPP[i], i, PP, 1);
      GetGraph(FileName, GPbPbC0[i], i, C0, 1e-1);
      GetGraph(FileName, GPbPbC1[i], i, C1, 1e-2);
      GetGraph(FileName, GPbPbC2[i], i, C2, 1e-3);
      GetGraph(FileName, GPbPbC3[i], i, C3, 1e-4);
      
      string OutFileName = "R" + radius[i] + "_Spectra.txt";
      ofstream OutFile(OutFileName.c_str());
      
      for(int j = 0; j < NBin; j++)
      {
         OutFile << bins[j] << "\t" << bins[j+1];
         OutFile << "\t" << GPP[i][VAL][j];
         OutFile << "\t" << GPbPbC0[i][VAL][j];
         OutFile << "\t" << GPbPbC1[i][VAL][j];
         OutFile << "\t" << GPbPbC2[i][VAL][j];
         OutFile << "\t" << GPbPbC3[i][VAL][j];
         OutFile << endl;
      }
      
      OutFile.close();
      
      OutFileName = "R" + radius[i] + "_Spectra_Stat.txt";
      OutFile.open(OutFileName.c_str());
      
      for(int j = 0; j < NBin; j++)
      {
         OutFile << bins[j] << "\t" << bins[j+1];
         OutFile << "\t" << GPP[i][STA][j];
         OutFile << "\t" << GPbPbC0[i][STA][j];
         OutFile << "\t" << GPbPbC1[i][STA][j];
         OutFile << "\t" << GPbPbC2[i][STA][j];
         OutFile << "\t" << GPbPbC3[i][STA][j];
         OutFile << endl;
      }
      
      OutFile.close();
      
      OutFileName = "R" + radius[i] + "_Spectra_Sys.txt";
      OutFile.open(OutFileName.c_str());
      
      for(int j = 0; j < NBin; j++)
      {
         OutFile << bins[j] << "\t" << bins[j+1];
         OutFile << "\t" << GPP[i][SYS][j];
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

void GetGraph(string FileName, double G[3][NBin], int R, int C, double Shift)
{
   TFile File(FileName.c_str());

   string Rs[] = {"2", "3", "4", "6", "8", "10"};
   string Cs[] = {"Cent0to10", "Cent10to30", "Cent30to50", "Cent50to90"};

   string HistBase = "";
   if(C != PP)
      HistBase = "akCs" + Rs[R] + "PU3PFFlowJetAnalyzer_" + Cs[C];
   else
      HistBase = "ak" + Rs[R] + "PFJetAnalyzer_" + Cs[0];

   // statistical error
   string HistName = HistBase + "_1";

   TH1D *H = (TH1D *)File.Get(HistName.c_str());
   if(H == nullptr)
      cout << HistName << endl;

   for(int i = 1; i <= H->GetNbinsX(); i++)
   {
      double X = H->GetXaxis()->GetBinLowEdge(i);
      double Y = H->GetBinContent(i) * Shift;
      double EY = H->GetBinError(i) * Shift;

      int bin = 0;
      
      for(int j = 0; j < NBin; j++)
         if(abs(X - bins[j]) < 0.01) bin = j;
      
      G[VAL][bin] = Y;
      G[STA][bin] = EY;
   }
   
   // systematic error
   HistName = HistBase + "_2";
   
   H = (TH1D *)File.Get(HistName.c_str());
   if(H == nullptr)
      cout << HistName << endl;

   for(int i = 1; i <= H->GetNbinsX(); i++)
   {
      double X = H->GetXaxis()->GetBinLowEdge(i);
      double Y = H->GetBinContent(i) * Shift;
      double EY = H->GetBinError(i) * Shift;

      int bin = 0;
      
      for(int j = 0; j < NBin; j++)
         if(abs(X - bins[j]) < 0.01) bin = j;
      
      G[SYS][bin] = EY;
   }

   File.Close();
}




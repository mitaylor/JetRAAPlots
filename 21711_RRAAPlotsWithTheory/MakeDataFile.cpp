#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;

#include "TH1D.h"
#include "TGraphAsymmErrors.h"
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

#include "CommandLine.h"

#define PT0 0
#define PT1 1
#define PT2 2
#define PT3 3
#define PT4 4

#define C0 0
#define C1 1
#define C2 2
#define C3 3

int Color[5] = {-1, -1, -1, -1, -1};

int main(int argc, char *argv[]);
void SetPad(TPad &P);
void SetAxis(TGaxis &A, bool Pres);
void GetRRAA(TGraphAsymmErrors &H, string FileName, int R, int C, bool Sys = false);
vector<TGraphAsymmErrors> GetTheory(string TheoryFileName, int BaseColor);

int main(int argc, char *argv[])
{
   string File = "RRAA_Smooth_RedStat.root";
   
   const int NBins = 5;
   TGraphAsymmErrors HC0RRAA[NBins];

   GetRRAA(HC0RRAA[0], File, PT0, C0, false);
   GetRRAA(HC0RRAA[1], File, PT1, C0, false);
   GetRRAA(HC0RRAA[2], File, PT2, C0, false);
   GetRRAA(HC0RRAA[3], File, PT3, C0, false);
   GetRRAA(HC0RRAA[4], File, PT4, C0, false);
   
   TGraphAsymmErrors HC0RRAASys[NBins];
   
   GetRRAA(HC0RRAASys[0], File, PT0, C0, true);
   GetRRAA(HC0RRAASys[1], File, PT1, C0, true);
   GetRRAA(HC0RRAASys[2], File, PT2, C0, true);
   GetRRAA(HC0RRAASys[3], File, PT3, C0, true);
   GetRRAA(HC0RRAASys[4], File, PT4, C0, true);
   
   ofstream OutFile("RRAA_0to10.txt");
   
   OutFile << "# <ptmin> <ptmax> <value> <stat> <sys> for R = 0.2, 0.3, 0.4, 0.6, 0.8, 1.0" << endl;
   OutFile << "# if values are 0 there is no data for that momentum range" << endl;
   
   
   double* Y = HC0RRAA[2].GetY();
   double* Ystat = HC0RRAA[2].GetEYhigh();
   double* Ysys = HC0RRAASys[2].GetEYhigh();
   OutFile << "300 400 1 0 0 ";
   OutFile << Y[1] << " " << Ystat[1] << " " << Ysys[2] << " "; //R=3
   OutFile << Y[2] << " " << Ystat[2] << " " << Ysys[3] << " "; //R=4
   OutFile << Y[4] << " " << Ystat[4] << " " << Ysys[5] << " "; //R=6
   OutFile << Y[6] << " " << Ystat[6] << " " << Ysys[7] << " "; //R=8
   OutFile << Y[8] << " " << Ystat[8] << " " << Ysys[9] << " "; //R=10
   OutFile << endl;
   
   Y = HC0RRAA[3].GetY();
   Ystat = HC0RRAA[3].GetEYhigh();
   Ysys = HC0RRAASys[3].GetEYhigh();
   OutFile << "400 500 1 0 0 ";
   OutFile << Y[1] << " " << Ystat[1] << " " << Ysys[2] << " "; //R=3
   OutFile << Y[2] << " " << Ystat[2] << " " << Ysys[3] << " "; //R=4
   OutFile << Y[4] << " " << Ystat[4] << " " << Ysys[5] << " "; //R=6
   OutFile << Y[6] << " " << Ystat[6] << " " << Ysys[7] << " "; //R=8
   OutFile << Y[8] << " " << Ystat[8] << " " << Ysys[9] << " "; //R=10
   OutFile << endl;
   
   Y = HC0RRAA[4].GetY();
   HC0RRAA[4].Print("all");
   Ystat = HC0RRAA[4].GetEYhigh();
   Ysys = HC0RRAASys[4].GetEYhigh();
   HC0RRAASys[4].Print("all");
   OutFile << "500 1000 1 0 0 ";
   OutFile << Y[1] << " " << Ystat[1] << " " << Ysys[2] << " "; //R=3
   OutFile << Y[2] << " " << Ystat[2] << " " << Ysys[3] << " "; //R=4
   OutFile << Y[4] << " " << Ystat[4] << " " << Ysys[5] << " "; //R=6
   OutFile << Y[6] << " " << Ystat[6] << " " << Ysys[7] << " "; //R=8
   OutFile << Y[8] << " " << Ystat[8] << " " << Ysys[9] << " "; //R=10
   OutFile << endl;
   
   OutFile.close();

   return 0;
}

void GetRRAA(TGraphAsymmErrors &H, string FileName, int P, int C, bool Sys)
{
   TFile File(FileName.c_str());

   string Ps[] = {"200p0to250p0", "250p0to300p0", "300p0to400p0", "400p0to500p0", "500p0to1000p0"};
   string Cs[] = {"0to10", "10to30", "30to50", "50to90"};

   string HistogramName = "";
   
   HistogramName = HistogramName + "Cent" + Cs[C];
   HistogramName = HistogramName + "_" + Ps[P];
   if(Sys == false)   HistogramName = HistogramName + "_1";
   else               HistogramName = HistogramName + "_2";

   if(Sys == false)
   {
      TGraphAsymmErrors *G = (TGraphAsymmErrors *)File.Get(HistogramName.c_str());
      
      for(int i = 0; i < G->GetN(); i++)
      {
         double X, Y, EX1, EX2, EY1, EY2;
         G->GetPoint(i, X, Y);
         EX1 = G->GetErrorXlow(i);
         EX2 = G->GetErrorXhigh(i);
         EY1 = G->GetErrorYlow(i);
         EY2 = G->GetErrorYhigh(i);
         H.SetPoint(i, X, Y);
         H.SetPointEXlow(i, EX1);
         H.SetPointEXhigh(i, EX2);
         H.SetPointEYlow(i, EY1);
         H.SetPointEYhigh(i, EY2);
      }
   }
   else
   {
      TH1D *Histogram = (TH1D *)File.Get(HistogramName.c_str());

      for(int i = 1; i <= Histogram->GetNbinsX(); i++)
      {
         double X = Histogram->GetBinCenter(i);
         double DX = Histogram->GetXaxis()->GetBinUpEdge(i) - X;
         double Y = Histogram->GetBinContent(i);
         double DY = Histogram->GetBinError(i);
         H.SetPoint(i, X, Y);
         H.SetPointEXlow(i, DX * 0.8);
         H.SetPointEXhigh(i, DX * 0.8);
         H.SetPointEYlow(i, DY);
         H.SetPointEYhigh(i, DY);
      }
   }

   File.Close();
}

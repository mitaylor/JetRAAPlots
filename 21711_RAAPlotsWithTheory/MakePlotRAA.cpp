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
void SetPad(TPad &P);
void SetAxis(TGaxis &A);
void GetRAA(TH1D &H, string FileName, int R, int C, bool Sys = false);
void Rebin(TH1D &H1, TH1D &H2);
void DrawLumiTAA(double Lumi, double TAA[4], double XMin, double XMax);
vector<TGraphAsymmErrors> GetTheoryGraphs(string TheoryFileName, int BaseColor);

int main(int argc, char *argv[])
{
   CommandLine CL(argc, argv);

   string TheoryType = CL.Get("TheoryType", ""); // "", "All", "Jewel", "Pyquen", "Hybrid", "Ivan", "NoData", "SCET"
   bool DoTheory = (TheoryType == "All") || (TheoryType == "NoData");
   bool DoJewel = (TheoryType == "Jewel");
   bool DoPyquen = (TheoryType == "Pyquen");
   bool DoHybrid = (TheoryType == "Hybrid");
   bool DoIvan = (TheoryType == "Ivan");
   bool DoSCET = (TheoryType == "SCET");
   bool NoData = DoJewel || DoPyquen || DoHybrid || DoIvan || DoSCET || (TheoryType == "NoData");

   // Preamble: sizing
   int PanelSize = 500;
   int PaddingWidthL = 100;
   int PaddingWidthR = 50;
   int PaddingHeight = 100;
   int CanvasWidth = PanelSize * 3 + PaddingWidthL + PaddingWidthR;
   int CanvasHeight = PanelSize * 2 + PaddingHeight * 2;

   double PadX0 = (double)PaddingWidthL / CanvasWidth;
   double PadY0 = (double)PaddingHeight / CanvasHeight;
   double PadDX = (double)PanelSize / CanvasWidth;
   double PadDY = (double)PanelSize / CanvasHeight;

   double XMin = 150;
   double XMax = 1500;
   double YMin = 0.0;
   double YMax = 1.199;

   string File = "RAA_Smooth.root";

   double Lumi = 0.023;
   double TAAFull[4] = {0.018646, 0.025341, 0.051233, 0.100719};
   double TAATheory[4] = {0.018646, -1, -1, -1};
   double *TAA = (DoTheory ? TAATheory : TAAFull);

   // Start declaring canvas, pad, axis, and axis title (as TLatex)
   TCanvas Canvas("Canvas", "", CanvasWidth, CanvasHeight);

   TH2D HWorld("HWorld", ";;", 100, XMin, XMax, 100, YMin, YMax);
   HWorld.SetStats(0);

   const int NPad = 6;
   TPad *P[NPad];
   P[0] = new TPad("P1", "", PadX0 + PadDX * 0, PadY0 + PadDY * 1, PadX0 + PadDX * 1, PadY0 + PadDY * 2, 0);
   P[1] = new TPad("P2", "", PadX0 + PadDX * 1, PadY0 + PadDY * 1, PadX0 + PadDX * 2, PadY0 + PadDY * 2, 0);
   P[2] = new TPad("P3", "", PadX0 + PadDX * 2, PadY0 + PadDY * 1, PadX0 + PadDX * 3, PadY0 + PadDY * 2, 0);
   P[3] = new TPad("P4", "", PadX0 + PadDX * 0, PadY0 + PadDY * 0, PadX0 + PadDX * 1, PadY0 + PadDY * 1, 0);
   P[4] = new TPad("P5", "", PadX0 + PadDX * 1, PadY0 + PadDY * 0, PadX0 + PadDX * 2, PadY0 + PadDY * 1, 0);
   P[5] = new TPad("P6", "", PadX0 + PadDX * 2, PadY0 + PadDY * 0, PadX0 + PadDX * 3, PadY0 + PadDY * 1, 0);
   
   SetPad(*P[0]);
   SetPad(*P[1]);
   SetPad(*P[2]);
   SetPad(*P[3]);
   SetPad(*P[4]);
   SetPad(*P[5]);
  
   Canvas.cd();

   TGaxis AxisX1(PadX0 + PadDX * 0, PadY0 + PadDY * 0, PadX0 + PadDX * 1, PadY0 + PadDY * 0, XMin, XMax, 510, "G");
   TGaxis AxisX2(PadX0 + PadDX * 1, PadY0 + PadDY * 0, PadX0 + PadDX * 2, PadY0 + PadDY * 0, XMin, XMax, 510, "G");
   TGaxis AxisX3(PadX0 + PadDX * 2, PadY0 + PadDY * 0, PadX0 + PadDX * 3, PadY0 + PadDY * 0, XMin, XMax, 510, "G");
   TGaxis AxisY1(PadX0 + PadDX * 0, PadY0 + PadDY * 0, PadX0 + PadDX * 0, PadY0 + PadDY * 1, YMin, YMax, 510, "");
   TGaxis AxisY2(PadX0 + PadDX * 0, PadY0 + PadDY * 1, PadX0 + PadDX * 0, PadY0 + PadDY * 2, YMin, YMax, 510, "");

   SetAxis(AxisX1);
   SetAxis(AxisX2);
   SetAxis(AxisX3);
   SetAxis(AxisY1);
   SetAxis(AxisY2);

   TLatex Latex;
   Latex.SetNDC();

   Latex.SetTextFont(42);
   Latex.SetTextSize(0.030);
   Latex.SetTextAlign(22);
   Latex.SetTextAngle(0);
   Latex.DrawLatex(PadX0 + PadDX * 1.5, PadY0 * 0.20, "Jet p_{T} (GeV)");

   Latex.SetTextFont(42);
   Latex.SetTextSize(0.030);
   Latex.SetTextAlign(22);
   Latex.SetTextAngle(90);
   Latex.DrawLatex(PadX0 * 0.20, PadY0 + PadDY, "R_{AA}");

   Latex.SetTextFont(62);
   Latex.SetTextSize(0.035);
   Latex.SetTextAlign(11);
   Latex.SetTextAngle(0);
   if(!NoData)                Latex.DrawLatex(PadX0, PadY0 + PadDY * 2 + PadY0 * 0.15, "CMS");
   if(DoTheory && NoData)     Latex.DrawLatex(PadX0, PadY0 + PadDY * 2 + PadY0 * 0.15, "Theory");
   if(DoJewel)                Latex.DrawLatex(PadX0, PadY0 + PadDY * 2 + PadY0 * 0.15, "Jewel");
   if(DoPyquen)               Latex.DrawLatex(PadX0, PadY0 + PadDY * 2 + PadY0 * 0.15, "Pyquen");
   if(DoIvan)                 Latex.DrawLatex(PadX0, PadY0 + PadDY * 2 + PadY0 * 0.15, "Li and Vitev");
   if(DoHybrid)               Latex.DrawLatex(PadX0, PadY0 + PadDY * 2 + PadY0 * 0.15, "Hybrid");

   Latex.SetTextFont(42);
   Latex.SetTextSize(0.030);
   Latex.SetTextAlign(31);
   Latex.SetTextAngle(0);
   if(!NoData) Latex.DrawLatex(PadX0 + PadDX * 3, PadY0 + PadDY * 2 + PadY0 * 0.15, "#sqrt{s_{NN}} = 5.02 TeV, PbPb 404 #mub^{-1}, pp 27.4 pb^{-1}");
   if (NoData) Latex.DrawLatex(PadX0 + PadDX * 3, PadY0 + PadDY * 2 + PadY0 * 0.15, "#sqrt{s_{NN}} = 5.02 TeV");

   TGraph GLine;
   GLine.SetPoint(0, XMin, 1);
   GLine.SetPoint(1, XMax, 1);
   GLine.SetLineStyle(kDashed);

   double Bins[] = {200, 250, 300, 400, 500, 1000};
   const int NRadii = 6;
   TH1D HC00RAA[NRadii];
   TH1D HC01RAA[NRadii];
   TH1D HC02RAA[NRadii];
   TH1D HC03RAA[NRadii];
   HC00RAA[0] = TH1D("HR02C00RAA", "", 5, Bins);   GetRAA(HC00RAA[0], File, R02, C0, false);
   HC01RAA[0] = TH1D("HR02C01RAA", "", 5, Bins);   GetRAA(HC01RAA[0], File, R02, C1, false);
   HC02RAA[0] = TH1D("HR02C02RAA", "", 5, Bins);   GetRAA(HC02RAA[0], File, R02, C2, false);
   HC03RAA[0] = TH1D("HR02C03RAA", "", 5, Bins);   GetRAA(HC03RAA[0], File, R02, C3, false);
   HC00RAA[1] = TH1D("HR03C00RAA", "", 5, Bins);   GetRAA(HC00RAA[1], File, R03, C0, false);
   HC01RAA[1] = TH1D("HR03C01RAA", "", 5, Bins);   GetRAA(HC01RAA[1], File, R03, C1, false);
   HC02RAA[1] = TH1D("HR03C02RAA", "", 5, Bins);   GetRAA(HC02RAA[1], File, R03, C2, false);
   HC03RAA[1] = TH1D("HR03C03RAA", "", 5, Bins);   GetRAA(HC03RAA[1], File, R03, C3, false);
   HC00RAA[2] = TH1D("HR04C00RAA", "", 5, Bins);   GetRAA(HC00RAA[2], File, R04, C0, false);
   HC01RAA[2] = TH1D("HR04C01RAA", "", 5, Bins);   GetRAA(HC01RAA[2], File, R04, C1, false);
   HC02RAA[2] = TH1D("HR04C02RAA", "", 5, Bins);   GetRAA(HC02RAA[2], File, R04, C2, false);
   HC03RAA[2] = TH1D("HR04C03RAA", "", 5, Bins);   GetRAA(HC03RAA[2], File, R04, C3, false);
   HC00RAA[3] = TH1D("HR06C00RAA", "", 5, Bins);   GetRAA(HC00RAA[3], File, R06, C0, false);
   HC01RAA[3] = TH1D("HR06C01RAA", "", 5, Bins);   GetRAA(HC01RAA[3], File, R06, C1, false);
   HC02RAA[3] = TH1D("HR06C02RAA", "", 5, Bins);   GetRAA(HC02RAA[3], File, R06, C2, false);
   HC03RAA[3] = TH1D("HR06C03RAA", "", 5, Bins);   GetRAA(HC03RAA[3], File, R06, C3, false);
   HC00RAA[4] = TH1D("HR08C00RAA", "", 5, Bins);   GetRAA(HC00RAA[4], File, R08, C0, false);
   HC01RAA[4] = TH1D("HR08C01RAA", "", 5, Bins);   GetRAA(HC01RAA[4], File, R08, C1, false);
   HC02RAA[4] = TH1D("HR08C02RAA", "", 5, Bins);   GetRAA(HC02RAA[4], File, R08, C2, false);
   HC03RAA[4] = TH1D("HR08C03RAA", "", 5, Bins);   GetRAA(HC03RAA[4], File, R08, C3, false);
   HC00RAA[5] = TH1D("HR10C00RAA", "", 5, Bins);   GetRAA(HC00RAA[5], File, R10, C0, false);
   HC01RAA[5] = TH1D("HR10C01RAA", "", 5, Bins);   GetRAA(HC01RAA[5], File, R10, C1, false);
   HC02RAA[5] = TH1D("HR10C02RAA", "", 5, Bins);   GetRAA(HC02RAA[5], File, R10, C2, false);
   HC03RAA[5] = TH1D("HR10C03RAA", "", 5, Bins);   GetRAA(HC03RAA[5], File, R10, C3, false);
   
   TH1D HC00RAASys[NRadii];
   TH1D HC01RAASys[NRadii];
   TH1D HC02RAASys[NRadii];
   TH1D HC03RAASys[NRadii];
   HC00RAASys[0] = TH1D("HR02C00RAASys", "", 5, Bins);   GetRAA(HC00RAASys[0], File, R02, C0, true);
   HC01RAASys[0] = TH1D("HR02C01RAASys", "", 5, Bins);   GetRAA(HC01RAASys[0], File, R02, C1, true);
   HC02RAASys[0] = TH1D("HR02C02RAASys", "", 5, Bins);   GetRAA(HC02RAASys[0], File, R02, C2, true);
   HC03RAASys[0] = TH1D("HR02C03RAASys", "", 5, Bins);   GetRAA(HC03RAASys[0], File, R02, C3, true);
   HC00RAASys[1] = TH1D("HR03C00RAASys", "", 5, Bins);   GetRAA(HC00RAASys[1], File, R03, C0, true);
   HC01RAASys[1] = TH1D("HR03C01RAASys", "", 5, Bins);   GetRAA(HC01RAASys[1], File, R03, C1, true);
   HC02RAASys[1] = TH1D("HR03C02RAASys", "", 5, Bins);   GetRAA(HC02RAASys[1], File, R03, C2, true);
   HC03RAASys[1] = TH1D("HR03C03RAASys", "", 5, Bins);   GetRAA(HC03RAASys[1], File, R03, C3, true);
   HC00RAASys[2] = TH1D("HR04C00RAASys", "", 5, Bins);   GetRAA(HC00RAASys[2], File, R04, C0, true);
   HC01RAASys[2] = TH1D("HR04C01RAASys", "", 5, Bins);   GetRAA(HC01RAASys[2], File, R04, C1, true);
   HC02RAASys[2] = TH1D("HR04C02RAASys", "", 5, Bins);   GetRAA(HC02RAASys[2], File, R04, C2, true);
   HC03RAASys[2] = TH1D("HR04C03RAASys", "", 5, Bins);   GetRAA(HC03RAASys[2], File, R04, C3, true);
   HC00RAASys[3] = TH1D("HR06C00RAASys", "", 5, Bins);   GetRAA(HC00RAASys[3], File, R06, C0, true);
   HC01RAASys[3] = TH1D("HR06C01RAASys", "", 5, Bins);   GetRAA(HC01RAASys[3], File, R06, C1, true);
   HC02RAASys[3] = TH1D("HR06C02RAASys", "", 5, Bins);   GetRAA(HC02RAASys[3], File, R06, C2, true);
   HC03RAASys[3] = TH1D("HR06C03RAASys", "", 5, Bins);   GetRAA(HC03RAASys[3], File, R06, C3, true);
   HC00RAASys[4] = TH1D("HR08C00RAASys", "", 5, Bins);   GetRAA(HC00RAASys[4], File, R08, C0, true);
   HC01RAASys[4] = TH1D("HR08C01RAASys", "", 5, Bins);   GetRAA(HC01RAASys[4], File, R08, C1, true);
   HC02RAASys[4] = TH1D("HR08C02RAASys", "", 5, Bins);   GetRAA(HC02RAASys[4], File, R08, C2, true);
   HC03RAASys[4] = TH1D("HR08C03RAASys", "", 5, Bins);   GetRAA(HC03RAASys[4], File, R08, C3, true);
   HC00RAASys[5] = TH1D("HR10C00RAASys", "", 5, Bins);   GetRAA(HC00RAASys[5], File, R10, C0, true);
   HC01RAASys[5] = TH1D("HR10C01RAASys", "", 5, Bins);   GetRAA(HC01RAASys[5], File, R10, C1, true);
   HC02RAASys[5] = TH1D("HR10C02RAASys", "", 5, Bins);   GetRAA(HC02RAASys[5], File, R10, C2, true);
   HC03RAASys[5] = TH1D("HR10C03RAASys", "", 5, Bins);   GetRAA(HC03RAASys[5], File, R10, C3, true);

   vector<TGraphAsymmErrors> Felix = GetTheoryGraphs("Theory/Felix/FelixRAA.txt", kBlack);
   vector<TGraphAsymmErrors> DanielWake = GetTheoryGraphs("Theory/Daniel/ForCMS/RAA_vs_R/Daniel_wake.txt", kMagenta);
   vector<TGraphAsymmErrors> DanielOnlyPos = GetTheoryGraphs("Theory/Daniel/ForCMS/RAA_vs_R/Daniel_onlypos.txt", kPink - 2);
   vector<TGraphAsymmErrors> DanielNoWake = GetTheoryGraphs("Theory/Daniel/ForCMS/RAA_vs_R/Daniel_nowake.txt", kOrange);
   vector<TGraphAsymmErrors> KorinnaC00 = GetTheoryGraphs("Theory/Korinna/JewelData_PbPb_0to10_RAA.txt", kOrange - 3);
   vector<TGraphAsymmErrors> KorinnaC01 = GetTheoryGraphs("Theory/Korinna/JewelData_PbPb_10to30_RAA.txt", kRed - 7);
   vector<TGraphAsymmErrors> KorinnaC02 = GetTheoryGraphs("Theory/Korinna/JewelData_PbPb_30to50_RAA.txt", kPink - 9);
   vector<TGraphAsymmErrors> KorinnaC03 = GetTheoryGraphs("Theory/Korinna/JewelData_PbPb_50to90_RAA.txt", kPink - 6);
   vector<TGraphAsymmErrors> KorinnaNoRecoilC00 = GetTheoryGraphs("Theory/Korinna/JewelData_PbPbNoRecoil_0to10_RAA.txt", kMagenta + 2);
   vector<TGraphAsymmErrors> KorinnaNoRecoilC01 = GetTheoryGraphs("Theory/Korinna/JewelData_PbPbNoRecoil_10to30_RAA.txt", kMagenta);
   vector<TGraphAsymmErrors> KorinnaNoRecoilC02 = GetTheoryGraphs("Theory/Korinna/JewelData_PbPbNoRecoil_30to50_RAA.txt", kViolet + 1);
   vector<TGraphAsymmErrors> KorinnaNoRecoilC03 = GetTheoryGraphs("Theory/Korinna/JewelData_PbPbNoRecoil_50to90_RAA.txt", kBlue - 5);
//   vector<TGraphAsymmErrors> PyquenB0 = GetTheoryGraphs("Theory/Pyquen/PyquenData_PbPb_RAA.txt", kBlue);
//   vector<TGraphAsymmErrors> PyquenWideRadB0 = GetTheoryGraphs("Theory/Pyquen/PyquenData_PbPbWide_RAA.txt", kViolet + 5);
   vector<TGraphAsymmErrors> PyquenC00 = GetTheoryGraphs("Theory/Pyquen/PyquenData_PbPb_0to10_RAA.txt", kTeal + 5);
   vector<TGraphAsymmErrors> PyquenWideRadC00 = GetTheoryGraphs("Theory/Pyquen/PyquenData_PbPbWide_0to10_RAA.txt", kTeal + 3);
   vector<TGraphAsymmErrors> IvanC00 = GetTheoryGraphs("Theory/Ivan/RAA_Centrality-0-10.txt", kBlue); // Only R = 02, 04, 08
   vector<TGraphAsymmErrors> IvanC01 = GetTheoryGraphs("Theory/Ivan/RAA_Centrality-10-30.txt", kGreen + 1);// Only R = 02, 04, 08
   vector<TGraphAsymmErrors> IvanC02 = GetTheoryGraphs("Theory/Ivan/RAA_Centrality-30-50.txt", kGreen + 3);// Only R = 02, 04, 08
   vector<TGraphAsymmErrors> SCET = GetTheoryGraphs("Theory/SCET/SCET_RAA.txt", kCyan + 1);// Only R = 02, 04, 06, 08, 10
   

   TLegend Legend11(0.1, 0.08, 0.5, 0.28);
   Legend11.SetTextFont(42);
   Legend11.SetTextSize(0.07);
   Legend11.SetFillStyle(0);
   Legend11.SetBorderSize(0);
   Legend11.AddEntry(&HC00RAASys[0], "0-10%", "plf");
   Legend11.AddEntry(&HC01RAASys[0], "10-30%", "plf");

   TLegend Legend12(0.45, 0.08, 0.85, 0.28);
   Legend12.SetTextFont(42);
   Legend12.SetTextSize(0.07);
   Legend12.SetFillStyle(0);
   Legend12.SetBorderSize(0);
   Legend12.AddEntry(&HC00RAA[0], "T_{AA}", "f");
   Legend12.AddEntry(&HC01RAA[0], "", "f");
   
   TLegend Legend21(0.1, 0.08, 0.5, 0.28);
   Legend21.SetTextFont(42);
   Legend21.SetTextSize(0.07);
   Legend21.SetFillStyle(0);
   Legend21.SetBorderSize(0);
   Legend21.AddEntry(&HC02RAASys[0], "30-50%", "plf");
   Legend21.AddEntry(&HC03RAASys[0], "50-90%", "plf");

   TLegend Legend22(0.45, 0.08, 0.85, 0.28);
   Legend22.SetTextFont(42);
   Legend22.SetTextSize(0.07);
   Legend22.SetFillStyle(0);
   Legend22.SetBorderSize(0);
   Legend22.AddEntry(&HC02RAA[0], "T_{AA}", "f");
   Legend22.AddEntry(&HC03RAA[0], "", "f");
   
   TH1D HLumi("HLumi", "", 1, 0, 1);
   HLumi.SetFillColor(Color[4]);
   TLegend Legend3(0.1, 0.08, 0.5, 0.28);
   Legend3.SetTextFont(42);
   Legend3.SetTextSize(0.07);
   Legend3.SetFillStyle(0);
   Legend3.SetBorderSize(0);
   Legend3.AddEntry(&HLumi, "Lumi", "f");
   Legend3.AddEntry("", "", "");

   TLegend LegendT1(0.07, 0.03, 0.48, 0.25);
   LegendT1.SetTextFont(42);
   LegendT1.SetTextSize(0.06);
   LegendT1.SetFillStyle(0);
   LegendT1.SetBorderSize(0);
   LegendT1.AddEntry(&HC00RAASys[0], "CMS 0-10%", "plf");
   LegendT1.AddEntry(&IvanC00[0], "Li and Vitev", "f");
   LegendT1.AddEntry(&Felix[0], "Factorization", "f");
   
   TLegend LegendT2(0.55, 0.05, 1.0, 0.23);
   LegendT2.SetTextFont(42);
   LegendT2.SetTextSize(0.06);
   LegendT2.SetFillStyle(0);
   LegendT2.SetBorderSize(0);
   LegendT2.AddEntry(&HC00RAA[0], "T_{AA}", "f");
   LegendT2.AddEntry(&HLumi, "Lumi", "f");

   TLegend LegendT3(0.07, 0.03, 0.48, 0.17);
   LegendT3.SetTextFont(42);
   LegendT3.SetTextSize(0.06);
   LegendT3.SetFillStyle(0);
   LegendT3.SetBorderSize(0);
   LegendT3.AddEntry(&PyquenC00[0], "Pyquen", "f");
   LegendT3.AddEntry(&PyquenWideRadC00[0], "Pyquen w/ wide rad.", "f");
   
   TLegend LegendT4(0.50, 0.03, 0.91, 0.17);
   LegendT4.SetTextFont(42);
   LegendT4.SetTextSize(0.06);
   LegendT4.SetFillStyle(0);
   LegendT4.SetBorderSize(0);
   LegendT4.AddEntry(&KorinnaC00[0], "Jewel", "f");
   LegendT4.AddEntry(&KorinnaNoRecoilC00[0], "Jewel w/o recoil", "f");
   
   TLegend LegendT5(0.07, 0.03, 0.48, 0.25);
   LegendT5.SetTextFont(42);
   LegendT5.SetTextSize(0.06);
   LegendT5.SetFillStyle(0);
   LegendT5.SetBorderSize(0);
   LegendT5.AddEntry(&DanielWake[0], "Hybrid w/ wake", "f");
   LegendT5.AddEntry(&DanielNoWake[0], "Hybrid w/o wake", "f");
   LegendT5.AddEntry(&DanielOnlyPos[0], "Hybrid w/ partial response", "f");
   
   TLegend LegendT6(0.07, 0.03, 0.48, 0.10);
   LegendT6.SetTextFont(42);
   LegendT6.SetTextSize(0.06);
   LegendT6.SetFillStyle(0);
   LegendT6.SetBorderSize(0);
   LegendT6.AddEntry(&SCET[0], "SCET w/o coll. E-loss", "f");

   TLegend LegendTO1(0.55, 0.03, 0.95, 0.17);
   LegendTO1.SetTextFont(42);
   LegendTO1.SetTextSize(0.06);
   LegendTO1.SetFillStyle(0);
   LegendTO1.SetBorderSize(0);
   LegendTO1.AddEntry(&IvanC00[0], "Li and Vitev", "f");
   LegendTO1.AddEntry(&Felix[0], "Factorization", "f");
   
   TLegend LegendTO2(0.07, 0.03, 0.47, 0.25);
   LegendTO2.SetTextFont(42);
   LegendTO2.SetTextSize(0.06);
   LegendTO2.SetFillStyle(0);
   LegendTO2.SetBorderSize(0);
   LegendTO2.AddEntry(&KorinnaC00[0], "Jewel", "f");
   LegendTO2.AddEntry(&KorinnaNoRecoilC00[0], "Jewel w/o recoil", "f");
   LegendTO2.AddEntry(&SCET[0], "SCET w/o coll. E-loss", "f");
   
   TLegend LegendTO3(0.07, 0.03, 0.47, 0.25);
   LegendTO3.SetTextFont(42);
   LegendTO3.SetTextSize(0.06);
   LegendTO3.SetFillStyle(0);
   LegendTO3.SetBorderSize(0);
   LegendTO3.AddEntry(&DanielWake[0], "Hybrid w/ wake", "f");
   LegendTO3.AddEntry(&DanielNoWake[0], "Hybrid w/o wake", "f");
   LegendTO3.AddEntry(&DanielOnlyPos[0], "Hybrid w/ partial response", "f");
   
   TLegend LegendTO4(0.07, 0.03, 0.47, 0.17);
   LegendTO4.SetTextFont(42);
   LegendTO4.SetTextSize(0.06);
   LegendTO4.SetFillStyle(0);
   LegendTO4.SetBorderSize(0);
   LegendTO4.AddEntry(&PyquenC00[0], "Pyquen", "f");
   LegendTO4.AddEntry(&PyquenWideRadC00[0], "Pyquen w/ wide rad.", "f");
   
   TLegend LegendH1(0.1, 0.03, 0.5, 0.30);
   LegendH1.SetTextFont(42);
   LegendH1.SetTextSize(0.07);
   LegendH1.SetFillStyle(0);
   LegendH1.SetBorderSize(0);
   LegendH1.AddEntry(&DanielWake[0], "Wake", "f");
   LegendH1.AddEntry(&DanielNoWake[0], "No wake", "f");
   LegendH1.AddEntry(&DanielOnlyPos[0], "Partial response", "f");
   
   TLegend LegendJ1(0.1, 0.03, 0.46, 0.17);
   LegendJ1.SetTextFont(42);
   LegendJ1.SetTextSize(0.07);
   LegendJ1.SetFillStyle(0);
   LegendJ1.SetBorderSize(0);
   LegendJ1.AddEntry(&KorinnaC00[0], "0-10%", "f");
   LegendJ1.AddEntry(&KorinnaC01[0], "10-30%", "f");
   
   TLegend LegendJ2(0.5, 0.03, 0.86, 0.17);
   LegendJ2.SetTextFont(42);
   LegendJ2.SetTextSize(0.07);
   LegendJ2.SetFillStyle(0);
   LegendJ2.SetBorderSize(0);
   LegendJ2.AddEntry(&KorinnaC02[0], "30-50%", "f");
   LegendJ2.AddEntry(&KorinnaC03[0], "50-90%", "f");
   
   TLegend LegendJ3(0.2, 0.03, 0.56, 0.17);
   LegendJ3.SetTextFont(42);
   LegendJ3.SetTextSize(0.07);
   LegendJ3.SetFillStyle(0);
   LegendJ3.SetBorderSize(0);
   LegendJ3.AddEntry(&KorinnaNoRecoilC00[0], "No recoil 0-10%", "f");
   LegendJ3.AddEntry(&KorinnaNoRecoilC01[0], "No recoil 10-30%", "f");
   
   TLegend LegendJ4(0.2, 0.03, 0.56, 0.17);
   LegendJ4.SetTextFont(42);
   LegendJ4.SetTextSize(0.07);
   LegendJ4.SetFillStyle(0);
   LegendJ4.SetBorderSize(0);
   LegendJ4.AddEntry(&KorinnaNoRecoilC02[0], "No recoil 30-50%", "f");
   LegendJ4.AddEntry(&KorinnaNoRecoilC03[0], "No recoil 50-90%", "f");
   
   TLegend LegendP1(0.1, 0.05, 0.5, 0.2);
   LegendP1.SetTextFont(42);
   LegendP1.SetTextSize(0.07);
   LegendP1.SetFillStyle(0);
   LegendP1.SetBorderSize(0);
   LegendP1.AddEntry(&PyquenC00[0], "Pyquen 0-10%", "f");
   LegendP1.AddEntry(&PyquenWideRadC00[0], "Pyquen w/ wide rad. 0-10%", "f");
   
   TLegend LegendI1(0.1, 0.05, 0.5, 0.3);
   LegendI1.SetTextFont(42);
   LegendI1.SetTextSize(0.07);
   LegendI1.SetFillStyle(0);
   LegendI1.SetBorderSize(0);
   LegendI1.AddEntry(&IvanC00[0], "Li and Vitev 0-10%", "f");
   LegendI1.AddEntry(&IvanC01[0], "Li and Vitev 10-30%", "f");
   LegendI1.AddEntry(&IvanC02[0], "Li and Vitev 30-50%", "f");
   
   TLegend LegendS1(0.1, 0.05, 0.5, 0.15);
   LegendS1.SetTextFont(42);
   LegendS1.SetTextSize(0.07);
   LegendS1.SetFillStyle(0);
   LegendS1.SetBorderSize(0);
   LegendS1.AddEntry(&SCET[0], "SCET w/o coll. E-loss", "f");
   
   Latex.SetTextFont(42);
   Latex.SetTextSize(0.07);
   Latex.SetTextAlign(31);
   Latex.SetTextAngle(0);
   
   for(int i = 0; i < NPad; i++)
   {
      P[i]->cd();
      HWorld.Draw("axis");
      if(DoTheory || DoHybrid) DanielWake[i].Draw("3");
//      if(DoTheory || DoHybrid) DanielWake[i].Draw("lX");
      if(DoTheory || DoHybrid) DanielNoWake[i].Draw("3");
//      if(DoTheory || DoHybrid) DanielNoWake[i].Draw("lX");
      if(DoTheory || DoHybrid) DanielOnlyPos[i].Draw("3");
//      if(DoTheory || DoHybrid) DanielOnlyPos[i].Draw("lX");
      if(DoTheory)             Felix[i].Draw("5");
//      if(DoTheory)             Felix[i].Draw("p");
      if(DoTheory || DoJewel)  KorinnaC00[i].Draw("5");
//      if(DoTheory || DoJewel)  KorinnaC00[i].Draw("p");
      if(DoJewel)              KorinnaC01[i].Draw("5");
//      if(DoJewel)              KorinnaC01[i].Draw("p");
      if(DoJewel)              KorinnaC02[i].Draw("5");
//      if(DoJewel)              KorinnaC02[i].Draw("p");
      if(DoJewel)              KorinnaC03[i].Draw("5");
//      if(DoJewel)              KorinnaC03[i].Draw("p");
      if(DoTheory || DoJewel)  KorinnaNoRecoilC00[i].Draw("5");
//      if(DoTheory || DoJewel)  KorinnaNoRecoilC00[i].Draw("p");
      if(DoJewel)              KorinnaNoRecoilC01[i].Draw("5");
//      if(DoJewel)              KorinnaNoRecoilC01[i].Draw("p");
      if(DoJewel)              KorinnaNoRecoilC02[i].Draw("5");
//      if(DoJewel)              KorinnaNoRecoilC02[i].Draw("p");
      if(DoJewel)              KorinnaNoRecoilC03[i].Draw("5");
//      if(DoJewel)              KorinnaNoRecoilC03[i].Draw("p");
      if(DoTheory || DoPyquen) PyquenC00[i].Draw("5");
//      if(DoTheory || DoPyquen) PyquenC00[i].Draw("p");
      if(DoTheory || DoPyquen) PyquenWideRadC00[i].Draw("5");
//      if(DoTheory || DoPyquen) PyquenWideRadC00[i].Draw("p");
      if(!NoData)              HC00RAASys[i].Draw("same e2");
      if(!DoTheory && !NoData) HC01RAASys[i].Draw("same e2");
      if(!DoTheory && !NoData) HC02RAASys[i].Draw("same e2");
      if(!DoTheory && !NoData) HC03RAASys[i].Draw("same e2");
      if(!NoData)              HC00RAA[i].Draw("same");
      if(!DoTheory && !NoData) HC01RAA[i].Draw("same");
      if(!DoTheory && !NoData) HC02RAA[i].Draw("same");
      if(!DoTheory && !NoData) HC03RAA[i].Draw("same");
      if(!NoData)              DrawLumiTAA(Lumi, TAA, XMin, XMax);
      GLine.Draw("l");
      
      if(i == 0 || i == 2 || i == 4) // Ivan only has R = 2, 4, 8
      {
         if(DoTheory || DoIvan)   IvanC00[i].Draw("3");
         if(DoIvan)               IvanC01[i].Draw("3");
         if(DoIvan)               IvanC02[i].Draw("3");
//         if(DoTheory || DoIvan)   IvanC00[i].Draw("lX");
//         if(DoIvan)               IvanC01[i].Draw("lX");
//         if(DoIvan)               IvanC02[i].Draw("lX");
      }
      
      if(i != 1) // SCET does not have R = 3
      {
         if(DoTheory || DoSCET)   SCET[i].Draw("3");
//         if(DoTheory || DoSCET)   SCET[i].Draw("lX");
      }
   }
   
   P[0]->cd();
   Latex.SetTextAlign(11);
   Latex.DrawLatex(0.08, 0.1, "anti-k_{T}, |#eta_{jet}| < 2");
   Latex.SetTextAlign(31);
   Latex.DrawLatex(0.92, 0.9, "R = 0.2");
   if(DoTheory && !NoData)  LegendT2.Draw();
   if(DoTheory && NoData)   LegendTO1.Draw();
   
   P[1]->cd();
   if(DoJewel)              LegendJ1.Draw();
   if(DoJewel)              LegendJ2.Draw();
   if(DoTheory && !NoData)  LegendT1.Draw();
   if(DoTheory && !NoData)  LegendT4.Draw();
   if(DoTheory && NoData)   LegendTO2.Draw();
   if(DoHybrid)             LegendH1.Draw();
   Latex.SetTextAlign(31);
   Latex.DrawLatex(0.92, 0.9, "R = 0.3");
   
   P[2]->cd();
   if(DoTheory && NoData)   LegendTO3.Draw();
   if(DoJewel)              LegendJ3.Draw();
   if(DoIvan)               LegendI1.Draw();
   if(DoSCET)               LegendS1.Draw();
   if(DoTheory && !NoData)  LegendT5.Draw();
   Latex.SetTextAlign(31);
   Latex.DrawLatex(0.92, 0.9, "R = 0.4");
   
   P[3]->cd();
   if(!DoTheory && !NoData) Legend11.Draw();
   if(!DoTheory && !NoData) Legend12.Draw();
   if(DoJewel)              LegendJ4.Draw();
   if(DoPyquen)             LegendP1.Draw();
   if(DoTheory && NoData)   LegendTO4.Draw();
   if(DoTheory && !NoData)  LegendT3.Draw();
   Latex.SetTextAlign(31);
   Latex.DrawLatex(0.92, 0.9, "R = 0.6");
   
   P[4]->cd();
   if(!DoTheory && !NoData) Legend21.Draw();
   if(!DoTheory && !NoData) Legend22.Draw();
   if(DoTheory && !NoData)  LegendT6.Draw();
   Latex.SetTextAlign(31);
   Latex.DrawLatex(0.92, 0.9, "R = 0.8");
   
   P[5]->cd();
   if(!DoTheory && !NoData) Legend3.Draw();
   Latex.SetTextAlign(31);
   Latex.DrawLatex(0.92, 0.9, "R = 1.0");

   string OutputBase = "";
   if(!NoData)    OutputBase = "Data";
   if(DoTheory)   OutputBase += "Theory";
   if(DoJewel)    OutputBase = "Jewel";
   if(DoPyquen)   OutputBase = "Pyquen";
   if(DoHybrid)   OutputBase = "Hybrid";
   if(DoIvan)     OutputBase = "Ivan";
   if(DoSCET)     OutputBase = "SCET";
   OutputBase += "RAA";

   Canvas.SaveAs((OutputBase + ".pdf").c_str());
   Canvas.SaveAs((OutputBase + ".png").c_str());
   Canvas.SaveAs((OutputBase + ".C").c_str());
   
   for(int i = 0; i < NPad; i++)
   {
      delete P[i];
   }

   return 0;
}

void SetPad(TPad &P)
{
   P.SetLogx();
   P.SetLeftMargin(0);
   P.SetTopMargin(0);
   P.SetRightMargin(0);
   P.SetBottomMargin(0);
   P.SetTickx();
   P.SetTicky();
   P.Draw();
}

void SetAxis(TGaxis &A)
{
   A.SetLabelFont(42);
   A.SetLabelSize(0.030);
   A.SetMaxDigits(6);
   A.SetMoreLogLabels();
   A.SetNoExponent();
   A.Draw();
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

   int Style[] = {20, 20, 20, 20};

   if(Color[C] == -1)
   {
      Color[0] = TColor::GetFreeColorIndex();
//      new TColor(Color[0], 0.9648, 0.2969, 0.2344);   // E74C3C
      new TColor(Color[0], 1, 0, 0);
      Color[1] = TColor::GetFreeColorIndex();
      new TColor(Color[1], 0.2031, 0.5938, 0.8555);   // 3498DB
      Color[2] = TColor::GetFreeColorIndex();
      new TColor(Color[2], 0.9414, 0.7656, 0.0586);   // F1C40F
      Color[3] = TColor::GetFreeColorIndex();
      new TColor(Color[3], 0.1797, 0.7969, 0.4414);   // 2ECC71
      Color[4] = TColor::GetFreeColorIndex();
      new TColor(Color[4], 0.5, 0.5, 0.5);
   }

   H.SetMarkerStyle(Style[C]);
   H.SetMarkerColor(Color[C]);
   H.SetLineColor(Color[C]);
   H.SetFillColor(Color[C]);
   if(Sys == true)
      H.SetFillColorAlpha(Color[C], 0.25);
//   H.SetMarkerSize(1.8);
   H.SetMarkerSize(2.2);
   H.SetLineWidth(2.0);

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

void DrawLumiTAA(double Lumi, double TAA[4], double XMin, double XMax)
{
   double Offset = 0.06;
   double Size = 0.08;
   double Grid = 0.10;

   TBox Box;

   double X1, X2;
   double x1, x2;

   Box.SetFillColor(Color[4]);
   x1 = Offset - Size * 0.5 + Grid * 0;
   x2 = Offset + Size * 0.5 + Grid * 0;
   X1 = exp(x1 * (log(XMax) - log(XMin)) + log(XMin));
   X2 = exp(x2 * (log(XMax) - log(XMin)) + log(XMin));
   Box.DrawBox(X1, 1 - Lumi, X2, 1 + Lumi);

   for(int i = 0; i < 4; i++)
   {
      if(TAA[i] < 0)
         continue;

      Box.SetFillColor(Color[i]);
      x1 = Offset - Size * 0.5 + Grid * (i + 1);
      x2 = Offset + Size * 0.5 + Grid * (i + 1);
      X1 = exp(x1 * (log(XMax) - log(XMin)) + log(XMin));
      X2 = exp(x2 * (log(XMax) - log(XMin)) + log(XMin));
      Box.DrawBox(X1, 1 - TAA[i], X2, 1 + TAA[i]);
   }
}

vector<TGraphAsymmErrors> GetTheoryGraphs(string TheoryFileName, int BaseColor)
{
   vector<TGraphAsymmErrors> Result(6);   // 0.2, 0.3, 0.4, 0.6, 0.8, 1.0

   for(int i = 0; i < 6; i++)
   {
      Result[i].SetLineColor(BaseColor);
      Result[i].SetFillColorAlpha(BaseColor, 0.25);
      Result[i].SetMarkerStyle(20);
      Result[i].SetMarkerColor(BaseColor);
   }

   ifstream in(TheoryFileName);

   int Count = 0;

   while(in)
   {
      char ch[10239];
      ch[0] = '\0';
      in.getline(ch, 10239, '\n');
      if(ch[0] == '\0')
         continue;

      stringstream str(ch);

      double PTMin = 0, PTMax = 0;
      str >> PTMin >> PTMax;

      if(PTMin < 1 || PTMax < 1)
         continue;

      double X = (PTMin + PTMax) / 2;
      double DX = fabs(PTMax - PTMin) / 2;

      for(int i = 0; i < 6; i++)
      {
         double R, RMin, RMax;
         str >> R >> RMax >> RMin;
         Result[i].SetPoint(Count, X, R);
         Result[i].SetPointError(Count, DX, DX, R - RMin, RMax - R);
      }

      Count = Count + 1;
   }

   in.close();

   return Result;
}

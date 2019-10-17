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
void SetAxis(TGaxis &A);
void GetRRAA(TGraphAsymmErrors &H, string FileName, int R, int C, bool Sys = false);
vector<TGraphAsymmErrors> GetTheory(string TheoryFileName, int BaseColor);

int main(int argc, char *argv[])
{
   CommandLine CL(argc, argv);

   string TheoryType = CL.Get("TheoryType", ""); // "", "All", "NoData", "Jewel", "Pyquen", "Hybrid"
   bool DoTheory = (TheoryType == "All") || (TheoryType == "NoData");
   bool DoJewel = (TheoryType == "Jewel");
   bool DoPyquen = (TheoryType == "Pyquen");
   bool DoHybrid = (TheoryType == "Hybrid");
   bool NoData = DoJewel || DoPyquen || DoHybrid || (TheoryType == "NoData");
   
   string Centrality = Cl.Get("Centrality", ""); // "", "0-10", "10-30", "30-50", "50-90"

   // Preamble: sizing
   int PanelSize = 500;
   int PaddingWidth = 100;
   int PaddingHeight = 100;
   int CanvasWidth = PanelSize * 3 + PaddingWidth * 2;
   int CanvasHeight = PanelSize * 2 + PaddingHeight * 2;

   double PadX0 = (double)PaddingWidth / CanvasWidth;
   double PadY0 = (double)PaddingHeight / CanvasHeight;
   double PadDX = (double)PanelSize / CanvasWidth;
   double PadDY = (double)PanelSize / CanvasHeight;

   double XMin = 0.15;
   double XMax = 1.10;
   // double YMin = 0.501;
   // double YMax = 1.5;
   double YMin = 0.701;
   double YMax = 1.999;
   
   if(DoJewel || DoTheory)    YMin = 0.201;
   if(DoJewel || DoTheory)    YMax = 1.999;

   string File = "RRAA_Smooth_RedStat.root";

   // double Lumi = 0.023;
   // double TAA[4] = {0.100719, 0.051233, 0.025341, 0.018646};

   // Start declaring canvas, pad, axis, and axis title (as TLatex)
   TCanvas Canvas("Canvas", "", CanvasWidth, CanvasHeight);

   TH2D HWorld("HWorld", ";;", 100, XMin, XMax, 100, YMin, YMax);
   HWorld.SetStats(0);
   HWorld.GetXaxis()->SetNdivisions(505);
   HWorld.GetYaxis()->SetNdivisions(505);

   TPad P1("P1", "", PadX0 + PadDX * 0, PadY0 + PadDY * 1, PadX0 + PadDX * 1, PadY0 + PadDY * 2, 0);
   TPad P2("P2", "", PadX0 + PadDX * 1, PadY0 + PadDY * 1, PadX0 + PadDX * 2, PadY0 + PadDY * 2, 0);
   TPad P3("P3", "", PadX0 + PadDX * 2, PadY0 + PadDY * 1, PadX0 + PadDX * 3, PadY0 + PadDY * 2, 0);
   TPad P4("P4", "", PadX0 + PadDX * 0, PadY0 + PadDY * 0, PadX0 + PadDX * 1, PadY0 + PadDY * 1, 0);
   TPad P5("P5", "", PadX0 + PadDX * 1, PadY0 + PadDY * 0, PadX0 + PadDX * 2, PadY0 + PadDY * 1, 0);
   TPad P6("P6", "", PadX0 + PadDX * 2, PadY0 + PadDY * 0, PadX0 + PadDX * 3, PadY0 + PadDY * 1, 0);
   
   SetPad(P1);
   SetPad(P2);
   SetPad(P3);
   SetPad(P4);
   SetPad(P5);
   SetPad(P6);
  
   Canvas.cd();

   TGaxis AxisX1(PadX0 + PadDX * 0, PadY0 + PadDY * 0, PadX0 + PadDX * 1, PadY0 + PadDY * 0, XMin, XMax, 505, "");
   TGaxis AxisX2(PadX0 + PadDX * 1, PadY0 + PadDY * 0, PadX0 + PadDX * 2, PadY0 + PadDY * 0, XMin, XMax, 505, "");
   TGaxis AxisX3(PadX0 + PadDX * 2, PadY0 + PadDY * 0, PadX0 + PadDX * 3, PadY0 + PadDY * 0, XMin, XMax, 505, "");
   TGaxis AxisY1(PadX0 + PadDX * 0, PadY0 + PadDY * 0, PadX0 + PadDX * 0, PadY0 + PadDY * 1, YMin, YMax, 505, "");
   TGaxis AxisY2(PadX0 + PadDX * 0, PadY0 + PadDY * 1, PadX0 + PadDX * 0, PadY0 + PadDY * 2, YMin, YMax, 505, "");

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
   Latex.DrawLatex(PadX0 + PadDX * 1.5, PadY0 * 0.20, "Jet R");

   Latex.SetTextFont(42);
   Latex.SetTextSize(0.030);
   Latex.SetTextAlign(22);
   Latex.SetTextAngle(90);
   Latex.DrawLatex(PadX0 * 0.20, PadY0 + PadDY, "R_{AA}^{R} / R_{AA}^{R = 0.2}");

   Latex.SetTextFont(62);
   Latex.SetTextSize(0.035);
   Latex.SetTextAlign(11);
   Latex.SetTextAngle(0);
   if(!NoData)                Latex.DrawLatex(PadX0, PadY0 + PadDY * 2 + PadY0 * 0.15, "CMS");
   if(DoTheory && NoData)     Latex.DrawLatex(PadX0, PadY0 + PadDY * 2 + PadY0 * 0.15, "Theory");
   if(DoJewel)                Latex.DrawLatex(PadX0, PadY0 + PadDY * 2 + PadY0 * 0.15, "Jewel");
   if(DoPyquen)               Latex.DrawLatex(PadX0, PadY0 + PadDY * 2 + PadY0 * 0.15, "Pyquen");
   if(DoHybrid)               Latex.DrawLatex(PadX0, PadY0 + PadDY * 2 + PadY0 * 0.15, "Hybrid");

   Latex.SetTextFont(42);
   Latex.SetTextSize(0.030);
   Latex.SetTextAlign(31);
   Latex.SetTextAngle(0);
   Latex.DrawLatex(PadX0 + PadDX * 3, PadY0 + PadDY * 2 + PadY0 * 0.15, "#sqrt{s_{NN}} = 5.02 TeV");

   TGraph GLine;
   GLine.SetPoint(0, XMin, 1);
   GLine.SetPoint(1, XMax, 1);
   GLine.SetLineStyle(kDashed);

   TGraphAsymmErrors HP0C0RRAA;   GetRRAA(HP0C0RRAA, File, PT0, C0, false);
   TGraphAsymmErrors HP0C1RRAA;   GetRRAA(HP0C1RRAA, File, PT0, C1, false);
   TGraphAsymmErrors HP0C2RRAA;   GetRRAA(HP0C2RRAA, File, PT0, C2, false);
   TGraphAsymmErrors HP0C3RRAA;   GetRRAA(HP0C3RRAA, File, PT0, C3, false);
   TGraphAsymmErrors HP1C0RRAA;   GetRRAA(HP1C0RRAA, File, PT1, C0, false);
   TGraphAsymmErrors HP1C1RRAA;   GetRRAA(HP1C1RRAA, File, PT1, C1, false);
   TGraphAsymmErrors HP1C2RRAA;   GetRRAA(HP1C2RRAA, File, PT1, C2, false);
   TGraphAsymmErrors HP1C3RRAA;   GetRRAA(HP1C3RRAA, File, PT1, C3, false);
   TGraphAsymmErrors HP2C0RRAA;   GetRRAA(HP2C0RRAA, File, PT2, C0, false);
   TGraphAsymmErrors HP2C1RRAA;   GetRRAA(HP2C1RRAA, File, PT2, C1, false);
   TGraphAsymmErrors HP2C2RRAA;   GetRRAA(HP2C2RRAA, File, PT2, C2, false);
   TGraphAsymmErrors HP2C3RRAA;   GetRRAA(HP2C3RRAA, File, PT2, C3, false);
   TGraphAsymmErrors HP3C0RRAA;   GetRRAA(HP3C0RRAA, File, PT3, C0, false);
   TGraphAsymmErrors HP3C1RRAA;   GetRRAA(HP3C1RRAA, File, PT3, C1, false);
   TGraphAsymmErrors HP3C2RRAA;   GetRRAA(HP3C2RRAA, File, PT3, C2, false);
   TGraphAsymmErrors HP3C3RRAA;   GetRRAA(HP3C3RRAA, File, PT3, C3, false);
   TGraphAsymmErrors HP4C0RRAA;   GetRRAA(HP4C0RRAA, File, PT4, C0, false);
   TGraphAsymmErrors HP4C1RRAA;   GetRRAA(HP4C1RRAA, File, PT4, C1, false);
   TGraphAsymmErrors HP4C2RRAA;   GetRRAA(HP4C2RRAA, File, PT4, C2, false);
   TGraphAsymmErrors HP4C3RRAA;   GetRRAA(HP4C3RRAA, File, PT4, C3, false);
   
   TGraphAsymmErrors HP0C0RRAASys;   GetRRAA(HP0C0RRAASys, File, PT0, C0, true);
   TGraphAsymmErrors HP0C1RRAASys;   GetRRAA(HP0C1RRAASys, File, PT0, C1, true);
   TGraphAsymmErrors HP0C2RRAASys;   GetRRAA(HP0C2RRAASys, File, PT0, C2, true);
   TGraphAsymmErrors HP0C3RRAASys;   GetRRAA(HP0C3RRAASys, File, PT0, C3, true);
   TGraphAsymmErrors HP1C0RRAASys;   GetRRAA(HP1C0RRAASys, File, PT1, C0, true);
   TGraphAsymmErrors HP1C1RRAASys;   GetRRAA(HP1C1RRAASys, File, PT1, C1, true);
   TGraphAsymmErrors HP1C2RRAASys;   GetRRAA(HP1C2RRAASys, File, PT1, C2, true);
   TGraphAsymmErrors HP1C3RRAASys;   GetRRAA(HP1C3RRAASys, File, PT1, C3, true);
   TGraphAsymmErrors HP2C0RRAASys;   GetRRAA(HP2C0RRAASys, File, PT2, C0, true);
   TGraphAsymmErrors HP2C1RRAASys;   GetRRAA(HP2C1RRAASys, File, PT2, C1, true);
   TGraphAsymmErrors HP2C2RRAASys;   GetRRAA(HP2C2RRAASys, File, PT2, C2, true);
   TGraphAsymmErrors HP2C3RRAASys;   GetRRAA(HP2C3RRAASys, File, PT2, C3, true);
   TGraphAsymmErrors HP3C0RRAASys;   GetRRAA(HP3C0RRAASys, File, PT3, C0, true);
   TGraphAsymmErrors HP3C1RRAASys;   GetRRAA(HP3C1RRAASys, File, PT3, C1, true);
   TGraphAsymmErrors HP3C2RRAASys;   GetRRAA(HP3C2RRAASys, File, PT3, C2, true);
   TGraphAsymmErrors HP3C3RRAASys;   GetRRAA(HP3C3RRAASys, File, PT3, C3, true);
   TGraphAsymmErrors HP4C0RRAASys;   GetRRAA(HP4C0RRAASys, File, PT4, C0, true);
   TGraphAsymmErrors HP4C1RRAASys;   GetRRAA(HP4C1RRAASys, File, PT4, C1, true);
   TGraphAsymmErrors HP4C2RRAASys;   GetRRAA(HP4C2RRAASys, File, PT4, C2, true);
   TGraphAsymmErrors HP4C3RRAASys;   GetRRAA(HP4C3RRAASys, File, PT4, C3, true);

   vector<TGraphAsymmErrors> Felix = GetTheory("Theory/Felix/FelixRRAA.txt", kBlack);
   vector<TGraphAsymmErrors> DanielWake = GetTheory("Theory/Daniel/ForCMS/r_RAA/Daniel_wake.txt", kMagenta);
   vector<TGraphAsymmErrors> DanielOnlyPos = GetTheory("Theory/Daniel/ForCMS/r_RAA/Daniel_onlypos.txt", kRed);
   vector<TGraphAsymmErrors> DanielNoWake = GetTheory("Theory/Daniel/ForCMS/r_RAA/Daniel_nowake.txt", kOrange);
   vector<TGraphAsymmErrors> KorinnaC00 = GetTheory("Theory/Korinna/JewelData_PbPb_0to10_RRAA.txt", kOrange - 3);
   vector<TGraphAsymmErrors> KorinnaC01 = GetTheory("Theory/Korinna/JewelData_PbPb_10to30_RRAA.txt", kRed - 7);
   vector<TGraphAsymmErrors> KorinnaC02 = GetTheory("Theory/Korinna/JewelData_PbPb_30to50_RRAA.txt", kPink - 9);
   vector<TGraphAsymmErrors> KorinnaC03 = GetTheory("Theory/Korinna/JewelData_PbPb_50to90_RRAA.txt", kPink - 6);
   vector<TGraphAsymmErrors> KorinnaNoRecoilC00 = GetTheory("Theory/Korinna/JewelData_PbPbNoRecoil_0to10_RRAA.txt", kMagenta + 2);
   vector<TGraphAsymmErrors> KorinnaNoRecoilC01 = GetTheory("Theory/Korinna/JewelData_PbPbNoRecoil_10to30_RRAA.txt", kMagenta);
   vector<TGraphAsymmErrors> KorinnaNoRecoilC02 = GetTheory("Theory/Korinna/JewelData_PbPbNoRecoil_30to50_RRAA.txt", kViolet + 1);
   vector<TGraphAsymmErrors> KorinnaNoRecoilC03 = GetTheory("Theory/Korinna/JewelData_PbPbNoRecoil_50to90_RRAA.txt", kBlue - 5);
   vector<TGraphAsymmErrors> PyquenB0 = GetTheory("Theory/Pyquen/PyquenData_PbPb_RRAA.txt", kBlue);
   vector<TGraphAsymmErrors> PyquenWideRadB0 = GetTheory("Theory/Pyquen/PyquenData_PbPbWide_RRAA.txt", kViolet + 5);
   vector<TGraphAsymmErrors> PyquenC00 = GetTheory("Theory/Pyquen/PyquenData_PbPb_0to10_RRAA.txt", kTeal + 5);
   vector<TGraphAsymmErrors> PyquenWideRadC00 = GetTheory("Theory/Pyquen/PyquenData_PbPbWide_0to10_RRAA.txt", kTeal + 3);
   
   TLegend Legend1(0.1, 0.3, 0.5, 0.7);
   Legend1.SetTextFont(42);
   Legend1.SetTextSize(0.07);
   Legend1.SetFillStyle(0);
   Legend1.SetBorderSize(0);
   Legend1.AddEntry(&HP0C0RRAASys, "0-10%", "plf");
   Legend1.AddEntry(&HP0C1RRAASys, "10-30%", "plf");
   Legend1.AddEntry(&HP0C2RRAASys, "30-50%", "plf");
   Legend1.AddEntry(&HP0C3RRAASys, "50-90%", "plf");
   
   TLegend LegendT1(0.05, 0.3, 0.5, 0.7);
   LegendT1.SetTextFont(42);
   LegendT1.SetTextSize(0.07);
   LegendT1.SetFillStyle(0);
   LegendT1.SetBorderSize(0);
   LegendT1.AddEntry(&HP0C0RRAASys, "0-10%", "plf");
   LegendT1.AddEntry(&Felix[0], "Factorization", "lf");
   LegendT1.AddEntry(&DanielWake[0], "Hybrid", "lf");
   LegendT1.AddEntry(&KorinnaC00[0], "Jewel", "lf");
   LegendT1.AddEntry(&KorinnaNoRecoilC00[0], "Jewel no recoil", "lf");
   LegendT1.AddEntry(&PyquenC00[0], "Pyquen", "lf");
   LegendT1.AddEntry(&PyquenWideRadC00[0], "Pyquen wide rad.", "lf");
   
   TLegend LegendTO1(0.1, 0.1, 0.9, 0.7);
   LegendTO1.SetTextFont(42);
   LegendTO1.SetTextSize(0.07);
   LegendTO1.SetFillStyle(0);
   LegendTO1.SetBorderSize(0);
   LegendTO1.AddEntry(&Felix[0], "Factorization", "lf");
   LegendTO1.AddEntry(&DanielWake[0], "Hybrid", "lf");
   LegendTO1.AddEntry(&KorinnaC00[0], "Jewel", "lf");
   LegendTO1.AddEntry(&KorinnaNoRecoilC00[0], "Jewel no recoil", "lf");
   LegendTO1.AddEntry(&PyquenC00[0], "Pyquen", "lf");
   LegendTO1.AddEntry(&PyquenWideRadC00[0], "Pyquen wide rad.", "lf");
   
   TLegend LegendJ1(0.05, 0.05, 0.9, 0.85);
   LegendJ1.SetTextFont(42);
   LegendJ1.SetTextSize(0.07);
   LegendJ1.SetFillStyle(0);
   LegendJ1.SetBorderSize(0);
   LegendJ1.AddEntry(&KorinnaC00[0], "0-10%", "lf");
   LegendJ1.AddEntry(&KorinnaC01[0], "10-30%", "lf");
   LegendJ1.AddEntry(&KorinnaC02[0], "30-50%", "lf");
   LegendJ1.AddEntry(&KorinnaC03[0], "50-90%", "lf");
   LegendJ1.AddEntry(&KorinnaNoRecoilC00[0], "No recoil 0-10%", "lf");
   LegendJ1.AddEntry(&KorinnaNoRecoilC01[0], "No recoil 10-30%", "lf");
   LegendJ1.AddEntry(&KorinnaNoRecoilC02[0], "No recoil 30-50%", "lf");
   LegendJ1.AddEntry(&KorinnaNoRecoilC03[0], "No recoil 50-90%", "lf");
   
   TLegend LegendP1(0.1, 0.3, 0.5, 0.7);
   LegendP1.SetTextFont(42);
   LegendP1.SetTextSize(0.07);
   LegendP1.SetFillStyle(0);
   LegendP1.SetBorderSize(0);
   LegendP1.AddEntry(&PyquenC00[0], "0-10%", "lf");
   LegendP1.AddEntry(&PyquenWideRadC00[0], "Wide rad. 0-10%", "lf");
   LegendP1.AddEntry(&PyquenB0[0], "b=0", "lf");
   LegendP1.AddEntry(&PyquenWideRadB0[0], "Wide rad. b=0", "lf");
   
   TLegend LegendH1(0.1, 0.3, 0.5, 0.6);
   LegendH1.SetTextFont(42);
   LegendH1.SetTextSize(0.07);
   LegendH1.SetFillStyle(0);
   LegendH1.SetBorderSize(0);
   LegendH1.AddEntry(&DanielWake[0], "Wake", "lf");
   LegendH1.AddEntry(&DanielNoWake[0], "No wake", "lf");
   LegendH1.AddEntry(&DanielOnlyPos[0], "Partial response", "lf");

   Latex.SetTextFont(42);
   Latex.SetTextSize(0.07);
   Latex.SetTextAlign(31);
   Latex.SetTextAngle(0);
   
   char* FelixStyle1 = "3";
   char*  FelixStyle2 = "lX";
   char*  HybridStyle1 = "3";
   char*  HybridStyle2 = "lX";
   char*  JewelStyle1 = "3";
   char*  JewelStyle2 = "lX";
   char*  PyquenStyle1 = "3";
   char*  PyquenStyle2 = "lX";
   
   P1.cd();
   HWorld.Draw("axis");
   if(DoTheory)             Felix[0].Draw(FelixStyle1);
   if(DoTheory)             Felix[0].Draw(FelixStyle2);
   if(DoTheory || DoHybrid) DanielWake[0].Draw(HybridStyle1);
   if(DoTheory || DoHybrid) DanielWake[0].Draw(HybridStyle2);
   if(DoHybrid)             DanielNoWake[0].Draw(HybridStyle1);
   if(DoHybrid)             DanielNoWake[0].Draw(HybridStyle2);
   if(DoHybrid)             DanielOnlyPos[0].Draw(HybridStyle1);
   if(DoHybrid)             DanielOnlyPos[0].Draw(HybridStyle2);
   if(DoTheory || DoJewel)  KorinnaC00[0].Draw(JewelStyle1);
   if(DoTheory || DoJewel)  KorinnaC00[0].Draw(JewelStyle2);
   if(DoJewel)              KorinnaC01[0].Draw(JewelStyle1);
   if(DoJewel)              KorinnaC01[0].Draw(JewelStyle2);
   if(DoJewel)              KorinnaC02[0].Draw(JewelStyle1);
   if(DoJewel)              KorinnaC02[0].Draw(JewelStyle2);
   if(DoJewel)              KorinnaC03[0].Draw(JewelStyle1);
   if(DoJewel)              KorinnaC03[0].Draw(JewelStyle2);
   if(DoTheory || DoJewel)  KorinnaNoRecoilC00[0].Draw(JewelStyle1);
   if(DoTheory || DoJewel)  KorinnaNoRecoilC00[0].Draw(JewelStyle2);
   if(DoJewel)              KorinnaNoRecoilC01[0].Draw(JewelStyle1);
   if(DoJewel)              KorinnaNoRecoilC01[0].Draw(JewelStyle2);
   if(DoJewel)              KorinnaNoRecoilC02[0].Draw(JewelStyle1);
   if(DoJewel)              KorinnaNoRecoilC02[0].Draw(JewelStyle2);
   if(DoJewel)              KorinnaNoRecoilC03[0].Draw(JewelStyle1);
   if(DoJewel)              KorinnaNoRecoilC03[0].Draw(JewelStyle2);
   if(DoTheory || DoPyquen) PyquenC00[0].Draw(PyquenStyle1);
   if(DoTheory || DoPyquen) PyquenC00[0].Draw(PyquenStyle2);
   if(DoTheory || DoPyquen) PyquenWideRadC00[0].Draw(PyquenStyle1);
   if(DoTheory || DoPyquen) PyquenWideRadC00[0].Draw(PyquenStyle2);
   if(DoPyquen)             PyquenB0[0].Draw(PyquenStyle1);
   if(DoPyquen)             PyquenB0[0].Draw(PyquenStyle2);
   if(DoPyquen)             PyquenWideRadB0[0].Draw(PyquenStyle1);
   if(DoPyquen)             PyquenWideRadB0[0].Draw(PyquenStyle2);
   if(!DoTheory && !NoData) HP0C0RRAASys.Draw("2");
   if(!DoTheory && !NoData) HP0C1RRAASys.Draw("2");
   if(!DoTheory && !NoData) HP0C2RRAASys.Draw("2");
   if(!DoTheory && !NoData) HP0C3RRAASys.Draw("2");
   if(!DoTheory && !NoData) HP0C0RRAA.Draw("p");
   if(!DoTheory && !NoData) HP0C1RRAA.Draw("p");
   if(!DoTheory && !NoData) HP0C2RRAA.Draw("p");
   if(!DoTheory && !NoData) HP0C3RRAA.Draw("p");
   GLine.Draw("l");
   Latex.SetTextAlign(11);
   Latex.DrawLatex(0.08, 0.9, "200 < p_{T} < 250 GeV");
   P2.cd();
   HWorld.Draw("axis");
   if(DoTheory)             Felix[1].Draw(FelixStyle1);
   if(DoTheory)             Felix[1].Draw(FelixStyle2);
   if(DoTheory || DoHybrid) DanielWake[1].Draw(HybridStyle1);
   if(DoTheory || DoHybrid) DanielWake[1].Draw(HybridStyle2);
   if(DoHybrid)             DanielNoWake[1].Draw(HybridStyle1);
   if(DoHybrid)             DanielNoWake[1].Draw(HybridStyle2);
   if(DoHybrid)             DanielOnlyPos[1].Draw(HybridStyle1);
   if(DoHybrid)             DanielOnlyPos[1].Draw(HybridStyle2);
   if(DoTheory || DoJewel)  KorinnaC00[1].Draw(JewelStyle1);
   if(DoTheory || DoJewel)  KorinnaC00[1].Draw(JewelStyle2);
   if(DoJewel)              KorinnaC01[1].Draw(JewelStyle1);
   if(DoJewel)              KorinnaC01[1].Draw(JewelStyle2);
   if(DoJewel)              KorinnaC02[1].Draw(JewelStyle1);
   if(DoJewel)              KorinnaC02[1].Draw(JewelStyle2);
   if(DoJewel)              KorinnaC03[1].Draw(JewelStyle1);
   if(DoJewel)              KorinnaC03[1].Draw(JewelStyle2);
   if(DoTheory || DoJewel)  KorinnaNoRecoilC00[1].Draw(JewelStyle1);
   if(DoTheory || DoJewel)  KorinnaNoRecoilC00[1].Draw(JewelStyle2);
   if(DoJewel)              KorinnaNoRecoilC01[1].Draw(JewelStyle1);
   if(DoJewel)              KorinnaNoRecoilC01[1].Draw(JewelStyle2);
   if(DoJewel)              KorinnaNoRecoilC02[1].Draw(JewelStyle1);
   if(DoJewel)              KorinnaNoRecoilC02[1].Draw(JewelStyle2);
   if(DoJewel)              KorinnaNoRecoilC03[1].Draw(JewelStyle1);
   if(DoJewel)              KorinnaNoRecoilC03[1].Draw(JewelStyle2);
   if(DoTheory || DoPyquen) PyquenC00[1].Draw(PyquenStyle1);
   if(DoTheory || DoPyquen) PyquenC00[1].Draw(PyquenStyle2);
   if(DoTheory || DoPyquen) PyquenWideRadC00[1].Draw(PyquenStyle1);
   if(DoTheory || DoPyquen) PyquenWideRadC00[1].Draw(PyquenStyle2);
   if(DoPyquen)             PyquenB0[1].Draw(PyquenStyle1);
   if(DoPyquen)             PyquenB0[1].Draw(PyquenStyle2);
   if(DoPyquen)             PyquenWideRadB0[1].Draw(PyquenStyle1);
   if(DoPyquen)             PyquenWideRadB0[1].Draw(PyquenStyle2);
   if(!DoTheory && !NoData) HP1C0RRAASys.Draw("2");
   if(!DoTheory && !NoData) HP1C1RRAASys.Draw("2");
   if(!DoTheory && !NoData) HP1C2RRAASys.Draw("2");
   if(!DoTheory && !NoData) HP1C3RRAASys.Draw("2");
   if(!DoTheory && !NoData) HP1C0RRAA.Draw("p");
   if(!DoTheory && !NoData) HP1C1RRAA.Draw("p");
   if(!DoTheory && !NoData) HP1C2RRAA.Draw("p");
   if(!DoTheory && !NoData) HP1C3RRAA.Draw("p");
   GLine.Draw("l");
   Latex.SetTextAlign(11);
   Latex.DrawLatex(0.08, 0.9, "250 < p_{T} < 300 GeV");
   P3.cd();
   HWorld.Draw("axis");
   if(DoTheory)             Felix[2].Draw(FelixStyle1);
   if(DoTheory)             Felix[2].Draw(FelixStyle2);
   if(DoTheory || DoHybrid) DanielWake[2].Draw(HybridStyle1);
   if(DoTheory || DoHybrid) DanielWake[2].Draw(HybridStyle2);
   if(DoHybrid)             DanielNoWake[2].Draw(HybridStyle1);
   if(DoHybrid)             DanielNoWake[2].Draw(HybridStyle2);
   if(DoHybrid)             DanielOnlyPos[2].Draw(HybridStyle1);
   if(DoHybrid)             DanielOnlyPos[2].Draw(HybridStyle2);
   if(DoTheory || DoJewel)  KorinnaC00[2].Draw(JewelStyle1);
   if(DoTheory || DoJewel)  KorinnaC00[2].Draw(JewelStyle2);
   if(DoJewel)              KorinnaC01[2].Draw(JewelStyle1);
   if(DoJewel)              KorinnaC01[2].Draw(JewelStyle2);
   if(DoJewel)              KorinnaC02[2].Draw(JewelStyle1);
   if(DoJewel)              KorinnaC02[2].Draw(JewelStyle2);
   if(DoJewel)              KorinnaC03[2].Draw(JewelStyle1);
   if(DoJewel)              KorinnaC03[2].Draw(JewelStyle2);
   if(DoTheory || DoJewel)  KorinnaNoRecoilC00[2].Draw(JewelStyle1);
   if(DoTheory || DoJewel)  KorinnaNoRecoilC00[2].Draw(JewelStyle2);
   if(DoJewel)              KorinnaNoRecoilC01[2].Draw(JewelStyle1);
   if(DoJewel)              KorinnaNoRecoilC01[2].Draw(JewelStyle2);
   if(DoJewel)              KorinnaNoRecoilC02[2].Draw(JewelStyle1);
   if(DoJewel)              KorinnaNoRecoilC02[2].Draw(JewelStyle2);
   if(DoJewel)              KorinnaNoRecoilC03[2].Draw(JewelStyle1);
   if(DoJewel)              KorinnaNoRecoilC03[2].Draw(JewelStyle2);
   if(DoTheory || DoPyquen) PyquenC00[2].Draw(PyquenStyle1);
   if(DoTheory || DoPyquen) PyquenC00[2].Draw(PyquenStyle2);
   if(DoTheory || DoPyquen) PyquenWideRadC00[2].Draw(PyquenStyle1);
   if(DoTheory || DoPyquen) PyquenWideRadC00[2].Draw(PyquenStyle2);
   if(DoPyquen)             PyquenB0[2].Draw(PyquenStyle1);
   if(DoPyquen)             PyquenB0[2].Draw(PyquenStyle2);
   if(DoPyquen)             PyquenWideRadB0[2].Draw(PyquenStyle1);
   if(DoPyquen)             PyquenWideRadB0[2].Draw(PyquenStyle2);
   if(!DoTheory && !NoData) HP2C0RRAASys.Draw("2");
   if(!DoTheory && !NoData) HP2C1RRAASys.Draw("2");
   if(!DoTheory && !NoData) HP2C2RRAASys.Draw("2");
   if(!DoTheory && !NoData) HP2C3RRAASys.Draw("2");
   if(!DoTheory && !NoData) HP2C0RRAA.Draw("p");
   if(!DoTheory && !NoData) HP2C1RRAA.Draw("p");
   if(!DoTheory && !NoData) HP2C2RRAA.Draw("p");
   if(!DoTheory && !NoData) HP2C3RRAA.Draw("p");
   GLine.Draw("l");
   Latex.SetTextAlign(11);
   Latex.DrawLatex(0.08, 0.9, "300 < p_{T} < 400 GeV");
   P4.cd();
   HWorld.Draw("axis");
   if(DoTheory)             Felix[3].Draw(FelixStyle1);
   if(DoTheory)             Felix[3].Draw(FelixStyle2);
   if(DoTheory || DoHybrid) DanielWake[3].Draw(HybridStyle1);
   if(DoTheory || DoHybrid) DanielWake[3].Draw(HybridStyle2);
   if(DoHybrid)             DanielNoWake[3].Draw(HybridStyle1);
   if(DoHybrid)             DanielNoWake[3].Draw(HybridStyle2);
   if(DoHybrid)             DanielOnlyPos[3].Draw(HybridStyle1);
   if(DoHybrid)             DanielOnlyPos[3].Draw(HybridStyle2);
   if(DoTheory || DoJewel)  KorinnaC00[3].Draw(JewelStyle1);
   if(DoTheory || DoJewel)  KorinnaC00[3].Draw(JewelStyle2);
   if(DoJewel)              KorinnaC01[3].Draw(JewelStyle1);
   if(DoJewel)              KorinnaC01[3].Draw(JewelStyle2);
   if(DoJewel)              KorinnaC02[3].Draw(JewelStyle1);
   if(DoJewel)              KorinnaC02[3].Draw(JewelStyle2);
   if(DoJewel)              KorinnaC03[3].Draw(JewelStyle1);
   if(DoJewel)              KorinnaC03[3].Draw(JewelStyle2);
   if(DoTheory || DoJewel)  KorinnaNoRecoilC00[3].Draw(JewelStyle1);
   if(DoTheory || DoJewel)  KorinnaNoRecoilC00[3].Draw(JewelStyle2);
   if(DoJewel)              KorinnaNoRecoilC01[3].Draw(JewelStyle1);
   if(DoJewel)              KorinnaNoRecoilC01[3].Draw(JewelStyle2);
   if(DoJewel)              KorinnaNoRecoilC02[3].Draw(JewelStyle1);
   if(DoJewel)              KorinnaNoRecoilC02[3].Draw(JewelStyle2);
   if(DoJewel)              KorinnaNoRecoilC03[3].Draw(JewelStyle1);
   if(DoJewel)              KorinnaNoRecoilC03[3].Draw(JewelStyle2);
   if(DoTheory || DoPyquen) PyquenC00[3].Draw(PyquenStyle1);
   if(DoTheory || DoPyquen) PyquenC00[3].Draw(PyquenStyle2);
   if(DoTheory || DoPyquen) PyquenWideRadC00[3].Draw(PyquenStyle1);
   if(DoTheory || DoPyquen) PyquenWideRadC00[3].Draw(PyquenStyle2);
   if(DoPyquen)             PyquenB0[3].Draw(PyquenStyle1);
   if(DoPyquen)             PyquenB0[3].Draw(PyquenStyle2);
   if(DoPyquen)             PyquenWideRadB0[3].Draw(PyquenStyle1);
   if(DoPyquen)             PyquenWideRadB0[3].Draw(PyquenStyle2);
   if(!DoTheory && !NoData) HP3C0RRAASys.Draw("2");
   if(!DoTheory && !NoData) HP3C1RRAASys.Draw("2");
   if(!DoTheory && !NoData) HP3C2RRAASys.Draw("2");
   if(!DoTheory && !NoData) HP3C3RRAASys.Draw("2");
   if(!DoTheory && !NoData) HP3C0RRAA.Draw("p");
   if(!DoTheory && !NoData) HP3C1RRAA.Draw("p");
   if(!DoTheory && !NoData) HP3C2RRAA.Draw("p");
   if(!DoTheory && !NoData) HP3C3RRAA.Draw("p");
   GLine.Draw("l");
   Latex.SetTextAlign(11);
   Latex.DrawLatex(0.08, 0.9, "400 < p_{T} < 500 GeV");
   P5.cd();
   HWorld.Draw("axis");
   if(DoTheory)             Felix[4].Draw(FelixStyle1);
   if(DoTheory)             Felix[4].Draw(FelixStyle2);
   if(DoTheory || DoHybrid) DanielWake[4].Draw(HybridStyle1);
   if(DoTheory || DoHybrid) DanielWake[4].Draw(HybridStyle2);
   if(DoHybrid)             DanielNoWake[4].Draw(HybridStyle1);
   if(DoHybrid)             DanielNoWake[4].Draw(HybridStyle2);
   if(DoHybrid)             DanielOnlyPos[4].Draw(HybridStyle1);
   if(DoHybrid)             DanielOnlyPos[4].Draw(HybridStyle2);
   if(DoTheory || DoJewel)  KorinnaC00[4].Draw(JewelStyle1);
   if(DoTheory || DoJewel)  KorinnaC00[4].Draw(JewelStyle2);
   if(DoJewel)              KorinnaC01[4].Draw(JewelStyle1);
   if(DoJewel)              KorinnaC01[4].Draw(JewelStyle2);
   if(DoJewel)              KorinnaC02[4].Draw(JewelStyle1);
   if(DoJewel)              KorinnaC02[4].Draw(JewelStyle2);
   if(DoJewel)              KorinnaC03[4].Draw(JewelStyle1);
   if(DoJewel)              KorinnaC03[4].Draw(JewelStyle2);
   if(DoTheory || DoJewel)  KorinnaNoRecoilC00[4].Draw(JewelStyle1);
   if(DoTheory || DoJewel)  KorinnaNoRecoilC00[4].Draw(JewelStyle2);
   if(DoJewel)              KorinnaNoRecoilC01[4].Draw(JewelStyle1);
   if(DoJewel)              KorinnaNoRecoilC01[4].Draw(JewelStyle2);
   if(DoJewel)              KorinnaNoRecoilC02[4].Draw(JewelStyle1);
   if(DoJewel)              KorinnaNoRecoilC02[4].Draw(JewelStyle2);
   if(DoJewel)              KorinnaNoRecoilC03[4].Draw(JewelStyle1);
   if(DoJewel)              KorinnaNoRecoilC03[4].Draw(JewelStyle2);
   if(DoTheory || DoPyquen) PyquenC00[4].Draw(PyquenStyle1);
   if(DoTheory || DoPyquen) PyquenC00[4].Draw(PyquenStyle2);
   if(DoTheory || DoPyquen) PyquenWideRadC00[4].Draw(PyquenStyle1);
   if(DoTheory || DoPyquen) PyquenWideRadC00[4].Draw(PyquenStyle2);
   if(DoPyquen)             PyquenB0[4].Draw(PyquenStyle1);
   if(DoPyquen)             PyquenB0[4].Draw(PyquenStyle2);
   if(DoPyquen)             PyquenWideRadB0[4].Draw(PyquenStyle1);
   if(DoPyquen)             PyquenWideRadB0[4].Draw(PyquenStyle2);
   if(!DoTheory && !NoData) HP4C0RRAASys.Draw("2");
   if(!DoTheory && !NoData) HP4C1RRAASys.Draw("2");
   if(!DoTheory && !NoData) HP4C2RRAASys.Draw("2");
   if(!DoTheory && !NoData) HP4C3RRAASys.Draw("2");
   if(!DoTheory && !NoData) HP4C0RRAA.Draw("p");
   if(!DoTheory && !NoData) HP4C1RRAA.Draw("p");
   if(!DoTheory && !NoData) HP4C2RRAA.Draw("p");
   if(!DoTheory && !NoData) HP4C3RRAA.Draw("p");
   GLine.Draw("l");
   Latex.SetTextAlign(11);
   Latex.DrawLatex(0.08, 0.9, "500 < p_{T} < 1000 GeV");
   P6.cd();
   HWorld.Draw("axis");
   Latex.SetTextAlign(11);
   Latex.DrawLatex(0.08, 0.9, "anti-k_{T}, |#eta_{jet}| < 2");
   if(!DoTheory && !NoData) Legend1.Draw();
   if(DoTheory && !NoData) LegendT1.Draw();
   if(DoTheory && NoData)  LegendTO1.Draw();
   if(DoJewel)             LegendJ1.Draw();
   if(DoPyquen)            LegendP1.Draw();
   if(DoHybrid)            LegendH1.Draw();

   string OutputBase = "PlotRRAA";
   if(DoTheory)   OutputBase = "PlotRRAATheory";
   if(NoData)     OutputBase = "PlotRRAATheoryOnly";
   if(DoJewel)    OutputBase = "JewelRRAA";
   if(DoPyquen)   OutputBase = "PyquenRRAA";
   if(DoHybrid)   OutputBase = "HybridRAA";

   Canvas.SaveAs((OutputBase + ".pdf").c_str());
   Canvas.SaveAs((OutputBase + ".png").c_str());
   Canvas.SaveAs((OutputBase + ".C").c_str());

   return 0;
}

void SetPad(TPad &P)
{
   // P.SetLogx();
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

   int Style[] = {20, 21, 22, 29};
   double Size[] = {1.8, 1.8, 1.8, 2.4};

   if(Color[C] == -1)
   {
      Color[0] = TColor::GetFreeColorIndex();
      new TColor(Color[0], 0.9648, 0.2969, 0.2344);   // E74C3C
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
   H.SetMarkerSize(Size[C]);
   H.SetLineWidth(2.0);

   File.Close();
}

vector<TGraphAsymmErrors> GetTheory(string TheoryFileName, int BaseColor)
{
   vector<TGraphAsymmErrors> Result(5);

   for(int i = 0; i < 5; i++)
   {
      Result[i].SetLineColor(BaseColor);
      Result[i].SetFillColorAlpha(BaseColor, 0.25);
      Result[i].SetMarkerColor(BaseColor);
      Result[i].SetMarkerStyle(20);
   }

   ifstream in(TheoryFileName);

   double JetR[] = {0.2, 0.3, 0.4, 0.6, 0.8, 1.0};
   double DR = 0.04;

   while(in)
   {
      char ch[10240];
      ch[0] = '\0';
      in.getline(ch, 10239, '\n');

      stringstream str(ch);

      double PTMin = 0, PTMax = 0;
      str >> PTMin >> PTMax;

      double PTAve = (PTMin + PTMax) / 2;

      int Index = -1;
      if(PTAve < 250 && PTAve > 200)    Index = 0;
      if(PTAve < 300 && PTAve > 250)    Index = 1;
      if(PTAve < 400 && PTAve > 300)    Index = 2;
      if(PTAve < 500 && PTAve > 400)    Index = 3;
      if(PTAve < 1000 && PTAve > 500)   Index = 4;

      if(Index < 0)
         continue;

      for(int i = 0; i < 6; i++)
      {
         double R, RMax, RMin;
         str >> R >> RMax >> RMin;

         Result[Index].SetPoint(i, JetR[i], R);
         Result[Index].SetPointError(i, DR, DR, R - RMin, RMax - R);
      }
   }

   return Result;
}




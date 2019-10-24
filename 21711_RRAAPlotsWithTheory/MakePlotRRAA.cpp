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

   string TheoryType = CL.Get("TheoryType", ""); // "", "All", "Jewel", "Pyquen", "Hybrid", "Ivan", "SCET"
   bool DoData = CL.GetBool("Data", true);
   bool DoTheory = (TheoryType == "All");
   bool DoJewel = (TheoryType == "Jewel");
   bool DoPyquen = (TheoryType == "Pyquen");
   bool DoHybrid = (TheoryType == "Hybrid");
   bool DoIvan = (TheoryType == "Ivan");
   bool DoSCET = (TheoryType == "SCET");
   bool OnlyData = DoData && !DoTheory && !DoJewel && !DoPyquen && !DoHybrid && !DoIvan && !DoSCET;
   bool FourPanel = DoData && !OnlyData; // Make smaller plots for theory data comparison because there are no data points in the first two panels otherwise.
   
   // Preamble: sizing
   int PanelSize = 500;
   int PaddingWidth = 100;
   int PaddingHeight = 100;
   int CanvasHeight = PanelSize * 2 + PaddingHeight * 2;
   int CanvasWidth = PanelSize * 3 + PaddingWidth * 2;
   if(FourPanel)  CanvasWidth = PanelSize * 2 + PaddingWidth * 2;

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
   if(DoHybrid)               YMin = 0.601;
   if(DoHybrid)               YMax = 1.799;

   string File = "RRAA_Smooth_RedStat.root";

   // double Lumi = 0.023;
   // double TAA[4] = {0.100719, 0.051233, 0.025341, 0.018646};

   // Start declaring canvas, pad, axis, and axis title (as TLatex)
   TCanvas Canvas("Canvas", "", CanvasWidth, CanvasHeight);

   TH2D HWorld("HWorld", ";;", 100, XMin, XMax, 100, YMin, YMax);
   HWorld.SetStats(0);
   HWorld.GetXaxis()->SetNdivisions(505);
   HWorld.GetYaxis()->SetNdivisions(505);

   const int NPad = 6;
   TPad *P[NPad] = {nullptr};
   if(FourPanel)
   {
      P[0] = new TPad("P1", "", PadX0 + PadDX * 0, PadY0 + PadDY * 1, PadX0 + PadDX * 1, PadY0 + PadDY * 2, 0);
      P[1] = new TPad("P2", "", PadX0 + PadDX * 1, PadY0 + PadDY * 1, PadX0 + PadDX * 2, PadY0 + PadDY * 2, 0);
      P[2] = new TPad("P4", "", PadX0 + PadDX * 0, PadY0 + PadDY * 0, PadX0 + PadDX * 1, PadY0 + PadDY * 1, 0);
      P[3] = new TPad("P5", "", PadX0 + PadDX * 1, PadY0 + PadDY * 0, PadX0 + PadDX * 2, PadY0 + PadDY * 1, 0);
   }
   else
   {
      P[0] = new TPad("P1", "", PadX0 + PadDX * 0, PadY0 + PadDY * 1, PadX0 + PadDX * 1, PadY0 + PadDY * 2, 0);
      P[1] = new TPad("P2", "", PadX0 + PadDX * 1, PadY0 + PadDY * 1, PadX0 + PadDX * 2, PadY0 + PadDY * 2, 0);
      P[2] = new TPad("P3", "", PadX0 + PadDX * 2, PadY0 + PadDY * 1, PadX0 + PadDX * 3, PadY0 + PadDY * 2, 0);
      P[3] = new TPad("P4", "", PadX0 + PadDX * 0, PadY0 + PadDY * 0, PadX0 + PadDX * 1, PadY0 + PadDY * 1, 0);
      P[4] = new TPad("P5", "", PadX0 + PadDX * 1, PadY0 + PadDY * 0, PadX0 + PadDX * 2, PadY0 + PadDY * 1, 0);
      P[5] = new TPad("P6", "", PadX0 + PadDX * 2, PadY0 + PadDY * 0, PadX0 + PadDX * 3, PadY0 + PadDY * 1, 0);
   }
   
   SetPad(*P[0]);
   SetPad(*P[1]);
   SetPad(*P[2]);
   SetPad(*P[3]);
   if(!FourPanel) SetPad(*P[4]);
   if(!FourPanel) SetPad(*P[5]);
  
   Canvas.cd();

   TGaxis AxisX1(PadX0 + PadDX * 0, PadY0 + PadDY * 0, PadX0 + PadDX * 1, PadY0 + PadDY * 0, XMin, XMax, 505, "");
   TGaxis AxisX2(PadX0 + PadDX * 1, PadY0 + PadDY * 0, PadX0 + PadDX * 2, PadY0 + PadDY * 0, XMin, XMax, 505, "");
   TGaxis AxisX3(PadX0 + PadDX * 2, PadY0 + PadDY * 0, PadX0 + PadDX * 3, PadY0 + PadDY * 0, XMin, XMax, 505, "");
   TGaxis AxisY1(PadX0 + PadDX * 0, PadY0 + PadDY * 0, PadX0 + PadDX * 0, PadY0 + PadDY * 1, YMin, YMax, 505, "");
   TGaxis AxisY2(PadX0 + PadDX * 0, PadY0 + PadDY * 1, PadX0 + PadDX * 0, PadY0 + PadDY * 2, YMin, YMax, 505, "");

   SetAxis(AxisX1);
   SetAxis(AxisX2);
   if(!FourPanel) SetAxis(AxisX3);
   SetAxis(AxisY1);
   SetAxis(AxisY2);

   TLatex Latex;
   Latex.SetNDC();

   Latex.SetTextFont(42);
   Latex.SetTextSize(0.030);
   Latex.SetTextAlign(22);
   Latex.SetTextAngle(0);
   if(FourPanel)  Latex.DrawLatex(PadX0 + PadDX * 1, PadY0 * 0.20, "Jet R");
   if(!FourPanel) Latex.DrawLatex(PadX0 + PadDX * 1.5, PadY0 * 0.20, "Jet R");

   Latex.SetTextFont(42);
   Latex.SetTextSize(0.030);
   Latex.SetTextAlign(22);
   Latex.SetTextAngle(90);
   Latex.DrawLatex(PadX0 * 0.20, PadY0 + PadDY, "R_{AA}^{R} / R_{AA}^{R = 0.2}");

   Latex.SetTextFont(62);
   Latex.SetTextSize(0.035);
   Latex.SetTextAlign(11);
   Latex.SetTextAngle(0);
   string TypeTag = "";
   if(DoData)               TypeTag = "CMS";
   if(DoTheory && !DoData)  TypeTag += "Theory";
   if(DoJewel && !DoData)   TypeTag += "Jewel";
   if(DoPyquen && !DoData)  TypeTag += "Pyquen";
   if(DoHybrid && !DoData)  TypeTag += "Hybrid";
   if(DoIvan && !DoData)    TypeTag += "Li and Vitev";
   if(DoSCET && !DoData)    TypeTag += "SCET";
   Latex.DrawLatex(PadX0, PadY0 + PadDY * 2 + PadY0 * 0.15, TypeTag.c_str());

   Latex.SetTextFont(42);
   Latex.SetTextSize(0.030);
   Latex.SetTextAlign(31);
   Latex.SetTextAngle(0);
   if(FourPanel && DoData) Latex.DrawLatex(PadX0 + PadDX * 2, PadY0 + PadDY * 2 + PadY0 * 0.15, "#sqrt{s_{NN}} = 5.02 TeV, PbPb 404 #mub^{-1}, pp 27.4 pb^{-1}");
   if(FourPanel && !DoData) Latex.DrawLatex(PadX0 + PadDX * 2, PadY0 + PadDY * 2 + PadY0 * 0.15, "#sqrt{s_{NN}} = 5.02 TeV");
   
   if(!FourPanel && DoData) Latex.DrawLatex(PadX0 + PadDX * 3, PadY0 + PadDY * 2 + PadY0 * 0.15, "#sqrt{s_{NN}} = 5.02 TeV, PbPb 404 #mub^{-1}, pp 27.4 pb^{-1}");
   if(!FourPanel && !DoData) Latex.DrawLatex(PadX0 + PadDX * 3, PadY0 + PadDY * 2 + PadY0 * 0.15, "#sqrt{s_{NN}} = 5.02 TeV");

   TGraph GLine;
   GLine.SetPoint(0, XMin, 1);
   GLine.SetPoint(1, XMax, 1);
   GLine.SetLineStyle(kDashed);
   
   const int NBins = 5;
   TGraphAsymmErrors HC0RRAA[NBins];
   TGraphAsymmErrors HC1RRAA[NBins];
   TGraphAsymmErrors HC2RRAA[NBins];
   TGraphAsymmErrors HC3RRAA[NBins];

   GetRRAA(HC0RRAA[0], File, PT0, C0, false);
   GetRRAA(HC1RRAA[0], File, PT0, C1, false);
   GetRRAA(HC2RRAA[0], File, PT0, C2, false);
   GetRRAA(HC3RRAA[0], File, PT0, C3, false);
   GetRRAA(HC0RRAA[1], File, PT1, C0, false);
   GetRRAA(HC1RRAA[1], File, PT1, C1, false);
   GetRRAA(HC2RRAA[1], File, PT1, C2, false);
   GetRRAA(HC3RRAA[1], File, PT1, C3, false);
   GetRRAA(HC0RRAA[2], File, PT2, C0, false);
   GetRRAA(HC1RRAA[2], File, PT2, C1, false);
   GetRRAA(HC2RRAA[2], File, PT2, C2, false);
   GetRRAA(HC3RRAA[2], File, PT2, C3, false);
   GetRRAA(HC0RRAA[3], File, PT3, C0, false);
   GetRRAA(HC1RRAA[3], File, PT3, C1, false);
   GetRRAA(HC2RRAA[3], File, PT3, C2, false);
   GetRRAA(HC3RRAA[3], File, PT3, C3, false);
   GetRRAA(HC0RRAA[4], File, PT4, C0, false);
   GetRRAA(HC1RRAA[4], File, PT4, C1, false);
   GetRRAA(HC2RRAA[4], File, PT4, C2, false);
   GetRRAA(HC3RRAA[4], File, PT4, C3, false);
   
   TGraphAsymmErrors HC0RRAASys[NBins];
   TGraphAsymmErrors HC1RRAASys[NBins];
   TGraphAsymmErrors HC2RRAASys[NBins];
   TGraphAsymmErrors HC3RRAASys[NBins];
   
   GetRRAA(HC0RRAASys[0], File, PT0, C0, true);
   GetRRAA(HC1RRAASys[0], File, PT0, C1, true);
   GetRRAA(HC2RRAASys[0], File, PT0, C2, true);
   GetRRAA(HC3RRAASys[0], File, PT0, C3, true);
   GetRRAA(HC0RRAASys[1], File, PT1, C0, true);
   GetRRAA(HC1RRAASys[1], File, PT1, C1, true);
   GetRRAA(HC2RRAASys[1], File, PT1, C2, true);
   GetRRAA(HC3RRAASys[1], File, PT1, C3, true);
   GetRRAA(HC0RRAASys[2], File, PT2, C0, true);
   GetRRAA(HC1RRAASys[2], File, PT2, C1, true);
   GetRRAA(HC2RRAASys[2], File, PT2, C2, true);
   GetRRAA(HC3RRAASys[2], File, PT2, C3, true);
   GetRRAA(HC0RRAASys[3], File, PT3, C0, true);
   GetRRAA(HC1RRAASys[3], File, PT3, C1, true);
   GetRRAA(HC2RRAASys[3], File, PT3, C2, true);
   GetRRAA(HC3RRAASys[3], File, PT3, C3, true);
   GetRRAA(HC0RRAASys[4], File, PT4, C0, true);
   GetRRAA(HC1RRAASys[4], File, PT4, C1, true);
   GetRRAA(HC2RRAASys[4], File, PT4, C2, true);
   GetRRAA(HC3RRAASys[4], File, PT4, C3, true);

   vector<TGraphAsymmErrors> Felix = GetTheory("Theory/Felix/FelixRRAA.txt", kBlack);
   vector<TGraphAsymmErrors> DanielWake = GetTheory("Theory/Daniel/ForCMS/r_RAA/Daniel_wake.txt", kMagenta);
   vector<TGraphAsymmErrors> DanielOnlyPos = GetTheory("Theory/Daniel/ForCMS/r_RAA/Daniel_onlypos.txt", kPink - 2);
   vector<TGraphAsymmErrors> DanielNoWake = GetTheory("Theory/Daniel/ForCMS/r_RAA/Daniel_nowake.txt", kOrange);
   vector<TGraphAsymmErrors> KorinnaC00 = GetTheory("Theory/Korinna/JewelData_PbPb_0to10_RRAA.txt", kOrange - 3);
   vector<TGraphAsymmErrors> KorinnaC01 = GetTheory("Theory/Korinna/JewelData_PbPb_10to30_RRAA.txt", kRed - 7);
   vector<TGraphAsymmErrors> KorinnaC02 = GetTheory("Theory/Korinna/JewelData_PbPb_30to50_RRAA.txt", kPink - 9);
   vector<TGraphAsymmErrors> KorinnaC03 = GetTheory("Theory/Korinna/JewelData_PbPb_50to90_RRAA.txt", kPink - 6);
   vector<TGraphAsymmErrors> KorinnaNoRecoilC00 = GetTheory("Theory/Korinna/JewelData_PbPbNoRecoil_0to10_RRAA.txt", kMagenta + 2);
   vector<TGraphAsymmErrors> KorinnaNoRecoilC01 = GetTheory("Theory/Korinna/JewelData_PbPbNoRecoil_10to30_RRAA.txt", kMagenta);
   vector<TGraphAsymmErrors> KorinnaNoRecoilC02 = GetTheory("Theory/Korinna/JewelData_PbPbNoRecoil_30to50_RRAA.txt", kViolet + 1);
   vector<TGraphAsymmErrors> KorinnaNoRecoilC03 = GetTheory("Theory/Korinna/JewelData_PbPbNoRecoil_50to90_RRAA.txt", kBlue - 5);
//   vector<TGraphAsymmErrors> PyquenB0 = GetTheory("Theory/Pyquen/PyquenData_PbPb_RRAA.txt", kBlue);
//   vector<TGraphAsymmErrors> PyquenWideRadB0 = GetTheory("Theory/Pyquen/PyquenData_PbPbWide_RRAA.txt", kViolet + 5);
   vector<TGraphAsymmErrors> PyquenC00 = GetTheory("Theory/Pyquen/PyquenData_PbPb_0to10_RRAA.txt", kTeal + 5);
   vector<TGraphAsymmErrors> PyquenWideRadC00 = GetTheory("Theory/Pyquen/PyquenData_PbPbWide_0to10_RRAA.txt", kTeal + 3);
   vector<TGraphAsymmErrors> IvanC00 = GetTheory("Theory/Ivan/RRAA_Centrality-0-10.txt", kBlue); // Only R = 02, 04, 08
   vector<TGraphAsymmErrors> IvanC01 = GetTheory("Theory/Ivan/RRAA_Centrality-10-30.txt", kGreen + 1);// Only R = 02, 04, 08
   vector<TGraphAsymmErrors> IvanC02 = GetTheory("Theory/Ivan/RRAA_Centrality-30-50.txt", kGreen + 3);// Only R = 02, 04, 08
   vector<TGraphAsymmErrors> SCET = GetTheory("Theory/SCET/SCET_RRAA.txt", kCyan + 1);// Only R = 02, 04, 06, 08, 10
   
   int NEntries;
   if(OnlyData)   NEntries = 4;
   if(DoTheory)   NEntries = 7;
   if(DoJewel)    NEntries = 8;
   if(DoPyquen)   NEntries = 2;
   if(DoHybrid)   NEntries = 3;
   if(DoIvan)     NEntries = 3;
   if(DoSCET)     NEntries = 1;
   if(DoData)     NEntries += 1;
   
   double LegendHeight = (NEntries*0.1 < 0.77) ? NEntries*0.1 : 0.77;
   
   TLegend Legend1(0.1, 0.08, 0.5, 0.08 + LegendHeight);
   Legend1.SetTextFont(42);
   Legend1.SetTextSize(0.07);
   Legend1.SetFillStyle(0);
   Legend1.SetBorderSize(0);
   if(OnlyData)   Legend1.AddEntry(&HC0RRAASys[0], "0-10%", "plf");
   if(OnlyData)   Legend1.AddEntry(&HC1RRAASys[0], "10-30%", "plf");
   if(OnlyData)   Legend1.AddEntry(&HC2RRAASys[0], "30-50%", "plf");
   if(OnlyData)   Legend1.AddEntry(&HC3RRAASys[0], "50-90%", "plf");
   if(!OnlyData && DoData)   Legend1.AddEntry(&HC0RRAASys[0], "CMS 0-10%", "plf");
   if(DoTheory)   Legend1.AddEntry(&Felix[0], "Factorization", "lf");
   if(DoHybrid || DoTheory)   Legend1.AddEntry(&DanielWake[0], "Hybrid w/ wake", "lf");
   if(DoHybrid || DoTheory)   Legend1.AddEntry(&DanielNoWake[0], "Hybrid w/o wake", "lf");
   if(DoHybrid || DoTheory)   Legend1.AddEntry(&DanielOnlyPos[0], "Hybrid w/ partial response", "lf");
   if(DoTheory)   Legend1.AddEntry(&KorinnaC00[0], "Jewel", "lf");
   if(DoTheory)   Legend1.AddEntry(&KorinnaNoRecoilC00[0], "Jewel w/o recoil", "lf");
   if(DoPyquen || DoTheory)   Legend1.AddEntry(&PyquenC00[0], "Pyquen", "lf");
   if(DoPyquen || DoTheory)   Legend1.AddEntry(&PyquenWideRadC00[0], "Pyquen w/ wide angle rad.", "lf");
   if(DoTheory)   Legend1.AddEntry(&IvanC00[0], "Li and Vitev", "f");
   if(DoJewel)    Legend1.AddEntry(&KorinnaC00[0], "Jewel 0-10%", "lf");
   if(DoJewel)    Legend1.AddEntry(&KorinnaC01[0], "Jewel 10-30%", "lf");
   if(DoJewel)    Legend1.AddEntry(&KorinnaC02[0], "Jewel 30-50%", "lf");
   if(DoJewel)    Legend1.AddEntry(&KorinnaC03[0], "Jewel 50-90%", "lf");
   if(DoJewel)    Legend1.AddEntry(&KorinnaNoRecoilC00[0], "Jewel w/o recoil 0-10%", "lf");
   if(DoJewel)    Legend1.AddEntry(&KorinnaNoRecoilC01[0], "Jewel w/o recoil 10-30%", "lf");
   if(DoJewel)    Legend1.AddEntry(&KorinnaNoRecoilC02[0], "Jewel w/o recoil 30-50%", "lf");
   if(DoJewel)    Legend1.AddEntry(&KorinnaNoRecoilC03[0], "Jewel w/o recoil 50-90%", "lf");
   if(DoIvan)     Legend1.AddEntry(&IvanC00[0], "Li and Vitev 0-10%", "f");
   if(DoIvan)     Legend1.AddEntry(&IvanC01[0], "Li and Vitev 10-30%", "f");
   if(DoIvan)     Legend1.AddEntry(&IvanC02[0], "Li and Vitev 30-50%", "f");
   if(DoTheory || DoSCET)     Legend1.AddEntry(&SCET[0], "SCET w/o coll. E-loss", "f");

   Latex.SetTextFont(42);
   Latex.SetTextSize(0.07);
   Latex.SetTextAlign(31);
   Latex.SetTextAngle(0);
   
   for(int i = 0; i < NBins; i++)
   {
      if(FourPanel && i < 2) continue; // Want to cut off the first two bins.
      
      if(FourPanel)  P[i-2]->cd();
      if(!FourPanel) P[i]->cd();
      
      HWorld.Draw("axis");
      if(DoTheory)             Felix[i].Draw("3");
      if(DoTheory)             Felix[i].Draw("lX");
      if(DoTheory || DoHybrid) DanielWake[i].Draw("3");
      if(DoTheory || DoHybrid) DanielWake[i].Draw("lX");
      if(DoTheory || DoHybrid) DanielNoWake[i].Draw("3");
      if(DoTheory || DoHybrid) DanielNoWake[i].Draw("lX");
      if(DoTheory || DoHybrid) DanielOnlyPos[i].Draw("3");
      if(DoTheory || DoHybrid) DanielOnlyPos[i].Draw("lX");
      if(DoTheory || DoJewel)  KorinnaC00[i].Draw("3");
      if(DoTheory || DoJewel)  KorinnaC00[i].Draw("lX");
      if(DoJewel)              KorinnaC01[i].Draw("3");
      if(DoJewel)              KorinnaC01[i].Draw("lX");
      if(DoJewel)              KorinnaC02[i].Draw("3");
      if(DoJewel)              KorinnaC02[i].Draw("lX");
      if(DoJewel)              KorinnaC03[i].Draw("3");
      if(DoJewel)              KorinnaC03[i].Draw("lX");
      if(DoTheory || DoJewel)  KorinnaNoRecoilC00[i].Draw("3");
      if(DoTheory || DoJewel)  KorinnaNoRecoilC00[i].Draw("lX");
      if(DoJewel)              KorinnaNoRecoilC01[i].Draw("3");
      if(DoJewel)              KorinnaNoRecoilC01[i].Draw("lX");
      if(DoJewel)              KorinnaNoRecoilC02[i].Draw("3");
      if(DoJewel)              KorinnaNoRecoilC02[i].Draw("lX");
      if(DoJewel)              KorinnaNoRecoilC03[i].Draw("3");
      if(DoJewel)              KorinnaNoRecoilC03[i].Draw("lX");
      if(DoTheory || DoPyquen) PyquenC00[i].Draw("3");
      if(DoTheory || DoPyquen) PyquenC00[i].Draw("lX");
      if(DoTheory || DoPyquen) PyquenWideRadC00[i].Draw("3");
      if(DoTheory || DoPyquen) PyquenWideRadC00[i].Draw("lX");
      if(DoTheory || DoIvan)   IvanC00[i].Draw("5");
      if(DoIvan)               IvanC01[i].Draw("5");
      if(DoIvan)               IvanC02[i].Draw("5");
      if(DoTheory || DoSCET)   SCET[i].Draw("5");
      if(DoData)               HC0RRAASys[i].Draw("2");
      if(OnlyData)             HC1RRAASys[i].Draw("2");
      if(OnlyData)             HC2RRAASys[i].Draw("2");
      if(OnlyData)             HC3RRAASys[i].Draw("2");
      if(DoData)               HC0RRAA[i].Draw("p");
      if(OnlyData)             HC1RRAA[i].Draw("p");
      if(OnlyData)             HC2RRAA[i].Draw("p");
      if(OnlyData)             HC3RRAA[i].Draw("p");
      GLine.Draw("l");
   }
   
   if(FourPanel)
   {
      P[0]->cd();
      Latex.SetTextAlign(11);
      Latex.DrawLatex(0.08, 0.9, "300 < p_{T} < 400 GeV");
      
      P[1]->cd();
      Latex.SetTextAlign(11);
      Latex.DrawLatex(0.08, 0.9, "400 < p_{T} < 500 GeV");
      
      P[2]->cd();
      Latex.SetTextAlign(11);
      Latex.DrawLatex(0.08, 0.9, "500 < p_{T} < 1000 GeV");
      
      P[3]->cd();
      HWorld.Draw("axis");
      Latex.SetTextAlign(11);
      Latex.DrawLatex(0.08, 0.9, "anti-k_{T}, |#eta_{jet}| < 2");
      Legend1.Draw();
   }
   else
   {
      P[0]->cd();
      Latex.SetTextAlign(11);
      Latex.DrawLatex(0.08, 0.9, "200 < p_{T} < 250 GeV");
      
      P[1]->cd();
      Latex.SetTextAlign(11);
      Latex.DrawLatex(0.08, 0.9, "250 < p_{T} < 300 GeV");
      
      P[2]->cd();
      Latex.SetTextAlign(11);
      Latex.DrawLatex(0.08, 0.9, "300 < p_{T} < 400 GeV");
      
      P[3]->cd();
      Latex.SetTextAlign(11);
      Latex.DrawLatex(0.08, 0.9, "400 < p_{T} < 500 GeV");
      
      P[4]->cd();
      Latex.SetTextAlign(11);
      Latex.DrawLatex(0.08, 0.9, "500 < p_{T} < 1000 GeV");
      
      P[5]->cd();
      HWorld.Draw("axis");
      Latex.SetTextAlign(11);
      Latex.DrawLatex(0.08, 0.9, "anti-k_{T}, |#eta_{jet}| < 2");
      Legend1.Draw();
   }

   string OutputBase = "";
   if(DoData)     OutputBase = "Data";
   if(DoTheory)   OutputBase += "Theory";
   if(DoJewel)    OutputBase += "Jewel";
   if(DoPyquen)   OutputBase += "Pyquen";
   if(DoHybrid)   OutputBase += "Hybrid";
   if(DoIvan)     OutputBase += "Ivan";
   if(DoSCET)     OutputBase += "SCET";
   OutputBase += "RRAA";

   Canvas.SaveAs((OutputBase + ".pdf").c_str());
   Canvas.SaveAs((OutputBase + ".png").c_str());
   Canvas.SaveAs((OutputBase + ".C").c_str());
   
   for(int i = 0; i < NPad; i++)
      if(P[i] != nullptr)  delete P[i];

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
   double Size[] = {1.8, 1.8, 2.0, 2.6};
//   double Size[] = {2.2, 2.2, 2.2, 2.2};

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




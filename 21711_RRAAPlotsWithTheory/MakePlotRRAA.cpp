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
   CommandLine CL(argc, argv);

   string TheoryType = CL.Get("TheoryType", ""); // "", "All", "Generators", "MC", "Calculations", "LBT", "None"
   bool DoData = CL.GetBool("Data", true);
   bool Pres = CL.GetBool("Pres", true);
   bool Prelim = CL.GetBool("Prelim", true);
   
   bool Do0to10All = (TheoryType == "All");
   bool Do0to10Gen = (TheoryType == "Generators");      // Jewel, Pyquen
   bool Do0to10MC = (TheoryType == "MC");               // Hybrid, Martini
   bool Do0to10Calc = (TheoryType == "Calculations");   // Ivan, Felix, Yang-Ting, Yacine
   
   bool Do0to10 = Do0to10All || Do0to10Gen || Do0to10MC || Do0to10Calc;
   bool Do10to30 = (TheoryType == "10to30");            // Ivan, Jewel, Martini
   bool Do30to50 = (TheoryType == "30to50");            // Ivan, Jewel, Martini
   bool Do50to90 = (TheoryType == "50to90");            // Jewel
   
   bool DoTheory = Do0to10 || Do10to30 || Do30to50 || Do50to90;
   bool OnlyData = DoData && !DoTheory;
   
   bool FourPanel = DoData && Do0to10; // Make smaller plots for theory data comparison because there are no data points in the first two panels otherwise for 0-10%.
   
   // Preamble: sizing
   int PanelSize = 500;
   int PaddingWidthL = Pres ? 140 : 100;
   int PaddingWidthR = 50;
   int PaddingHeight = Pres ? 120 : 100;
   int CanvasHeight = PanelSize * 2 + PaddingHeight * 2;
   int CanvasWidth = PanelSize * 3 + PaddingWidthL + PaddingWidthR;
   if(FourPanel)  CanvasWidth = PanelSize * 2 + PaddingWidthL + PaddingWidthR;

   double PadX0 = (double)PaddingWidthL / CanvasWidth;
   double PadY0 = (double)PaddingHeight / CanvasHeight;
   double PadDX = (double)PanelSize / CanvasWidth;
   double PadDY = (double)PanelSize / CanvasHeight;

   double XMin = 0.15;
   double XMax = 1.10;
   double YMin = OnlyData ? 0.601 : 0.201;
   double YMax = OnlyData ? 1.499 : 1.999;

   string File = "RRAA_Smooth_RedStat.root";
   
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

   SetAxis(AxisX1, Pres);
   SetAxis(AxisX2, Pres);
   if(!FourPanel) SetAxis(AxisX3, Pres);
   SetAxis(AxisY1, Pres);
   SetAxis(AxisY2, Pres);

   TLatex Latex;
   Latex.SetNDC();

   Latex.SetTextFont(42);
   if(Pres) Latex.SetTextSize(0.045);
   else     Latex.SetTextSize(0.030);
   Latex.SetTextAlign(22);
   Latex.SetTextAngle(0);
   if(FourPanel)  Latex.DrawLatex(PadX0 + PadDX * 1, PadY0 * 0.25, "Jet R");
   if(!FourPanel) Latex.DrawLatex(PadX0 + PadDX * 1.5, PadY0 * 0.25, "Jet R");

   Latex.SetTextFont(42);
   if(Pres) Latex.SetTextSize(0.045);
   else     Latex.SetTextSize(0.030);
   Latex.SetTextAlign(22);
   Latex.SetTextAngle(90);
   Latex.DrawLatex(PadX0 * 0.25, PadY0 + PadDY, "R_{AA}^{R} / R_{AA}^{R = 0.2}");

   Latex.SetTextFont(62);
   if(Pres) Latex.SetTextSize(0.045);
   else     Latex.SetTextSize(0.035);
   Latex.SetTextAlign(11);
   Latex.SetTextAngle(0);
   string TypeTag = "";
   if(DoData)               TypeTag = "CMS";
   if(Prelim)               TypeTag += " #scale[0.8]{#font[52]{Preliminary}}";
   if(DoTheory && !DoData)  TypeTag = "Theory";
   Latex.DrawLatex(PadX0, PadY0 + PadDY * 2 + PadY0 * 0.15, TypeTag.c_str());

   Latex.SetTextFont(42);
   if(Pres) Latex.SetTextSize(0.035);
   else     Latex.SetTextSize(0.030);
   Latex.SetTextAlign(31);
   Latex.SetTextAngle(0);
   if(FourPanel && !Pres) Latex.DrawLatex(PadX0 + PadDX * 2, PadY0 + PadDY * 2 + PadY0 * 0.15, "#sqrt{s_{NN}} = 5.02 TeV, PbPb 404 #mub^{-1}, pp 27.4 pb^{-1}");
   if(FourPanel && Pres)  Latex.DrawLatex(PadX0 + PadDX * 2, PadY0 + PadDY * 2 + PadY0 * 0.15, "PbPb 404 #mub^{-1}, pp 27.4 pb^{-1}");
   
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
   vector<TGraphAsymmErrors> DanielWake = GetTheory("Theory/Daniel/ForCMS/r_RAA/Daniel_wake.txt", kOrange + 10);
   vector<TGraphAsymmErrors> DanielOnlyPos = GetTheory("Theory/Daniel/ForCMS/r_RAA/Daniel_onlypos.txt", kOrange);
   vector<TGraphAsymmErrors> DanielNoWake = GetTheory("Theory/Daniel/ForCMS/r_RAA/Daniel_nowake.txt", kOrange + 4);
   vector<TGraphAsymmErrors> KorinnaC00 = GetTheory("Theory/Korinna/JewelData_PbPb_0to10_RRAA.txt", kPink + 1);
   vector<TGraphAsymmErrors> KorinnaC01 = GetTheory("Theory/Korinna/JewelData_PbPb_10to30_RRAA.txt", kPink + 1);
   vector<TGraphAsymmErrors> KorinnaC02 = GetTheory("Theory/Korinna/JewelData_PbPb_30to50_RRAA.txt", kPink + 1);
   vector<TGraphAsymmErrors> KorinnaC03 = GetTheory("Theory/Korinna/JewelData_PbPb_50to90_RRAA.txt", kPink + 1);
   vector<TGraphAsymmErrors> KorinnaNoRecoilC00 = GetTheory("Theory/Korinna/JewelData_PbPbNoRecoil_0to10_RRAA.txt", kMagenta);
   vector<TGraphAsymmErrors> KorinnaNoRecoilC01 = GetTheory("Theory/Korinna/JewelData_PbPbNoRecoil_10to30_RRAA.txt", kMagenta);
   vector<TGraphAsymmErrors> KorinnaNoRecoilC02 = GetTheory("Theory/Korinna/JewelData_PbPbNoRecoil_30to50_RRAA.txt", kMagenta);
   vector<TGraphAsymmErrors> KorinnaNoRecoilC03 = GetTheory("Theory/Korinna/JewelData_PbPbNoRecoil_50to90_RRAA.txt", kMagenta);
   vector<TGraphAsymmErrors> PyquenC00 = GetTheory("Theory/Pyquen/PyquenTest_PbPb350_0to10_RRAA.txt", kCyan + 3);
   vector<TGraphAsymmErrors> PyquenWideRadC00 = GetTheory("Theory/Pyquen/PyquenTest_PbPbWide350_0to10_RRAA.txt", kCyan - 6);
   vector<TGraphAsymmErrors> IvanC00 = GetTheory("Theory/Ivan/RRAA_Centrality-0-10.txt", kBlue); // Only R = 02, 04, 08
   vector<TGraphAsymmErrors> IvanC01 = GetTheory("Theory/Ivan/RRAA_Centrality-10-30.txt", kBlue);// Only R = 02, 04, 08
   vector<TGraphAsymmErrors> IvanC02 = GetTheory("Theory/Ivan/RRAA_Centrality-30-50.txt", kBlue);// Only R = 02, 04, 08
   vector<TGraphAsymmErrors> SCET = GetTheory("Theory/SCET/SCET_RRAA.txt", kAzure + 7);// Only R = 02, 04, 06, 08, 10
   vector<TGraphAsymmErrors> MartiniC00 = GetTheory("Theory/Martini/MARTINI_RRAA_0to10.txt", kViolet - 3);
   vector<TGraphAsymmErrors> MartiniC01 = GetTheory("Theory/Martini/MARTINI_RRAA_10to30.txt", kViolet - 3);
   vector<TGraphAsymmErrors> MartiniC02 = GetTheory("Theory/Martini/MARTINI_RRAA_30to50.txt", kViolet - 3);
   vector<TGraphAsymmErrors> LBTNoResponse = GetTheory("Theory/LBT/RRAA_showers_only.txt", kSpring - 6); // Only R = 02, 04, 06, 08, 10
   vector<TGraphAsymmErrors> LBTResponse = GetTheory("Theory/LBT/RRAA_response.txt", kSpring + 5); // Only R = 02, 04, 06, 08, 10
   vector<TGraphAsymmErrors> Yacine = GetTheory("Theory/Yacine/RRAA.txt", kOrange + 1); // Only R = 02, 04, 06, 08, 10
   vector<TGraphAsymmErrors> CCNUHydro = GetTheory("Theory/CCNU/RRAA_hydro.txt", kBlue - 7);
   vector<TGraphAsymmErrors> CCNUNoHydro = GetTheory("Theory/CCNU/RRAA_no_hydro.txt", kViolet + 2);
   
   int NEntries;
   
   if(OnlyData)      NEntries = 4;
   if(Do0to10Calc)   NEntries = 4;
   if(Do0to10MC)     NEntries = 8;
   if(Do0to10Gen)    NEntries = 4;
   if(Do0to10All)    NEntries = 14;
   if(Do10to30)      NEntries = 4;
   if(Do30to50)      NEntries = 4;
   if(Do50to90)      NEntries = 2;
   if(DoData)     NEntries += 1;
   
   double LegendHeight = (NEntries*0.1 < 0.77) ? NEntries*0.1 : 0.79;
   double LegendBase = (NEntries < 5) ? 0.08 : 0.03;
   double LegendX0 = 0.1;
   
   TLegend Legend1(LegendX0, LegendBase, LegendX0 + 0.4, LegendBase + LegendHeight);
   Legend1.SetTextFont(42);
   Legend1.SetTextSize(0.07);
   if(Do0to10All) Legend1.SetTextSize(0.06);
   Legend1.SetFillStyle(0);
   Legend1.SetBorderSize(0);
   if(OnlyData)   Legend1.AddEntry(&HC0RRAASys[0], "0-10%", "plf");
   if(OnlyData)   Legend1.AddEntry(&HC1RRAASys[0], "10-30%", "plf");
   if(OnlyData)   Legend1.AddEntry(&HC2RRAASys[0], "30-50%", "plf");
   if(OnlyData)   Legend1.AddEntry(&HC3RRAASys[0], "50-90%", "plf");
   if(DoData && Do0to10)   Legend1.AddEntry(&HC0RRAASys[0], "CMS", "plf");
   if(DoData && Do10to30)  Legend1.AddEntry(&HC1RRAASys[0], "CMS", "plf");
   if(DoData && Do30to50)  Legend1.AddEntry(&HC2RRAASys[0], "CMS", "plf");
   if(DoData && Do50to90)  Legend1.AddEntry(&HC3RRAASys[0], "CMS", "plf");
   if(Do0to10Calc || Do0to10All)     Legend1.AddEntry(&Felix[0], "Factorization", "lf");
   if(Do0to10Calc || Do0to10All)     Legend1.AddEntry(&SCET[0], "SCET w/o coll. E-loss", "f");
   if(Do0to10Calc || Do0to10All)     Legend1.AddEntry(&IvanC00[0], "Li and Vitev", "f");
   if(Do0to10Calc || Do0to10All)     Legend1.AddEntry(&Yacine[0], "Coherent antenna BDMPS", "lf");
   if(Do0to10MC || Do0to10All)       Legend1.AddEntry(&DanielWake[0], "Hybrid w/ wake", "lf");
   if(Do0to10MC || Do0to10All)       Legend1.AddEntry(&DanielNoWake[0], "Hybrid w/o wake", "lf");
   if(Do0to10MC || Do0to10All)       Legend1.AddEntry(&DanielOnlyPos[0], "Hybrid w/ pos wake", "lf");
   if(Do0to10MC || Do0to10All)       Legend1.AddEntry(&MartiniC00[0], "MARTINI", "lf");
//   if(Do0to10MC || Do0to10All)       Legend1.AddEntry(&CCNUHydro[0], "CCNU coupled jet-fluid w/ hydro", "f");
//   if(Do0to10MC || Do0to10All)       Legend1.AddEntry(&CCNUNoHydro[0], "CCNU coupled jet-fluid w/o hydro", "f");
   if(Do0to10Gen || Do0to10All)      Legend1.AddEntry(&PyquenC00[0], "PYQUEN", "lf");
   if(Do0to10Gen || Do0to10All)      Legend1.AddEntry(&PyquenWideRadC00[0], "PYQUEN w/ wide angle rad.", "lf");
   if(Do0to10Gen || Do0to10All)      Legend1.AddEntry(&KorinnaC00[0], "JEWEL", "lf");
   if(Do0to10Gen || Do0to10All)      Legend1.AddEntry(&KorinnaNoRecoilC00[0], "JEWEL w/o recoil", "lf");
   if(Do0to10MC || Do0to10All)       Legend1.AddEntry(&LBTNoResponse[0], "LBT w/ showers only", "f");
   if(Do0to10MC || Do0to10All)       Legend1.AddEntry(&LBTResponse[0], "LBT w/ med. response", "f");
   if(Do10to30)   Legend1.AddEntry(&KorinnaC01[0], "Jewel", "lf");
   if(Do10to30)   Legend1.AddEntry(&KorinnaNoRecoilC01[0], "Jewel w/o recoil", "lf");
   if(Do10to30)   Legend1.AddEntry(&IvanC01[0], "Li and Vitev", "f");
   if(Do10to30)   Legend1.AddEntry(&MartiniC01[0], "MARTINI", "lf");
   if(Do30to50)   Legend1.AddEntry(&KorinnaC02[0], "Jewel", "lf");
   if(Do30to50)   Legend1.AddEntry(&KorinnaNoRecoilC02[0], "Jewel w/o recoil", "lf");
   if(Do30to50)   Legend1.AddEntry(&IvanC02[0], "Li and Vitev", "f");
   if(Do30to50)   Legend1.AddEntry(&MartiniC02[0], "MARTINI", "lf");
   if(Do50to90)   Legend1.AddEntry(&KorinnaC03[0], "Jewel", "lf");
   if(Do50to90)   Legend1.AddEntry(&KorinnaNoRecoilC03[0], "Jewel w/o recoil", "lf");

   Latex.SetTextFont(42);
   if(Pres) Latex.SetTextSize(0.09);
   else     Latex.SetTextSize(0.07);
   Latex.SetTextAlign(31);
   Latex.SetTextAngle(0);
   
   for(int i = 0; i < NBins; i++)
   {
      double LineWidth = (FourPanel) ? 3 : 2;
      DanielWake[i].SetLineWidth(LineWidth);
      DanielNoWake[i].SetLineWidth(LineWidth);
      DanielOnlyPos[i].SetLineWidth(LineWidth);
      Yacine[i].SetLineWidth(LineWidth);
      MartiniC00[i].SetLineWidth(LineWidth);
      MartiniC01[i].SetLineWidth(LineWidth);
      MartiniC02[i].SetLineWidth(LineWidth);
      PyquenC00[i].SetLineWidth(LineWidth);
      PyquenWideRadC00[i].SetLineWidth(LineWidth);
      KorinnaC00[i].SetLineWidth(LineWidth);
      KorinnaC01[i].SetLineWidth(LineWidth);
      KorinnaC02[i].SetLineWidth(LineWidth);
      KorinnaC03[i].SetLineWidth(LineWidth);
      KorinnaNoRecoilC00[i].SetLineWidth(LineWidth);
      KorinnaNoRecoilC01[i].SetLineWidth(LineWidth);
      KorinnaNoRecoilC02[i].SetLineWidth(LineWidth);
      KorinnaNoRecoilC03[i].SetLineWidth(LineWidth);
      PyquenWideRadC00[i].SetLineWidth(LineWidth);
      Felix[i].SetLineWidth(LineWidth);
      
      if(FourPanel && i < 2) continue; // Want to cut off the first two bins.
      
      if(FourPanel)  P[i-2]->cd();
      if(!FourPanel) P[i]->cd();
      
      HWorld.Draw("axis");
      if(Do0to10Calc || Do0to10All)     Felix[i].Draw("3");
      if(Do0to10Calc || Do0to10All)     Felix[i].Draw("lX");
      if(Do0to10Calc || Do0to10All)     SCET[i].Draw("5");
      if(Do0to10Calc || Do0to10All)     IvanC00[i].Draw("5");
      if(Do0to10Calc || Do0to10All)     Yacine[i].Draw("5");
      if(Do0to10MC || Do0to10All)       DanielWake[i].Draw("3");
      if(Do0to10MC || Do0to10All)       DanielWake[i].Draw("lX");
      if(Do0to10MC || Do0to10All)       DanielNoWake[i].Draw("3");
      if(Do0to10MC || Do0to10All)       DanielNoWake[i].Draw("lX");
      if(Do0to10MC || Do0to10All)       DanielOnlyPos[i].Draw("3");
      if(Do0to10MC || Do0to10All)       DanielOnlyPos[i].Draw("lX");
      if(Do0to10MC || Do0to10All)       MartiniC00[i].Draw("3");
      if(Do0to10MC || Do0to10All)       MartiniC00[i].Draw("lX");
      if(Do0to10MC || Do0to10All)       CCNUHydro[i].Draw("5");
      if(Do0to10MC || Do0to10All)       CCNUNoHydro[i].Draw("5");
      if(Do0to10Gen || Do0to10All)      KorinnaC00[i].Draw("3");
      if(Do0to10Gen || Do0to10All)      KorinnaC00[i].Draw("lX");
      if(Do0to10Gen || Do0to10All)      KorinnaNoRecoilC00[i].Draw("3");
      if(Do0to10Gen || Do0to10All)      KorinnaNoRecoilC00[i].Draw("lX");
      if(Do0to10Gen || Do0to10All)      PyquenC00[i].Draw("3");
      if(Do0to10Gen || Do0to10All)      PyquenC00[i].Draw("lX");
      if(Do0to10Gen || Do0to10All)      PyquenWideRadC00[i].Draw("3");
      if(Do0to10Gen || Do0to10All)      PyquenWideRadC00[i].Draw("lX");
      if(Do0to10MC || Do0to10All)       LBTNoResponse[i].Draw("5");
      if(Do0to10MC || Do0to10All)       LBTResponse[i].Draw("5");
      if(Do10to30)   KorinnaC01[i].Draw("3");
      if(Do10to30)   KorinnaC01[i].Draw("lX");
      if(Do10to30)   KorinnaNoRecoilC01[i].Draw("3");
      if(Do10to30)   KorinnaNoRecoilC01[i].Draw("lX");
      if(Do10to30)   IvanC01[i].Draw("5");
      if(Do10to30)   MartiniC01[i].Draw("3");
      if(Do10to30)   MartiniC01[i].Draw("lX");
      if(Do30to50)   KorinnaC02[i].Draw("3");
      if(Do30to50)   KorinnaC02[i].Draw("lX");
      if(Do30to50)   KorinnaNoRecoilC02[i].Draw("3");
      if(Do30to50)   KorinnaNoRecoilC02[i].Draw("lX");
      if(Do30to50)   IvanC02[i].Draw("5");
      if(Do30to50)   MartiniC02[i].Draw("3");
      if(Do30to50)   MartiniC02[i].Draw("lX");
      if(Do50to90)   KorinnaC03[i].Draw("3");
      if(Do50to90)   KorinnaC03[i].Draw("lX");
      if(Do50to90)   KorinnaNoRecoilC03[i].Draw("3");
      if(Do50to90)   KorinnaNoRecoilC03[i].Draw("lX");
      if(OnlyData || (DoData && Do0to10))    HC0RRAASys[i].Draw("2");
      if(OnlyData || (DoData && Do10to30))   HC1RRAASys[i].Draw("2");
      if(OnlyData || (DoData && Do30to50))   HC2RRAASys[i].Draw("2");
      if(OnlyData || (DoData && Do50to90))   HC3RRAASys[i].Draw("2");
      if(OnlyData || (DoData && Do0to10))    HC0RRAA[i].Draw("p");
      if(OnlyData || (DoData && Do10to30))   HC1RRAA[i].Draw("p");
      if(OnlyData || (DoData && Do30to50))   HC2RRAA[i].Draw("p");
      if(OnlyData || (DoData && Do50to90))   HC3RRAA[i].Draw("p");
      GLine.Draw("l");
   }
   
   if(FourPanel)
   {
      P[0]->cd();
      Latex.SetTextAlign(11);
      if(Pres) Latex.DrawLatex(0.08, 0.88, "300 < p_{T}^{jet} < 400 GeV");
      else     Latex.DrawLatex(0.08, 0.9, "300 < p_{T}^{jet} < 400 GeV");
      if(Pres) Latex.SetTextSize(0.08);
      if(FourPanel && Pres) Latex.DrawLatex(0.08, 0.08, "#sqrt{s_{NN}} = 5.02 TeV");
      if(Pres) Latex.SetTextSize(0.09);
      
      P[1]->cd();
      Latex.SetTextAlign(11);
      if(Pres) Latex.DrawLatex(0.08, 0.88, "400 < p_{T}^{jet} < 500 GeV");
      else     Latex.DrawLatex(0.08, 0.9, "400 < p_{T}^{jet} < 500 GeV");
      
      P[2]->cd();
      Latex.SetTextAlign(11);
      if(Pres) Latex.DrawLatex(0.08, 0.88, "500 < p_{T}^{jet} < 1000 GeV");
      else     Latex.DrawLatex(0.08, 0.9, "500 < p_{T}^{jet} < 1000 GeV");
      
      P[3]->cd();
      HWorld.Draw("axis");
      Latex.SetTextAlign(11);
      if(Pres) Latex.DrawLatex(0.08, 0.88, "anti-k_{T}, |#eta_{jet}| < 2");
      else     Latex.DrawLatex(0.08, 0.9, "anti-k_{T}, |#eta_{jet}| < 2");
      Latex.SetTextFont(62);
      Latex.SetTextAlign(31);
      if(Pres)
      {
         if(Do0to10)   Latex.DrawLatex(0.95, 0.88, "0-10%");
         if(Do10to30)  Latex.DrawLatex(0.95, 0.88, "10-30%");
         if(Do30to50)  Latex.DrawLatex(0.95, 0.88, "30-50%");
         if(Do50to90)  Latex.DrawLatex(0.95, 0.88, "50-90%");
      }
      else
      {
         if(Do0to10)   Latex.DrawLatex(0.92, 0.9, "0-10%");
         if(Do10to30)  Latex.DrawLatex(0.92, 0.9, "10-30%");
         if(Do30to50)  Latex.DrawLatex(0.92, 0.9, "30-50%");
         if(Do50to90)  Latex.DrawLatex(0.92, 0.9, "50-90%");
      }
      Legend1.Draw();
   }
   else
   {
      P[0]->cd();
      Latex.SetTextAlign(11);
      if(Pres) Latex.DrawLatex(0.08, 0.88, "200 < p_{T}^{jet} < 250 GeV");
      else     Latex.DrawLatex(0.08, 0.9, "200 < p_{T}^{jet} < 250 GeV");
      
      P[1]->cd();
      Latex.SetTextAlign(11);
      if(Pres) Latex.DrawLatex(0.08, 0.88, "250 < p_{T}^{jet} < 300 GeV");
      else     Latex.DrawLatex(0.08, 0.9, "250 < p_{T}^{jet} < 300 GeV");
      
      P[2]->cd();
      Latex.SetTextAlign(11);
      if(Pres) Latex.DrawLatex(0.08, 0.88, "300 < p_{T}^{jet} < 400 GeV");
      else     Latex.DrawLatex(0.08, 0.9, "300 < p_{T}^{jet} < 400 GeV");
      
      P[3]->cd();
      Latex.SetTextAlign(11);
      if(Pres) Latex.DrawLatex(0.08, 0.88, "400 < p_{T}^{jet} < 500 GeV");
      else     Latex.DrawLatex(0.08, 0.9, "400 < p_{T}^{jet} < 500 GeV");
      
      P[4]->cd();
      Latex.SetTextAlign(11);
      if(Pres) Latex.DrawLatex(0.08, 0.88, "500 < p_{T}^{jet} < 1000 GeV");
      else     Latex.DrawLatex(0.08, 0.9, "500 < p_{T}^{jet} < 1000 GeV");
      
      P[5]->cd();
      HWorld.Draw("axis");
      Latex.SetTextAlign(11);
      if(Pres) Latex.DrawLatex(0.08, 0.88, "anti-k_{T}, |#eta_{jet}| < 2");
      else     Latex.DrawLatex(0.08, 0.9, "anti-k_{T}, |#eta_{jet}| < 2");
      Latex.SetTextFont(62);
      Latex.SetTextAlign(31);
      if(Pres)
      {
         if(Do0to10)   Latex.DrawLatex(0.95, 0.88, "0-10%");
         if(Do10to30)  Latex.DrawLatex(0.95, 0.88, "10-30%");
         if(Do30to50)  Latex.DrawLatex(0.95, 0.88, "30-50%");
         if(Do50to90)  Latex.DrawLatex(0.95, 0.88, "50-90%");
      }
      else
      {
         if(Do0to10)   Latex.DrawLatex(0.92, 0.9, "0-10%");
         if(Do10to30)  Latex.DrawLatex(0.92, 0.9, "10-30%");
         if(Do30to50)  Latex.DrawLatex(0.92, 0.9, "30-50%");
         if(Do50to90)  Latex.DrawLatex(0.92, 0.9, "50-90%");
      }
      Legend1.Draw();
   }

   string OutputBase = "";
   if(Pres)          OutputBase += "Pres_";
   else              OutputBase += "Paper_";
   if(Prelim)        OutputBase += "Prelim_";
   if(DoData)        OutputBase += "Data";
   if(Do0to10All)    OutputBase += "Theory0to10";
   if(Do0to10MC)     OutputBase += "MC0to10";
   if(Do0to10Calc)   OutputBase += "Calc0to10";
   if(Do0to10Gen)    OutputBase += "Gen0to10";
   if(Do10to30)      OutputBase += "Theory10to30";
   if(Do30to50)      OutputBase += "Theory30to50";
   if(Do50to90)      OutputBase += "Theory50to90";
   OutputBase += "RRAA";

   Canvas.SaveAs((OutputBase + ".pdf").c_str());
//   Canvas.SaveAs((OutputBase + ".png").c_str());
//   Canvas.SaveAs((OutputBase + ".C").c_str());
   
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

void SetAxis(TGaxis &A, bool Pres)
{
   A.SetLabelFont(42);
   if(Pres) A.SetLabelSize(0.040);
   else     A.SetLabelSize(0.030);
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
      if(PTAve < 250 && PTAve >= 200)    Index = 0;
      if(PTAve < 300 && PTAve >= 250)    Index = 1;
      if(PTAve < 400 && PTAve >= 300)    Index = 2;
      if(PTAve < 500 && PTAve >= 400)    Index = 3;
      if(PTAve < 1000 && PTAve >= 500)   Index = 4;

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




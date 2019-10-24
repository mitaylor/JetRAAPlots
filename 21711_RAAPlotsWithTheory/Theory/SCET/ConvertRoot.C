void ConvertRoot(TString InFileName)
{
   TFile InFile(InFileName.Data(),"READ");
   if(!InFile.IsOpen()) cout << "bad input" << endl;
   
   const int NRadii = 6;
   TH1D* HG18[NRadii];
   TH1D* HG22[NRadii];
   HG22[0] = (TH1D*) InFile.Get("RAA R = 0.2 g = 2.2");
   if(HG22[0] == NULL) cout << "bad histogram" << endl;
   HG18[0] = (TH1D*) InFile.Get("RAA R = 0.2 g = 1.8");
   HG22[2] = (TH1D*) InFile.Get("RAA R = 0.4 g = 2.2");
   HG18[2] = (TH1D*) InFile.Get("RAA R = 0.4 g = 1.8");
   HG22[3] = (TH1D*) InFile.Get("RAA R = 0.6 g = 2.2");
   HG18[3] = (TH1D*) InFile.Get("RAA R = 0.6 g = 1.8");
   HG22[4] = (TH1D*) InFile.Get("RAA R = 0.8 g = 2.2");
   HG18[4] = (TH1D*) InFile.Get("RAA R = 0.8 g = 1.8");
   HG22[5] = (TH1D*) InFile.Get("RAA R = 1.0 g = 2.2");
   HG18[5] = (TH1D*) InFile.Get("RAA R = 1.0 g = 1.8");
   
   ofstream OutFile;
   string OutFileBase = "SCET";
   OutFile.open((OutFileBase + "_RAA.txt").c_str());
   
   TAxis *axis = HG22[0]->GetXaxis();
   
   for(int iB = 1; iB <= axis->GetNbins(); iB++)
   {
      OutFile << axis->GetBinLowEdge(iB) << " ";
      OutFile << axis->GetBinUpEdge(iB) << " ";
      
      for(int iR = 0; iR < NRadii; iR++)
      {
         if (iR != 1)
         {
            double BinLow = HG22[iR]->GetBinContent(iB);
            double BinHigh = HG18[iR]->GetBinContent(iB);
            OutFile << (BinLow + BinHigh)/2 << " ";
            OutFile << BinHigh << " ";
            OutFile << BinLow << " ";
         }
         else
            OutFile << "0 0 0 ";
      }
         
      OutFile << endl;
   }
   
   OutFile.close();
   
   OutFile.open((OutFileBase + "_RRAA.txt").c_str());
   
//   for(int iB = 1; iB <= axis->GetNbins(); iB++)
//   {
//      OutFile << axis->GetBinLowEdge(iB) << " ";
//      OutFile << axis->GetBinUpEdge(iB) << " ";
//
//      for(int iR = 0; iR < NRadii; iR++)
//      {
//         if (iR != 1 && iR != 0)
//         {
//            double ThisBinLow = HG22[iR]->GetBinContent(iB);
//            double ThisBinHigh = HG18[iR]->GetBinError(iB);
//            double R02BinLow = HG22[0]->GetBinContent(iB);
//            double R02BinHigh = HG18[0]->GetBinError(iB);
//
//            double ThisRAAValue = (ThisBinLow + ThisBinHigh)/2;
//            double ThisRAARelError = (ThisBinLow-ThisBinHigh)/2/ThisRAAValue;
//            double R02RAAValue = (R02BinLow + R02BinHigh)/2;
//            double R02RAARelError = (R02BinLow-R02BinHigh)/2/R02RAAValue;
//            double RRAAValue = ThisRAAValue/R02RAAValue;
//            double RRAAError = sqrt(pow(ThisRAARelError,2) + pow(R02RAARelError,2))*RRAAValue;
//
//            OutFile << RRAAValue << " ";
//            OutFile << RRAAValue + RRAAError << " ";
//            OutFile << RRAAValue - RRAAError << " ";
//         }
//         else
//            OutFile << "1 1 1 ";
//      }
   
   // 200, 250, 300, 400, 500, 1000
   vector<double> BinCenter = {225, 275, 350, 450, 750};
   vector<double> Bins = {200, 250, 300, 400, 500, 1000};
   
   for(int iB = 0; iB < BinCenter.size(); iB++)
   {
      OutFile << Bins[iB] << " ";
      OutFile << Bins[iB+1] << " ";
      
      for(int iR = 0; iR < NRadii; iR++)
      {
         if (iR != 1 && iR != 0)
         {
            double ThisBinLow = HG22[iR]->Interpolate(BinCenter[iB]);
            double ThisBinHigh = HG18[iR]->Interpolate(BinCenter[iB]);
            double R02BinLow = HG22[0]->Interpolate(BinCenter[iB]);
            double R02BinHigh = HG18[0]->Interpolate(BinCenter[iB]);
            
            double ThisRAAValue = (ThisBinLow + ThisBinHigh)/2;
            double ThisRAARelError = (ThisBinLow-ThisBinHigh)/2/ThisRAAValue;
            double R02RAAValue = (R02BinLow + R02BinHigh)/2;
            double R02RAARelError = (R02BinLow-R02BinHigh)/2/R02RAAValue;
            double RRAAValue = ThisRAAValue/R02RAAValue;
            double RRAAError = sqrt(pow(ThisRAARelError,2) + pow(R02RAARelError,2))*RRAAValue;
            
            OutFile << RRAAValue << " ";
            OutFile << RRAAValue + RRAAError << " ";
            OutFile << RRAAValue - RRAAError << " ";
         }
         else
            OutFile << "1 1 1 ";
      }
         
      OutFile << endl;
   }
   
   OutFile.close();
}



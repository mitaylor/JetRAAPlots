#include <iostream>
#include <fstream>
#include <string>

using namespace std;

// Assuming all three files have identical format.
int main(int argc, char *argv[])
{
   ifstream InFilePT200("bin_1_RAA_R_over_RAA_02.dat");
   ifstream InFilePT250("bin_2_RAA_R_over_RAA_02.dat");
   ifstream InFilePT300("bin_3_RAA_R_over_RAA_02.dat");
   ifstream InFilePT400("bin_4_RAA_R_over_RAA_02.dat");
   ifstream InFilePT500("bin_5_RAA_R_over_RAA_02.dat");
   
   ofstream OutFile("RRAA.txt");

   // Get the first three lines, which are comments.
   const int NBin = 5;
   const int NRad = 6;
   
   double RRAA[NRad][NBin] = {0};
   
   while(InFilePT200)
   {
      int iR;
      double R = 0;
      InFilePT200 >> R;
      InFilePT250 >> R;
      InFilePT300 >> R;
      InFilePT400 >> R;
      InFilePT500 >> R;
      
      if(R == 0.2)      iR = 0;
      else if(R == 0.4) iR = 2;
      else if(R == 0.6) iR = 3;
      else if(R == 0.8) iR = 4;
      else if(R == 1.0) iR = 5;
      else continue;
      
      InFilePT200 >> RRAA[iR][0];
      InFilePT250 >> RRAA[iR][1];
      InFilePT300 >> RRAA[iR][2];
      InFilePT400 >> RRAA[iR][3];
      InFilePT500 >> RRAA[iR][4];
   }

   double bins[NBin] = {200, 250, 300, 400, 500};
   for(int iB = 0; iB < NBin; iB++)
   {
      OutFile << bins[iB] << " " << bins[iB] << " ";
      
      for(int iR = 0; iR < NRad; iR++)
      {
         OutFile << RRAA[iR][iB] << " " << RRAA[iR][iB] << " " << RRAA[iR][iB] << " ";
      }
      
      OutFile << endl;
   }
   
   InFilePT200.close();
   InFilePT250.close();
   InFilePT300.close();
   InFilePT400.close();
   InFilePT500.close();
   OutFile.close();
   
   return 0;
}

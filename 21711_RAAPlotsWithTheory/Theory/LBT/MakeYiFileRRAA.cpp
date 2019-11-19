#include <iostream>
#include <fstream>
#include <string>

using namespace std;

// Assuming all three files have identical format.
int main(int argc, char *argv[])
{
   ifstream InFilePT200("RAA_0010_pT200.txt");
   ifstream InFilePT250("RAA_0010_pT250.txt");
   ifstream InFilePT300("RAA_0010_pT300.txt");
   ifstream InFilePT400("RAA_0010_pT400.txt");
   ifstream InFilePT500("RAA_0010_pT500.txt");
   
   ofstream OutFileResponse("RRAA_response.txt");
   ofstream OutFileNoResponse("RRAA_showers_only.txt");

   int Count = 0;
   
   // Get the first three lines, which are comments.
   string line;
   getline(InFilePT200, line);
   getline(InFilePT250, line);
   getline(InFilePT300, line);
   getline(InFilePT400, line);
   getline(InFilePT500, line);
   
   const int NBin = 5;
   const int NRad = 6;
   
   double RRAA[2][NRad][NBin] = {0};
   double RRAAError[2][NRad][NBin] = {0};
   
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
         
      InFilePT200 >> RRAA[0][iR][0] >> RRAAError[0][iR][0] >> RRAA[1][iR][0] >> RRAAError[1][iR][0];
      InFilePT250 >> RRAA[0][iR][1] >> RRAAError[0][iR][1] >> RRAA[1][iR][1] >> RRAAError[1][iR][1];
      InFilePT300 >> RRAA[0][iR][2] >> RRAAError[0][iR][2] >> RRAA[1][iR][2] >> RRAAError[1][iR][2];
      InFilePT400 >> RRAA[0][iR][3] >> RRAAError[0][iR][3] >> RRAA[1][iR][3] >> RRAAError[1][iR][3];
      InFilePT500 >> RRAA[0][iR][4] >> RRAAError[0][iR][4] >> RRAA[1][iR][4] >> RRAAError[1][iR][4];
   }

   double bins[NBin] = {200, 250, 300, 400, 500};
   for(int iB = 0; iB < NBin; iB++)
   {
      OutFileResponse << bins[iB] << " " << bins[iB] << " ";
      
      for(int iR = 0; iR < NRad; iR++)
      {
         OutFileResponse << RRAA[1][iR][iB] << " " << RRAA[1][iR][iB]+RRAAError[0][iR][iB] << " " << RRAA[1][iR][iB]-RRAAError[1][iR][iB] << " ";
      }
      
      OutFileResponse << endl;
   }
   
   for(int iB = 0; iB < NBin; iB++)
   {
      OutFileNoResponse << bins[iB] << " " << bins[iB] << " ";
      
      for(int iR = 0; iR < NRad; iR++)
      {
         OutFileNoResponse << RRAA[0][iR][iB] << " " << RRAA[0][iR][iB]+RRAAError[0][iR][iB] << " " << RRAA[0][iR][iB]-RRAAError[0][iR][iB] << " ";
      }
      
      OutFileNoResponse << endl;
   }
   
   InFilePT200.close();
   InFilePT250.close();
   InFilePT300.close();
   InFilePT400.close();
   InFilePT500.close();
   OutFileResponse.close();
   OutFileNoResponse.close();
   
   return 0;
}

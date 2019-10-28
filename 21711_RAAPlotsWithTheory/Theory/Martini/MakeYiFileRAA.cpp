#include <iostream>
#include <fstream>
#include <string>

using namespace std;

// Assuming all three files have identical format.
int main(int argc, char *argv[])
{
   if (argc != 3)
   {
      cout << "Input Error: Need input file + output file name." << endl;
      return -1;
   }

   ifstream InFile(argv[1]);
   ofstream OutFile(argv[2]);
   
   if(!InFile.is_open()) {
      cout << "Input Error: Invalid input file." << endl;
      return -1;
   }
   if(!OutFile.is_open()) {
      cout << "Output Error: Output file not open." << endl;
      return -1;
   }

   int Count = 0;
   
   // Get the first three lines, which are comments.
   string line;
   getline(InFile, line);
   getline(InFile, line);
   getline(InFile, line);
   
   const int NRad = 6;

   while(InFile)
   {
      double PT = 0;
      double PTMin = 0;
      double PTMax = 0;
      InFile >> PT >> PTMin >> PTMax;

      if(PT < 1)
         continue;

      double RAA[NRad], RAAError[NRad];
      for(int i = 0; i < NRad; i++)
      {
         InFile >> RAA[i] >> RAAError[i];
      }
      
      OutFile << PTMin << " " << PTMax << " ";
      for(int i = 0; i < NRad; i++)
      {
         OutFile << RAA[i] << " " << RAA[i] + RAAError[i] << " " << RAA[i] - RAAError[i] << " ";
      }
      
      OutFile << endl;
   }

   InFile.close();
   OutFile.close();
   
   return 0;
}

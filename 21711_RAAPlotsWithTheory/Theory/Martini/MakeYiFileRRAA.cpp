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
   const int NBins = 5;
   double RAA[NBins][NRad];
   double RAAError[NBins][NRad];
   double bins[NBins+1] = {200, 250, 300, 400, 500, 1000};

   for(int j = 0; j < NRad; j++)
   {
      double rad;

      InFile >> rad;

      for(int i = 0; i < NBins; i++)
      {
         InFile >> RAA[i][j] >> RAAError[i][j];
      }
   }
   
   for(int i = 0; i < NBins; i++)
   {
      OutFile << bins[i] << " " << bins[i+1] << " ";
      for(int j = 0; j < NRad; j++)
      {
         OutFile << RAA[i][j] << " " << RAA[i][j] + RAAError[i][j] << " " << RAA[i][j] - RAAError[i][j] << " ";
      }
      
      OutFile << endl;
   }

   InFile.close();
   OutFile.close();
   
   return 0;
}

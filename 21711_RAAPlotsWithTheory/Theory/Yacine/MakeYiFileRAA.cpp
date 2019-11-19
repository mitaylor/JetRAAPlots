#include <iostream>
#include <fstream>
#include <string>

using namespace std;

// Assuming all three files have identical format.
int main(int argc, char *argv[])
{
   ifstream InFileR02("RAA_02.dat");
   ifstream InFileR04("RAA_04.dat");
   ifstream InFileR06("RAA_06.dat");
   ifstream InFileR08("RAA_08.dat");
   ifstream InFileR10("RAA_1.dat");
   
   ofstream OutFile("RAA.txt");
   
   const int NRad = 6;

   while(InFileR02)
   {
      double PT = 0;
      double RAA02;
      double RAA04;
      double RAA06;
      double RAA08;
      double RAA10;
      
      InFileR02 >> PT >> RAA02;
      InFileR04 >> PT >> RAA04;
      InFileR06 >> PT >> RAA08;
      InFileR08 >> PT >> RAA08;
      InFileR10 >> PT >> RAA10;
      if(PT < 1) continue;
      
      OutFile << PT << " " << PT << " ";
      
      OutFile << RAA02 << " " << RAA02 << " " << RAA02 << " ";
      OutFile << "0 0 0 ";
      OutFile << RAA04 << " " << RAA04 << " " << RAA04 << " ";
      OutFile << RAA06 << " " << RAA06 << " " << RAA06 << " ";
      OutFile << RAA08 << " " << RAA08 << " " << RAA08 << " ";
      OutFile << RAA10 << " " << RAA10 << " " << RAA10 << " ";
      OutFile << endl;
   }

   InFileR02.close();
   InFileR04.close();
   InFileR06.close();
   InFileR08.close();
   InFileR10.close();
   OutFile.close();
   
   return 0;
}

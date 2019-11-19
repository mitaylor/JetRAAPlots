#include <iostream>
#include <fstream>
#include <string>

using namespace std;

// Assuming all three files have identical format.
int main(int argc, char *argv[])
{
   ifstream InFileR02("RAA_0010_R2.txt");
   ifstream InFileR04("RAA_0010_R4.txt");
   ifstream InFileR06("RAA_0010_R6.txt");
   ifstream InFileR08("RAA_0010_R8.txt");
   ifstream InFileR10("RAA_0010_R10.txt");
   
   ofstream OutFileShowersOnly("RAA_showers_only.txt");
   ofstream OutFileWithRecoil("RAA_with_recoil.txt");
   ofstream OutFileRecoilReaction("RAA_with_recoil_and_back_reaction.txt");

   int Count = 0;
   
   // Get the first three lines, which are comments.
   string line;
   getline(InFileR02, line);
   getline(InFileR04, line);
   getline(InFileR06, line);
   getline(InFileR08, line);
   getline(InFileR10, line);
   
   const int NRad = 6;

   while(InFileR02)
   {
      double PT = 0;
      InFileR02 >> PT;
      InFileR04 >> PT;
      InFileR06 >> PT;
      InFileR08 >> PT;
      InFileR10 >> PT;
      if(PT < 1) continue;
      
      OutFileShowersOnly << PT << " " << PT << " ";
      OutFileWithRecoil << PT << " " << PT << " ";
      OutFileRecoilReaction << PT << " " << PT << " ";

      double RAA02, RAAError02;
      double RAA04, RAAError04;
      double RAA06, RAAError06;
      double RAA08, RAAError08;
      double RAA10, RAAError10;
      
      InFileR02 >> RAA02 >> RAAError02;
      InFileR04 >> RAA04 >> RAAError04;
      InFileR06 >> RAA06 >> RAAError06;
      InFileR08 >> RAA08 >> RAAError08;
      InFileR10 >> RAA10 >> RAAError10;
      
      OutFileShowersOnly << RAA02 << " " << RAA02+RAAError02 << " " << RAA02-RAAError02 << " ";
      OutFileShowersOnly << "0 0 0 ";
      OutFileShowersOnly << RAA04 << " " << RAA04+RAAError04 << " " << RAA04-RAAError04 << " ";
      OutFileShowersOnly << RAA06 << " " << RAA06+RAAError06 << " " << RAA06-RAAError06 << " ";
      OutFileShowersOnly << RAA08 << " " << RAA08+RAAError08 << " " << RAA08-RAAError08 << " ";
      OutFileShowersOnly << RAA10 << " " << RAA10+RAAError10 << " " << RAA10-RAAError10 << " ";
      OutFileShowersOnly << endl;
      
      InFileR02 >> RAA02 >> RAAError02;
      InFileR04 >> RAA04 >> RAAError04;
      InFileR06 >> RAA06 >> RAAError06;
      InFileR08 >> RAA08 >> RAAError08;
      InFileR10 >> RAA10 >> RAAError10;
      
      OutFileWithRecoil << RAA02 << " " << RAA02+RAAError02 << " " << RAA02-RAAError02 << " ";
      OutFileWithRecoil << "0 0 0 ";
      OutFileWithRecoil << RAA04 << " " << RAA04+RAAError04 << " " << RAA04-RAAError04 << " ";
      OutFileWithRecoil << RAA06 << " " << RAA06+RAAError06 << " " << RAA06-RAAError06 << " ";
      OutFileWithRecoil << RAA08 << " " << RAA08+RAAError08 << " " << RAA08-RAAError08 << " ";
      OutFileWithRecoil << RAA10 << " " << RAA10+RAAError10 << " " << RAA10-RAAError10 << " ";
      OutFileWithRecoil << endl;
      
      InFileR02 >> RAA02 >> RAAError02;
      InFileR04 >> RAA04 >> RAAError04;
      InFileR06 >> RAA06 >> RAAError06;
      InFileR08 >> RAA08 >> RAAError08;
      InFileR10 >> RAA10 >> RAAError10;
      
      OutFileRecoilReaction << RAA02 << " " << RAA02+RAAError02 << " " << RAA02-RAAError02 << " ";
      OutFileRecoilReaction << "0 0 0 ";
      OutFileRecoilReaction << RAA04 << " " << RAA04+RAAError04 << " " << RAA04-RAAError04 << " ";
      OutFileRecoilReaction << RAA06 << " " << RAA06+RAAError06 << " " << RAA06-RAAError06 << " ";
      OutFileRecoilReaction << RAA08 << " " << RAA08+RAAError08 << " " << RAA08-RAAError08 << " ";
      OutFileRecoilReaction << RAA10 << " " << RAA10+RAAError10 << " " << RAA10-RAAError10 << " ";
      OutFileRecoilReaction << endl;
   }

   InFileR02.close();
   InFileR04.close();
   InFileR06.close();
   InFileR08.close();
   InFileR10.close();
   OutFileShowersOnly.close();
   OutFileWithRecoil.close();
   OutFileRecoilReaction.close();
   
   return 0;
}

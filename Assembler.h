#ifndef _ASSEMBLER_H
#define _ASSEMBLER_H
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <bitset> 
#include <algorithm>
#include <fstream>
#include "FileExceptions.cpp"

class Assembler
{
	private:
		struct instruction 
		{
			string key;
			string value;
			bool immediateAddressing;
			bool supportsImmediateAddressing;
		};

		instruction* instructionSet;

		struct Variable
		{
			string name;
			int value;
			int address;//line where is stored
		};
		
		string curFile;
		vector<string>* lines;
		vector<string>* mc;
		vector<Variable*>* vars;
		
		int start;
		int end;
		
		void setInstructionSet();
		void readLines();
		void parseVariables();
		void linesIntoMC();
		void saveMC();
		int isCharPresent(string line, char toFind);
		void removeEndStartTokens();
		
		
		string decToBin(int dec, int size);
		string negateBin(string bin);
		string binAdd(string bin1, string bin2);
	public:
		Assembler();
		~Assembler();		
		Assembler(string filepath);
		vector<string> getLines();
		vector<string> getMC();
		void printVars();
		void setFile(string filepath);
		string getFile();
		//public only for testing
		
	
	
};
#endif

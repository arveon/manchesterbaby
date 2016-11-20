#ifndef _ASSEMBLER_H
#define _ASSEMBLER_H

#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <sstream>
#include <iterator>
#include <fstream>

#include "FileExceptions.cpp"

class Assembler
{
	private:
		const int INSTRUCTION_NUM = 10; // amount of instructions
		const string INSTRUCTION_SOURCE ="instructions.txt"; // file from which to read the instructions
		const int STANDARD_VAR_SIZE = 2147483647; // the maximum bit size for a standardly declared variable
		const int IMMEDIATE_VAR_SIZE = 65535; // the maximum bit size for an immediately declared variable
		const string LINE_OF_ZEROES = "00000000000000000000000000000000"; // for printing out a VAR 0 at the beginning of the output

		struct instruction // struct to store an instruction
		{
			string key; // the name of the instruction
			string value; // the binary value of the instruction
		};

		struct Variable // struct to store an assembly program variable
		{
			string name; // the name of the variable
			int value; // the decimal value of the variable
			int address; // line at which the variable is declared in the assembly program
		};
		
		string curFile;
		vector<string>* lines;
		vector<string>* mc;
		vector<Variable*>* vars;
		instruction* instructionSet;
		
		int start;
		int end;
		
		void setInstructionSet(); // loads the instructions from the file and stores them in an array
		void readLines();
		void parseVariables();
		void linesIntoMC(); // reads the first loop output into a second loop to create the machine code
		void saveMC(); // saves the machine code to an output file
		int isCharPresent(string line, char toFind);
		void removeEndStartTokens();
		string decToBin(int dec, int size); // converts a decimal number to a binary value
		string negateBin(string bin); // converts a binary number to a negative binary value
		string binAdd(string bin1, string bin2); // method used by the negateBin method to negate a binary value
	
	public:
		Assembler(); // constructor
		~Assembler(); // destructor	
		Assembler(string filepath); // constructor with a filepath to read the assembly program from
		vector<string> getLines();
		vector<string> getMC();
		void printVars();
		void setFile(string filepath);
		//public only for testing
};
#endif

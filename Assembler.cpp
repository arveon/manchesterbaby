#include "Assembler.h"

using namespace std;

// constructor
Assembler::Assembler()
{
	curFile="";
	lines = new vector<string>;
	mc = new vector<string>;
	vars = new vector<Variable*>;
	setInstructionSet();
}

// constructor with a filepath to read the assembly program from
Assembler::Assembler(string filepath)
{
	curFile = filepath;
	lines = new vector<string>;
	mc = new vector<string>;
	readLines(); // go through each line of the assembly program, remove the comments and End/Start tokens as well as separate the variables into another vector
	parseVariables();
	removeEndStartTokens();
	linesIntoMC(); // create the machine code using the initial output from the assembler program
	saveMC(); // once the machine code is correct, print it to the output file
}

// destructor
Assembler::~Assembler()
{
	delete lines;
	delete mc;
	for(int i = 0; i < vars->size(); i++)
	{
		delete vars->at(i);
	}
	delete vars;
	delete[] instructionSet;
}

void Assembler::setFile(string filepath)
{
	curFile = filepath;
	readLines();
	parseVariables();
	removeEndStartTokens();
	linesIntoMC();
	saveMC();
}

// reads lines of assembler from the file and stores them in line vector
// strips the code of comments and parses variable names into addresses
void Assembler::readLines()
{
	cout << "\033[0;37mReading assembler lines...\033[0m" << endl;
	if(curFile=="")//will throw a file not set exception if file path is null
		throw FileNotSetException();
		
	ifstream file;
	file.open(curFile);
	if(!file.good() && !file.is_open())
		throw OpenFileException(curFile);
		
	string line;
	int file_line_num = 1;
	while(getline(file,line))
	{
		// form a line of assembler code that will be put in vector
		// need to stop reading line after ;
		// need to skip all the spaces
		stringstream newline;
		bool linestarted = false;
		for(int i = 0; i < line.length(); i++)
		{
			// if endline symbol reached, stop reading the line
			if(line[i]==';')
				break;
			
			if(i==line.length()-1 && line[i]!=';')
				throw LineNotTerminatedException(file_line_num, newline.str(), curFile);
			
			if(!linestarted && line[i]==' ')
			{// if line hasn't started and character is space, skip it
				continue;
			}
			else if(linestarted && line[i]==' ')
			{// if there were symbols before this space, insert one space and stop saving spaces
				newline << line[i];
				linestarted = false;
			}
			else
			{// if not space, just store it in the stream and start saving spaces
				newline << line[i];
				linestarted = true;
			}	
		}
		// if there was something in the line, store it
		if(newline.str().length()>0)
			lines->push_back(newline.str());
			
		file_line_num++;
	} 
	file.close();
	cout << "\033[1;32mAssembler lines read successfully!\033[0m" << endl;
}

void Assembler::parseVariables()
{
	cout << "\033[0;37mParsing variable names, values and addresses...\033[0m" << endl;
	int counter = 0;
	for(int i = 0; i < lines->size(); i++)
	{
		string s_val;
		Variable* temp_v = new Variable;
		
		string temp = lines->at(i);
		
		if(temp.find(":") != string::npos && temp.find(" VAR ") != string::npos)
		{
			temp.replace(temp.find(" VAR "), 5, "");
			stringstream buffer(temp);
			
			getline(buffer, temp_v->name, ':');
			try
			{
				string s_value;
				getline(buffer, s_value, ':');
				temp_v->value = stoi(s_value);

				if(temp_v->value <= -(STANDARD_VAR_SIZE) || temp_v->value >= STANDARD_VAR_SIZE)
					throw out_of_range("asd");
			}
			catch(invalid_argument& e)
			{
				throw InvalidVariableDeclarationException(curFile, temp, temp_v->name);
			}
			catch(out_of_range& e1)
			{
				throw VariableOutOfRangeException(curFile, temp, temp_v->name);
			}
			temp_v->address = counter;
			vars->push_back(temp_v);
			
			//remove variable declaration from lines
			lines->erase(lines->begin()+i);
			i--;
		}
		counter++;
	}
	cout << "\033[1;32mVariable names, addresses and values successfully passed!\033[0m" << endl;
}

void Assembler::removeEndStartTokens()
{
	cout << "\033[0;37mParsing start and end tokens...\033[0m" << endl;
	bool startfound = false;
	bool endfound = false;
	int counter = 0;
	for(int i = 0; i < lines->size(); i++)
	{
		int temp_end = lines->at(counter).find("END: ");
		int temp_start = lines->at(counter).find("START: ");
		
		if(temp_end != string::npos)
		{
			lines->at(counter) = lines->at(counter).replace(temp_end, 5, "");
			end = counter;
			startfound = true;
		}
		else if (temp_start != string::npos)
		{
			lines->at(counter) = lines->at(counter).replace(temp_start, 7, "");
			start = counter;
			endfound = true;
		}
		counter++;
	}
	
	try
	{
		if(!startfound || !endfound)
			throw StartOrEndNotFoundException(curFile);
		else
			cout << "\033[1;32mstart and end tokens parsed successfully!\033[0m" << endl; 
	}
	catch(StartOrEndNotFoundException ex1)
	{
		cout << ex1.message() << endl;
		cout << "\033[1;34mFailed to parse start and end tokens!\033[0m" << endl;
	}
}

vector<string> Assembler::getLines()
{
	return *lines;
}

vector<string> Assembler::getMC()
{
	return *mc;
}

void Assembler::printVars()
{
	for(int i = 0; i < vars->size(); i++)
	{
		cout << vars->at(i)->name << " = " << vars->at(i)->value << " at: " << vars->at(i)->address << endl;
	}
}

int Assembler::isCharPresent(string line, char toFind)
{
	for(int i = 0; i < line.length();i++)
	{
		if(line[i] == toFind)
		{
			return i;
		}
	}
	return -1;
}

// loads the instructions from the file and stores them in an array
void Assembler::setInstructionSet()
{
	cout << "\033[0;37mLoading possible command properties...\033[0m" << endl;
	instructionSet = new instruction[INSTRUCTION_NUM];
	string name, bitValue;
	ifstream instructionFile;
 	instructionFile.open (INSTRUCTION_SOURCE);
 	int i = 0;
	while (instructionFile >> name >> bitValue)
	{
	    instructionSet[i].key = name;
	    instructionSet[i].value = bitValue;
	    i++;
	}
 	instructionFile.close();
 	cout << "\033[1;32mCommand properties successfully loaded!\033[0m" << endl; 
}

// reads the first assembler output (lines and vars vectors) into a second method to create the machine code
void Assembler::linesIntoMC()
{
	bool varZeroMissing = false; // tracks whether the necessary line of 0s at the beginning of the machine code has been created using a VAR 0
	bool stopConfirmed = false; // tracks whether the STP keyword has been encountered

	for(int i=0; i<lines->size(); i++) //beginning of main loop that goes through the inital assembler program output, the lines vector
	{
		if(lines->at(i).find("VAR 0") != string::npos) // add row of 0s where the VAR 0 should be declared
		{
			mc->push_back(LINE_OF_ZEROES); // add the 0s to the vector
		}
		else
		{
			if(lines->at(0).find("VAR 0") == string::npos)
			{
				varZeroMissing = true;
			}

			// look for instruction on the current line
			bool found = false; // keep track of whether an instruction has been found on the current line
			for(int j=0; j<INSTRUCTION_NUM; j++) // loop through all the instructions available in the array
			{
				if(lines->at(i).find(instructionSet[j].key) != string::npos) // if an instruction is found on the current line
				{
					found = true;
					mc->push_back(instructionSet[j].value); // add the binary instruction opcode to the machine code vector
					break;
				}
			}
			if(!found) // if none is found, output an error
			{
				throw CommandNotRecognisedException(curFile, lines->at(i)); 
			}

			//put the operands before each command
			for(int j=0; j<vars->size(); j++) // loop through each variable
			{
				if(lines->at(i).find(vars->at(j)->name) != string::npos) // if a variable name is found on the current name
				{
					int decimalValue = vars->at(j)->address; // store the variable address in an integer
					if(varZeroMissing)
					{
						decimalValue++;
					}
					string binaryValue = decToBin(decimalValue, 13); // convert the integer to a binary value of 13 bits
					mc->at(i) = binaryValue + mc->at(i); // place the binary operand at the beginning of the line, before the opcode
				}	
			}

			//check for immediately declared variables and store them in the machine code
			int immediateVariable; // variable to store the variable in
			try
			{
				bool negative = false; // keeps track whether the variable is negative or not
				string lineToCheck = lines->at(i); // string to store the vector string to test
				istringstream split(lineToCheck); // create an istringstream object
				vector<string> tokens; // create a vector to store each element of the string
				copy(istream_iterator<string>(split), istream_iterator<string>(), back_inserter(tokens)); // split the string and store in tokens vector
				immediateVariable = stoi(tokens.at(1)); // store the possible integer that's in the second position of the string that's been split
				
				if(immediateVariable <= -(IMMEDIATE_VAR_SIZE) || immediateVariable >= IMMEDIATE_VAR_SIZE) // if the variable exceeds the immediate variable size
				{
					throw "a"; // throw an exception
				}
				
				if(immediateVariable < 0) // if the variable is negative, keep track of it so that we can indicate this fact in the machine code later
				{
					negative = true;
					immediateVariable = abs(immediateVariable); // make the variable positive so that we can convert it to binary first
				}	
				string binaryValue = decToBin(immediateVariable, 16); // convert the variable to a binary value of 16 bits
				
				// add the first part of the immediate variable, before the opcode
				string firstHalf;
				for(int x=0; x<13; x++) // take the first 13 bits of the binary variable and store them in a string
				{
					firstHalf+= binaryValue[x];
				}
				mc->at(i) = firstHalf + mc->at(i); // add the string before the opcode in the machine code line

				// add second part of the immediate variable, after the opcode
				int afterOpcode = 12;
				string secondHalf;
				for(int x=0; x<3; x++) // take the last 3 bits of the binary variable and store them in a string
				{
					secondHalf+=binaryValue[afterOpcode];
					afterOpcode++;
				} 
				int counter = 0;
				for(int x=17; x<20; x++) // add the string right after the opcode
				{
					mc->at(i)[x] = secondHalf[counter]; 
					counter++;
				}

				// add a 1 after the second part of the binary variable if the variable was negative
				if(negative)
				{
					mc->at(i)[20] = '1'; // replaces a 0 with a 1
				}

				// leave a 1 at the end to show that it's an immediate variable
				mc->at(i)[31] = '1'; // replaces a 0 with a 1
			}
			catch(invalid_argument& e)
			{
			}
			// if the number is too big to be an integer
			catch(out_of_range& e1)
			{
				if(lines->at(i).find("STP")==string::npos) // if there wasn't a STP on the line
				{
					throw VariableOutOfRangeException(curFile, lines->at(i), "that was immediately declared"); // prints error message
				}
			}
			// if the number is bigger than 16 bits
			catch(const char* a)
			{
				throw VariableOutOfRangeException(curFile, lines->at(i), to_string(immediateVariable)); // prints error message
			}

			// looks for STP command
			if(lines->at(i).find("STP") != string::npos) // if the current line contains STP
			{
				stopConfirmed = true;
			}
		}
	} // end of main loop that was going through the lines vector

	if(lines->at(0).find("VAR 0") == string::npos) // if there wasn't a VAR 0 at the beginning, manually add a row of 0s because it's necessary for the machester baby
	{
		mc->insert(mc->begin(), LINE_OF_ZEROES);

		// add the 0s at the beginning of the machine code vector
		try // print a warning message too since no VAR 0 was in the assembly program
		{
			throw VarZeroNotFoundException(curFile);
		}
		catch(VarZeroNotFoundException ex1)
		{
			cout << ex1.message() << endl;
			cout << "\033[1;34mAdded a VAR 0 automatically!\033[0m" << endl;
		}
	}

	// if no STP keyword was found
	if(!stopConfirmed)
	{
		throw StopCommandNotFoundException(curFile); // prints an error message as the STP command is necessary to end the program
	}
	
	// add the binary values of the variables at the end of the output
	for(int j=0; j<vars->size(); j++) // loops through the vars vector
	{
		bool negative = false; // keeps track whether the variable is negative or not
		int decimalValue = vars->at(j)->value; 
		if(decimalValue < 0) // if the variable is negative
		{
			negative = true;
			decimalValue = abs(decimalValue); // make the variable positive so that we can convert it to binary first
		}		
		string binaryValue = decToBin(decimalValue, 32); // convert the variable to a binary value of 32 bits
		if(negative) // if it was negative
		{
			binaryValue = negateBin(binaryValue); // negate the binary value
		}
		mc->push_back(binaryValue); // add the binary value to the end of the machine code
	}
}

// converts a decimal number to a binary value
string Assembler::decToBin(int dec, int size) 
{
	string bin;
	
	for(int i=0; i<size; i++) 
	{
		bin += to_string(dec%2);
		dec/=2;
	}
	return bin;
}

// converts a binary number to a negative binary value
string Assembler::negateBin(string bin) 
{
	string temp;
	string binCopy;
	binCopy=bin;
	for(int i=0; i<16; i++) 
	{
		if(bin[i]=='0') binCopy[i] = '1';
		else binCopy[i] = '0';
		temp+='0';
	}
	temp[0]='1';

	string result = binAdd(binCopy, temp);
	return result;
}

// method used by the negateBin method to negate a binary value
string Assembler::binAdd(string bin1, string bin2) 
{
	string result;
	for(int i=0; i<32; i++) result+='0';
	char temp = '0';
	for(int i=0; i<32; i++) 
	{
		if(bin1[i]=='0') 
		{
			if(temp=='1') 
			{
				if(bin2[i]=='0') 
				{
					result[i] = '1';
					temp = '0';
				} 
				else 
				{
					result[i] = '0';
				}
			}
			else 
			{
				if(bin2[i]=='0') 
				{
					result[i] = '0';
				} else 
				{
					result[i] = '1';
				}
			}
		} 
		else 
		{
			if(temp=='1') 
			{
				if(bin2[i]=='0') 
				{
					result[i] = '0';
				} 
				else 
				{
					result[i] = '1';
				}
			}
			else 
			{
				if(bin2[i]=='0') 
				{
					result[i] = '1';
				} 
				else 
				{
					result[i] = '0';
					temp='1';
				}
			}
		}

	}
	return result;
}

// saves the machine code to an output file
void Assembler::saveMC()
{
	ofstream myfile; // create an ofstream object
 	myfile.open ("output"); // machine code is read to an output file
	for(int i = 0; i < mc->size(); i++)
	{
		myfile << mc->at(i) << endl; // print the vector contents
	}
 	myfile.close(); // close the file once it's finished
}

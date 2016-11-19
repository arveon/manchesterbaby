#include "Assembler.h"

//sponsored by sad music .com  https://www.youtube.com/watch?v=57iOgKv2xRM

using namespace std;
Assembler::Assembler()
{
	curFile="";
	lines = new vector<string>;
	mc = new vector<string>;
	vars = new vector<Variable*>;
	setInstructionSet();
}

Assembler::Assembler(string filepath)
{
	curFile = filepath;
	lines = new vector<string>;
	mc = new vector<string>;
	readLines();
	parseVariables();
	removeEndStartTokens();
	linesIntoMC();
	saveMC();
}

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

string Assembler::getFile()
{
	return curFile;
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

//reads lines of assembler from the file and stores them in line vector
//strips the code of comments and parses variable names into addresses
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
		//form a line of assembler code that will be put in vector
		//need to stop reading line after ;
		//need to skip all the spaces
		stringstream newline;
		bool linestarted = false;
		for(int i = 0; i < line.length(); i++)
		{
			//if endline symbol reached, stop reading the line
			if(line[i]==';')
				break;
			
			if(i==line.length()-1 && line[i]!=';')
				throw LineNotTerminatedException(file_line_num, newline.str(), curFile);
			
			if(!linestarted && line[i]==' ')
			{//if line hasn't started and character is space, skip it
				continue;
			}
			else if(linestarted && line[i]==' ')
			{//if there were symbols before this space, insert one space and stop saving spaces
				newline << line[i];
				linestarted = false;
			}
			else
			{//if not space, just store it in the stream and start saving spaces
				newline << line[i];
				linestarted = true;
			}	
		}
		//if there was something in the line, store it
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

void Assembler::setInstructionSet()
{
	cout << "\033[0;37mLoading possible command properties...\033[0m" << endl;
	instructionSet = new instruction[INSTRUCTION_NUM];
	string name, bitValue, supportsImmediateAddressing;
	ifstream instructionFile;
 	instructionFile.open ("instructions.txt");
 	int i = 0;
	while (instructionFile >> name >> bitValue >> supportsImmediateAddressing)
	{
	    instructionSet[i].key = name;
	    instructionSet[i].value = bitValue;
	    if(supportsImmediateAddressing == "1")
	    {
	    	instructionSet[i].supportsImmediateAddressing = true;
	    }
	    else
	    {
	    	instructionSet[i].supportsImmediateAddressing = false;
	    }
	    i++;
	}
 	instructionFile.close();
 	cout << "\033[1;32mCommand properties successfully loaded!\033[0m" << endl; 
}

void Assembler::linesIntoMC()
{
	bool emptyLine = false;
	bool stopConfirmed = false;
	for(int i=0; i<lines->size(); i++)
	{
		// add row of 0s where a VAR 0 was declared
		if(lines->at(i).find("VAR 0") != string::npos)
		{
			mc->push_back("00000000000000000000000000000000");
			emptyLine = true;
		}
		else
		{
			// look for instruction on the line
			bool found = false;
			for(int j=0; j<INSTRUCTION_NUM; j++)
			{
				if(lines->at(i).find(instructionSet[j].key) != string::npos)
				{
					found = true;
					mc->push_back(instructionSet[j].value);
					break;
				}
			}
			if(!found)
			{
				throw CommandNotRecognisedException(curFile, lines->at(i));
			}

			//put the operands before each command
			for(int j=0; j<vars->size(); j++)
			{
				if(lines->at(i).find(vars->at(j)->name) != string::npos)
				{
					int decimalValue = vars->at(j)->address; 
					string binaryValue = decToBin(decimalValue, 13);
					mc->at(i) = binaryValue + mc->at(i); 
				}	
			}

			//check for immediately declared variables
			try
			{
				bool negative = false;
				string lineToCheck = lines->at(i);
				istringstream iss(lineToCheck);
				vector<string> tokens;
				copy(istream_iterator<string>(iss),
     			istream_iterator<string>(),
     			back_inserter(tokens));
				int immediateVariable = stoi(tokens.at(1));
				if(immediateVariable < 0)
				{
					negative = true;
					immediateVariable = abs(immediateVariable);
				}	
				string binaryValue = decToBin(immediateVariable, 16);

				// add first part of the immediate variable, before the opcode
				string firstHalf;
				for(int x=0; x<13; x++)
				{
					firstHalf+= binaryValue[x];
				}
				mc->at(i) = firstHalf + mc->at(i);

				// add second part of the immediate variable, after the opcode
				int afterOpcode = 12;
				string secondHalf;
				for(int x=0; x<3; x++)
				{
					secondHalf+=binaryValue[afterOpcode];
					afterOpcode++;
				}
				int counter = 0;
				for(int x=16; x<19; x++)
				{
					mc->at(i)[x] = secondHalf[counter]; 
					counter++;
				}

				// add a 1 if the variable was negative
				if(negative)
				{
					mc->at(i)[20] = '1'; 
				}

				// leave a 1 at the end to show that it's an immediate variable
				mc->at(i)[31] = '1';
			}
			catch(invalid_argument& e)
			{
			}
			catch(out_of_range& e1)
			{
			}
		}

		// look for STP command
		if(lines->at(i).find("STP") != string::npos)
		{
			stopConfirmed = true;
		}
	}

	// if no STP keyword was found
	if(!stopConfirmed)
	{
		throw StopCommandNotFoundException(curFile);
	}
	
	// add binary values at the end of the output
	for(int j=0; j<vars->size(); j++)
	{
		bool negative = false;
		int decimalValue = vars->at(j)->value; 
		if(decimalValue < 0)
		{
			negative = true;
			decimalValue = abs(decimalValue);
		}		
		string binaryValue = decToBin(decimalValue, 32);
		if(negative)
		{
			binaryValue = negateBin(binaryValue);
		}
		mc->push_back(binaryValue);
	}

	// if there wasn't a VAR 0 at the beginning, manually add a row of 0s because it's necessary for the baby
	if(!(mc->at(0).find("00000000000000000000000000000000") != string::npos))
	{
		mc->insert(mc->begin() + 0, "00000000000000000000000000000000");
		// warning: no VAR 0 declared at the beginning?
	}
}

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

void Assembler::saveMC()
{
	ofstream myfile;
 	myfile.open ("output");
	for(int i = 0; i < mc->size(); i++)
	{
		myfile << mc->at(i) << endl;
	}
 	myfile.close();
}

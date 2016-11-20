#include "Assembler.h"

using namespace std;

int main()
{
	Assembler* assembler = new Assembler();
	
	try
	{
		string file;
		cout << endl << "Please enter the name of the file that contains the assembly program:" << endl;
		getline(cin, file);
		cout << endl;
		assembler->setFile(file); // assembly program to read from
		vector<string> asd = assembler->getLines();
		for(int i = 0; i < asd.size(); i++)
		{
			cout << asd.at(i) << endl;
		}
		
		cout << endl << "Variable list: " << endl;
		assembler->printVars();
		
	}
	catch(FileNotSetException e)
	{
		cout << e.message() << endl;
	}
	catch(OpenFileException ex)
	{
		cout << ex.message() << endl;
	}
	catch(LineNotTerminatedException ce1)
	{
		cout << ce1.message() << endl;
	}
	catch(InvalidVariableDeclarationException ce2)
	{
		cout << ce2.message() << endl;
	}
	catch(StartOrEndNotFoundException ce3)
	{
		cout << ce3.message() << endl;
	}
	catch(VariableOutOfRangeException ce4)
	{
		cout << ce4.message() << endl;
	}
	catch(CommandNotRecognisedException ce5)
	{
		cout << ce5.message() << endl;
	}

	catch(StopCommandNotFoundException ce6)
	{
		cout << ce6.message() << endl;
	}
	
	delete assembler;
	return 0;
}

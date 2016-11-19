#include <sstream>
#include <string>
using namespace std;

class FileNotSetException : public exception
{
	public:
		string message()
		{
			return "\033[1;31m assembler error: \033[0m  - file not specified! use assembler->setFile(string filepath) to specify the path";
		}
		virtual const char* what() const throw()
		{
			return "File to read from not specified. Use setFile(string filepath) to specify the file.";
		}
};

class OpenFileException : public exception
{
	private:
		string path;
	public:
		OpenFileException(string file)
		{
			path = file;
		}
		string message()
		{
			stringstream stream;
			stream << "\033[1m" << path << ": \033[1;31m error: \033[0m file could not be opened!";
			return stream.str();
		}
		virtual const char* what() const throw()
		{
			return "Couldn't open the file. Check filepath.";
		}
		
};

class LineNotTerminatedException : public exception
{
	private:
		int line_num;
		string line;
		string path;
	public:
	LineNotTerminatedException(int line_num, string line, string filename)
	{
		this->line_num = line_num;
		this->line = line;
		this->path = filename;
	}
	string message()
	{
		stringstream stream;
		stream << "\033[1m" << path << ":" << line_num << ": \033[1;31m error: \033[0m - line terminator \033[1m';'\033[0m expected\n\033[1m'" << line << "'\033[0m";
		return stream.str();
	}
	virtual const char* what() const throw()
	{
		return "One of the lines in the file wasn't terminated.";
	}
	
};

class InvalidVariableDeclarationException : public exception
{
	private:
		string path;
		string line;
		string var;
	public:
		InvalidVariableDeclarationException(string filepath, string line, string varname)
		{
			path = filepath;
			this->line = line;
			var = varname;
		}
		
		string message()
		{
			stringstream message;
			message << "\033[1m" << path << ": \033[1;31m error: \033[0m - illegal variable \033[1m\'"<< var << "\'\033[0m value format \n\033[1m\t'" << line << "'\033[0m"; 
			return message.str();
		}
		
		virtual const char* what() const throw()
		{
			return "Invalind variable declaration!";
		}
};

class VariableOutOfRangeException : public exception
{
	private:
		string path;
		string line;
		string name;
	public:
		VariableOutOfRangeException(string filepath, string line, string name)
		{
			path = filepath;
			this->line = line;
			this->name = name;
		}
		
		string message()
		{
			stringstream message;
			message <<"\033[1m" << path << ": \033[1;31m error: \033[0m - variable \033[1m'" << name << "'\033[0m value out of range \n\033[1m\t'" << line << "'\033[0m"; 
			return message.str();
		}
		
		virtual const char* what() const throw()
		{
			return "Variable value out of range!";
		}
};

class StartOrEndNotFoundException : public exception
{
	private:
		string path;
	public:
		StartOrEndNotFoundException(string filepath)
		{
			path = filepath;
		}
		
		string message()
		{
			stringstream message;
			message <<"\033[1m" << path <<  ": \033[1;34m warning: \033[0m - \033[1m'START'\033[0m or \033[1m'END'\033[0m tokens not found!"; 
			return message.str();
		}
		
		virtual const char* what() const throw()
		{
			return "Start or end token not found!";
		}
};

class CommandNotRecognisedException : public exception
{
	private:
		string line;
		string path;
	public:
	CommandNotRecognisedException(string filename, string line)
	{
		this->line = line;
		this->path = filename;
	}
	string message()
	{
		stringstream stream;
		stream << "\033[1m" << path << ": \033[1;31m error: \033[0m - command not recognised \n\033[1m\t'" << line << "'\033[0m";
		return stream.str();
	}
	virtual const char* what() const throw()
	{
		return "Command not recognised.";
	}
	
};
class StopCommandNotFoundException : public exception
{
	private:
		string path;
	public:
	StopCommandNotFoundException(string filename)
	{
		this->path = filename;
	}
	string message()
	{
		stringstream stream;
		stream << "\033[1m" << path << ": \033[1;31m error: \033[0m - STP command missing from the file'\033[0m";
		return stream.str();
	}
	virtual const char* what() const throw()
	{
		return "STP command missing from the file.";
	}
	
};



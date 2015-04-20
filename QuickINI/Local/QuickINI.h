#pragma once

// Includes:
#include <exception>
#include <iostream>
#include <string>
#include <map>

// Namespace(s):
namespace quickLib
{
	namespace INI
	{
		// Namespace(s):
		using namespace std;

		// Typedefs:

		// These are used to represent encoded INI data:
		typedef string INISectionTag;
		typedef pair<string, string> INIVariable;
		typedef map<string, string> INISection;
		typedef map<INISectionTag, INISection> INIVariables;

		typedef size_t lineNumber_t;

		// Constant variable(s):
		const char assignmentSymbol = '=';
		const char commentSymbol = ';';
		const char sectionBeginSymbol = '[';
		const char sectionEndSymbol = ']';

		const INISectionTag defaultINISection = "global"; // "master";

		// Classes:
		
		// Exceptions:
		class INI_EXCEPTION : public exception
		{
			public:
				// Constructor(s):
				INI_EXCEPTION();

				// Methods:
				virtual const string message() const throw();
		};

		class lineError : public INI_EXCEPTION
		{
			public:
				// Constructor(s):
				lineError(lineNumber_t lineNumber);

				// Methods:
				virtual const string message() const throw() override;

				// This currently acts as a standard-compliant wrapper for the 'message' command.
				virtual const char* what() const throw() override;

				// Fields:
				lineNumber_t error_line;
		};

		class invalidSegment : public lineError
		{
			public:
				// Constructor(s):
				invalidSegment(lineNumber_t lineNumber);

				// Methods:
				virtual const string message() const throw() override;

				// Fields:
				// Nothing so far.
		};

		class variableParseError : public lineError
		{
			public:
				// Constructor(s):
				variableParseError(lineNumber_t lineNumber, string info="");

				// Methods:
				virtual const string message() const throw() override;

				// Fields:
				string information;
		};

		class invalidRightOperand : public variableParseError
		{
			public:
				// Constructor(s):
				invalidRightOperand(lineNumber_t lineNumber, string left, string right=string());

				// Methods:
				virtual const string message() const throw() override;

				// Fields:
				string leftOperand;
				string rightOperand;
		};

		// Functions:

		// This trims a string from the beginning.
		string ltrim(string s);

		// This trims a string in reverse.
		string rtrim(string s);

		// This trims a string from both ends.
		string trim(string s);

		void load(const string& path, INIVariables& variables);
		void save(const string& path, const INIVariables& variables, bool insertTrailingSpaces=false);

		void read(istream& is, INIVariables& variables);
		void write(ostream& os, const INIVariables& variables, bool insertTrailingSpaces=false);

		// This command reads an INI entry from the specified input-stream.
		// The return value is the new line-number, based on the input given.
		// DO NOT call this on an input-stream that has already ended; please handle this yourself.
		lineNumber_t readEntryFromLines(istream& is, INIVariable& variable, string line, const lineNumber_t currentLineNumber=0, bool skipStreamCheck=true, const char separator=assignmentSymbol, const char commentChar=commentSymbol);

		void flushVariables(INIVariables& variables, INISectionTag& currentSection, INISection& currentEntries);

		inline INIVariables load(const string& path)
		{
			INIVariables output;

			load(path, output);

			return output;
		}

		inline INIVariables read(istream& is)
		{
			INIVariables output;

			read(is, output);

			return output;
		}
	}
}
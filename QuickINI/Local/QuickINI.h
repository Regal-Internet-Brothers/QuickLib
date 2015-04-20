#pragma once

/*
	NOTES:
		* DO NOT mix wide and standard STL technologies, unless you're sure it will compile.
		For example, wide-streams likely won't support 'std::string' as an input.
*/

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
		typedef wstring wINISectionTag;

		typedef size_t lineNumber_t;

		// Generic aliases:
		template <typename strType=string>
		using INIVariable = pair<strType, strType>;

		template <typename strType=string>
		using INISection = map<strType, strType>;

		template <typename strType=INISectionTag>
		using INIVariables = map<strType, INISection<strType>>;

		// Constant variable(s):
		enum symbols : INISectionTag::value_type // char
		{
			assignmentSymbol = '=',
			commentSymbol = ';',
			sectionBeginSymbol = '[',
			sectionEndSymbol = ']',
		};

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

		class operationUnsupported : public INI_EXCEPTION
		{
			public:
				// Constructor(s):
				operationUnsupported();

				// Methods:
				virtual const string message() const throw();

				// This currently acts as a standard-compliant wrapper for the 'message' command.
				virtual const char* what() const throw() override;
		};

		class lineError : public INI_EXCEPTION
		{
			public:
				// Constructor(s):
				lineError(lineNumber_t lineNumber);

				// Methods:
				virtual const string message() const throw() override;

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

		// String management:

		// Functions:
		string wideStringToDefault(const wstring wstr);
		wstring defaultStringToWide(const string str);

		inline void correctString(const string str, wstring& output)
		{
			output = defaultStringToWide(str);

			return;
		}

		inline void correctString(const wstring wstr, string& output)
		{
			output = wideStringToDefault(wstr);

			return;
		}

		inline void correctString(const string str, string& output)
		{
			output = str;

			return;
		}

		inline void correctString(const wstring wstr, wstring& output)
		{
			output = wstr;

			return;
		}

		inline string abstractStringToDefault(const string str)
		{
			return str;
		}

		inline string abstractStringToDefault(const wstring wstr)
		{
			return wideStringToDefault(wstr);
		}

		inline wstring abstractStringToWide(const wstring wstr)
		{
			return wstr;
		}

		inline wstring abstractStringToWide(const string str)
		{
			return defaultStringToWide(str);
		}

		// This trims a string from the beginning.
		template <typename str=string>
		str ltrim(str s);

		// This trims a string in reverse.
		template <typename str=string>
		str rtrim(str s);

		// This trims a string from both ends.
		template <typename str=string>
		str trim(str s);

		template <typename INIVars=INIVariables<>>
		void load(const string& path, INIVars& variables);

		template <typename INIVars=INIVariables<>>
		void save(const string& path, const INIVars& variables, bool insertTrailingSpaces=false);

		template <typename INIVars=INIVariables<>>
		void load(const wstring& path, INIVars& variables);

		template <typename INIVars=INIVariables<>>
		void save(const wstring& path, const INIVars& variables, bool insertTrailingSpaces=false);

		template <typename inputStream=istream, typename INIVars=INIVariables<>>
		void read(inputStream& is, INIVars& variables);

		template <typename outputStream=ostream, typename INIVars=INIVariables<>>
		void write(outputStream& os, const INIVars& variables, bool insertTrailingSpaces=false);

		// This command reads an INI entry from the specified input-stream.
		// The return value is the new line-number, based on the input given.
		// DO NOT call this on an input-stream that has already ended; please handle this yourself.
		template <typename inputStream=istream, typename INIVars=INIVariables<>, typename strType=string>
		lineNumber_t readEntryFromLines(inputStream& is, INIVars& variable, strType line, const lineNumber_t currentLineNumber=0, bool skipStreamCheck=true, const symbols separator=assignmentSymbol, const symbols commentChar=commentSymbol);

		template <typename INIVars=INIVariables<>, typename INISecTag=INISectionTag, typename INISec=INISection<>>
		void flushVariables(INIVars& variables, INISecTag& currentSection, INISec& currentEntries);

		template <typename strType=string, typename INIVars=INIVariables<>>
		inline INIVars load(const strType& path)
		{
			INIVars output;

			load(path, output);

			return output;
		}

		template <typename inputStream=istream, typename INIVars=INIVariables<basic_string<inputStream::char_type>>>
		inline INIVars read(inputStream& is)
		{
			INIVars output;

			read(is, output);

			return output;
		}
	}
}
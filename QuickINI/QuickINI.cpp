// Preprocessor related:
//#define QINI_LOCAL_BUILD

// Includes:
#include <fstream>
#include <sstream>

#include <algorithm>
#include <functional>
#include <cctype>
#include <codecvt>
//#include <locale>

#ifdef QINI_LOCAL_BUILD
	#include "Local/QuickINI.h"
#else
	#include <QuickLib/QuickINI/QuickINI.h>
#endif

// Namespace(s):
namespace quickLib
{
	namespace INI
	{
		// Classes:
		
		// Exceptions:

		// INI_EXCEPTION:

		// Constructor(s):
		INI_EXCEPTION::INI_EXCEPTION() : exception()
		{
			// Nothing so far.
		}
		
		const string INI_EXCEPTION::message() const throw()
		{
			return (string)what();
		}

		// operationUnsupported:

		// Constructor(s):
		operationUnsupported::operationUnsupported() : INI_EXCEPTION()
		{
			// Nothing so far.
		}

		// Methods:
		const string operationUnsupported::message() const throw()
		{
			return (string)"Operation unsupported.";
		}

		const char* operationUnsupported::what() const throw()
		{
			return message().c_str();
		}

		// lineError:

		// Constructor(s):
		lineError::lineError(lineNumber_t lineNumber) : error_line(lineNumber)
		{
			// Nothing so far.
		}

		// Methods:
		const string lineError::message() const throw()
		{
			stringstream ss;

			ss << "Unknown error on line: " << error_line;

			return ss.str();
		}

		// invalidSegment:

		// Constructor(s):
		invalidSegment::invalidSegment(lineNumber_t lineNumber) : lineError(lineNumber)
		{
			// Nothing so far.
		}

		// Methods:
		const string invalidSegment::message() const throw()
		{
			stringstream ss;

			ss << "Invalid segment on line: " << error_line;

			return ss.str();
		}

		// variableParseError:

		// Constructor(s):
		variableParseError::variableParseError(lineNumber_t lineNumber, string info) : lineError(lineNumber), information(info)
		{
			// Nothing so far.
		}

		const string variableParseError::message() const throw()
		{
			stringstream ss;

			ss << "Unable to parse variable on line: " << error_line;
			
			if (!information.empty())
			{
				ss << " - " << information;
			}

			return ss.str();
		}

		// invalidRightOperand:

		// Constructor(s):
		invalidRightOperand::invalidRightOperand(lineNumber_t lineNumber, string left, string right)
			: variableParseError(lineNumber), leftOperand(left), rightOperand(right) { /* Nothing so far. */ }

		// Methods:
		const string invalidRightOperand::message() const throw()
		{
			stringstream ss;

			ss << "Invalid right operand on line: " << error_line << " | ";

			if (!leftOperand.empty())
			{
				ss << "L: " << left << ", ";

				if (!rightOperand.empty())
				{
					ss << "R: " << right;
				}
				else
				{
					ss << "R: NULL";
				}
			}
			else if (!rightOperand.empty())
			{
				ss << "L: NULL, R: " << right;
			}
			else
			{
				ss << "NULL";
			}

			return ss.str();
		}

		// Functions:
		
		// String management:
		string wideStringToDefault(const wstring wstr)
		{
			std::wstring_convert<std::codecvt_utf8<wchar_t>> stringConverter;

			return stringConverter.to_bytes(wstr);
		}
		
		wstring defaultStringToWide(const string str)
		{
			std::wstring_convert<std::codecvt_utf8<wchar_t>> stringConverter;

			return stringConverter.from_bytes(str);
		}

		static int isquote(int c)
		{
			return (int)(c == '\"');
		}

		// String "cleanup" / trimming code by Evan Teran:
		template <typename str>
		str ltrim(str s)
		{
			s.erase(s.begin(), find_if(s.begin(), s.end(), not1(ptr_fun<int, int>(isspace))));
			s.erase(s.begin(), find_if(s.begin(), s.end(), not1(ptr_fun<int, int>(isquote))));
			//s.erase(s.begin(), find_if(s.begin(), s.end(), not1(ptr_fun<int, int>(ispunct))));

			return s;
		}

		template <typename str>
		str rtrim(str s)
		{
			s.erase(find_if(s.rbegin(), s.rend(), not1(ptr_fun<int, int>(isspace))).base(), s.end());
			s.erase(find_if(s.rbegin(), s.rend(), not1(ptr_fun<int, int>(isquote))).base(), s.end());
			//s.erase(find_if(s.rbegin(), s.rend(), not1(ptr_fun<int, int>(ispunct))).base(), s.end());

			return s;
		}

		template <typename str>
		str trim(str s)
		{
			return rtrim(ltrim(s));
		}

		template <typename INIVars>
		void load(const string& path, INIVars& variables)
		{
			ifstream f;

			f.open(path);

			read(f, variables);

			f.close();

			return;
		}

		template <typename INIVars>
		void save(const string& path, const INIVars& variables, bool insertTrailingSpaces)
		{
			ofstream f;

			f.open(path);

			write(f, variables, insertTrailingSpaces);

			f.close();

			return;
		}

		template <typename INIVars>
		void load(const wstring& path, INIVars& variables)
		{
			wifstream f;

			f.open(path);

			read(f, variables);

			f.close();

			return;
		}

		template <typename INIVars>
		void save(const wstring& path, const INIVars& variables, bool insertTrailingSpaces)
		{
			wofstream f;

			f.open(path);

			write(f, variables, insertTrailingSpaces);

			f.close();

			return;
		}

		template <typename inputStream, typename INIVars>
		void read(inputStream& is, INIVars& variables)
		{
			// Typedefs:
			typedef INIVars::key_type strType;

			// Local variable(s):

			// The global line-number for this read operation.
			lineNumber_t lineNumber = 1;

			// The current INI section.
			strType currentSection;

			// Unfortunately, since function templates are picky, I have to use this.
			correctString("global", currentSection);

			INISection<strType> currentEntries;

			// Read every line of the file:
			while (!is.eof())
			{
				// Local variable(s):
				strType line;

				getline(is, line);

				if (line.empty())
					continue;

				auto sectionBegin = line.find(sectionBeginSymbol);
				
				// Section tags must be on a single line (Currently):
				if (sectionBegin != strType::npos)
				{
					auto sectionEnd = line.find(sectionEndSymbol);

					// Ensure this is an actual section-tag:
					if (line.find(commentSymbol) > sectionEnd && line.find(assignmentSymbol) > sectionEnd)
					{
						if (sectionEnd != strType::npos)
						{
							// Flush the current section, then get the new section-name:
							flushVariables(variables, currentSection, currentEntries);
							currentSection = line.substr(sectionBegin+1, sectionEnd-1);
						}
						else
						{
							throw invalidSegment(lineNumber);
						}

						// Check if there's more to the line:
						if (sectionEnd+1 == line.length())
							continue;

						// Change the "scope" of the line.
						line = line.substr(sectionEnd+1);
					}
				}

				// The variable-entry we're going to read.
				INIVariable<strType> variable;

				// Read the entry, then add to the line-number.
				lineNumber = readEntryFromLines(is, variable, line, lineNumber);

				// Insert the newly read entry.
				currentEntries.insert(variable);
			}

			// Flush the current variables to the 'variables' container.
			flushVariables(variables, currentSection, currentEntries);

			return;
		}

		template <typename outputStream, typename INIVars>
		void write(outputStream& os, const INIVars& variables, bool insertTrailingSpaces)
		{
			for (auto& section : variables)
			{
				os << (char)sectionBeginSymbol << section.first << (char)sectionEndSymbol << endl;

				for (auto& entry : section.second)
				{
					os << entry.first << " " << (char)assignmentSymbol << " " << entry.second << endl;
				}

				if (insertTrailingSpaces)
				{
					os << endl;
				}
			}

			return;
		}

		template <typename inputStream, typename INIVars, typename strType>
		lineNumber_t readEntryFromLines(inputStream& is, INIVars& variable, strType line, const lineNumber_t currentLineNumber, bool skipStreamCheck, const symbols separator, const symbols commentChar)
		{
			if (!skipStreamCheck && is.eof())
			{
				throw variableParseError(currentLineNumber, "Stream ended.");
			}

			// Local variable(s):
			auto comment = line.find(commentChar);
			
			// Check for comments:
			if (comment != strType::npos)
			{
				// This line is only a comment, ignore it.
				if (comment == 0)
				{
					getline(is, line);
					
					return readEntryFromLines(is, variable, line, currentLineNumber+1, false, separator, commentChar);
				}

				// Change the "scope" of the line to ignore the comment.
				line = line.substr(0, comment);
			}

			auto assignment = line.find(separator);

			if (assignment == strType::npos)
			{
				variable.first = variable.first + trim(line);

				getline(is, line);

				return readEntryFromLines(is, variable, line, currentLineNumber+1, false, separator, commentChar);
			}
				
			if (variable.first.empty())
			{
				variable.first = trim(line.substr(0, assignment));
			}
			else
			{
				variable.first += trim(line.substr(0, assignment));
			}

			// Now that we're loaded a name for this variable, make sure it's valid:
			if (variable.first.empty())
			{
				throw variableParseError(currentLineNumber, "Invalid variable name.");
			}

			if ((assignment + 1) < line.length())
			{
				variable.second = trim(line.substr(assignment+1));
			}
			else
			{
				throw invalidRightOperand(currentLineNumber, abstractStringToDefault(variable.first));
			}

			return currentLineNumber+1;
		}

		template <typename INIVars, typename INISecTag, typename INISec>
		void flushVariables(INIVars& variables, INISecTag& currentSection, INISec& currentEntries)
		{
			// Insert the new variable into the 'variables' container:
			variables[currentSection] = currentEntries;
			currentEntries = INISec(); // currentEntries.clear();

			return;
		}
	}
}

/*
int main()
{
	// Namespace(s):
	using namespace std;
	using namespace quickLib;

	wstring file = L"X\t\t\t=Y\nZ=  W\nA = B\nC =D\n[Test]O_O=\"[10, 15, 20, 25]\" ; Hello world.\nLife=42\nYou know what they say         = Hello World ;;;;.";

	wstringstream is(file);

	INI::INIVariables<wstring> data;

	try
	{
		data = INI::read(is);

		for (auto& section : data)
		{
			wcout << '[' << section.first << ']' << endl;

			for (auto& e : section.second)
			{
				wcout << '\'' << e.first << '\'' << " = " << '\'' << e.second << '\'' << endl;
			}

			wcout << endl;
		}
	}
	catch (const INI::INI_EXCEPTION& e)
	{
		cout << "Exception caught: " << e.message() << endl;

		system("PAUSE");

		return -1;
	}

	cout << "------------------------" << endl;

	INI::write(wcout, data, true);

	system("PAUSE");

	return 0;
}
*/
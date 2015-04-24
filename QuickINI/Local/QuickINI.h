#pragma once

/*
	NOTES:
		* This library is currently header-only. That being said, it's a good idea to reduce includes of this header.
		* DO NOT mix wide and standard STL technologies, unless you're sure it will compile.
		For example, wide-streams likely won't support 'std::string' as an input.
*/

// Includes:
#include <stdexcept>
#include <algorithm>
#include <functional>

#include <string>

#include <cctype>
#include <codecvt>
//#include <locale>

#include <iostream>
#include <fstream>
#include <sstream>

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
		class INI_EXCEPTION : public runtime_error
		{
			public:
				// Constructor(s):
				INI_EXCEPTION(const string& exception_name="QuickINI: Exception.") : runtime_error(exception_name) { /* Nothing so far. */ }

				// Methods:
				virtual const string message() const throw()
				{
					return (string)what();
				}
		};

		template<typename characterType=char, typename characterTraits=char_traits<characterType>, typename strAlloc=allocator<characterType>>
		class fileException : public INI_EXCEPTION
		{
			public:
				// Typedefs:
				typedef basic_string<characterType, characterTraits, strAlloc> strType;
				typedef basic_stringstream<characterType, characterTraits, strAlloc> strStream;

				// Fields:
				const strType file_path;

				// Constructor(s):
				fileException(const strType path, const string& exception_name="QuickINI: File exception.") : INI_EXCEPTION(exception_name), file_path(path) { /* Nothing so far. */ }

				// Methods:
				virtual const strType native_message() const throw()
				{
					strStream ss;

					strType str;

					correctString("Unknown file exception: ", str);

					ss << str << file_path;

					return ss.str();
				}

				virtual const string message() const throw() override
				{
					return abstractStringToDefault(native_message());
				}
		};

		template<typename characterType=char, typename characterTraits=char_traits<characterType>, typename strAlloc=allocator<characterType>>
		class fileNotFound : public fileException<characterType, characterTraits, strAlloc>
		{
			public:
				// Constructor(s):
				fileNotFound(const strType path, const string& exception_name="QuickINI: File not found.") : fileException(path, exception_name) { /* Nothing so far. */ }

				// Methods:
				virtual const strType native_message() const throw() override
				{
					strStream ss;

					strType str;

					correctString("File not found: ", str);

					ss << str << file_path;

					return ss.str();
				}
		};

		template<typename characterType=char, typename characterTraits=char_traits<characterType>, typename strAlloc=allocator<characterType>>
		class invalidWriteOperation : public fileException<characterType, characterTraits, strAlloc>
		{
			public:
				// Constructor(s):
				invalidWriteOperation(const strType path, const string& exception_name="QuickINI: Invalid write operation.") : fileException(path, exception_name) { /* Nothing so far. */ }

				// Methods:
				virtual const strType native_message() const throw() override
				{
					strStream ss;

					strType str;

					correctString("Invalid write operation: ", str);

					ss << str << file_path;

					return ss.str();
				}
		};

		class operationUnsupported : public INI_EXCEPTION
		{
			public:
				// Constructor(s):
				operationUnsupported(const string& exception_name="QuickINI: Operation unsupported.") : INI_EXCEPTION(exception_name) { /* Nothing so far. */ }

				// Methods:
				virtual const string message() const throw()
				{
					return (string)"Operation unsupported.";
				}
		};

		class lineError : public INI_EXCEPTION
		{
			public:
				// Fields:
				const lineNumber_t error_line;

				// Constructor(s):
				lineError(const lineNumber_t lineNumber, const string& exception_name="QuickINI: Generic line error.") : INI_EXCEPTION(exception_name), error_line(lineNumber) { /* Nothing so far. */ }

				// Methods:
				virtual const string message() const throw() override
				{
					stringstream ss;

					ss << "Unknown error on line: " << error_line;

					return ss.str();
				}
		};

		class invalidSegment : public lineError
		{
			public:
				// Constructor(s):
				invalidSegment(const lineNumber_t lineNumber, const string& exception_name="QuickINI: Invalid segment.") : lineError(lineNumber, exception_name) { /* Nothing so far. */ }

				// Methods:
				virtual const string message() const throw() override
				{
					stringstream ss;

					ss << "Invalid segment on line: " << error_line;

					return ss.str();
				}

				// Fields:
				// Nothing so far.
		};

		class variableParseError : public lineError
		{
			public:
				// Constructor(s):
				variableParseError(const lineNumber_t lineNumber, const string info="", const string& exception_name="QuickINI: Variable parse error.")
					: lineError(lineNumber, exception_name), information(info) { /* Nothing so far. */ }

				// Methods:
				virtual const string message() const throw() override
				{
					stringstream ss;

					ss << "Unable to parse variable on line: " << error_line;
			
					if (!information.empty())
					{
						ss << " - " << information;
					}

					return ss.str();
				}

				// Fields:
				const string information;
		};

		class invalidRightOperand : public variableParseError
		{
			public:
				// Fields:
				const string leftOperand;
				const string rightOperand;

				// Constructor(s):
				invalidRightOperand(const lineNumber_t lineNumber, const string left, const string right=string(), const string& exception_name="QuickINI: Invalid right-operand.")
					: variableParseError(lineNumber, exception_name), leftOperand(left), rightOperand(right) { /* Nothing so far. */ }

				// Methods:
				virtual const string message() const throw() override
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
		};

		// Functions:

		// String management:

		// Functions:
		inline string wideStringToDefault(const wstring wstr)
		{
			std::wstring_convert<std::codecvt_utf8<wchar_t>> stringConverter;

			return stringConverter.to_bytes(wstr);
		}
		
		inline wstring defaultStringToWide(const string str)
		{
			std::wstring_convert<std::codecvt_utf8<wchar_t>> stringConverter;

			return stringConverter.from_bytes(str);
		}

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

		static inline int isquote(int c)
		{
			return (int)(c == '\"');
		}

		// String "cleanup" / trimming code by Evan Teran:
		template <typename str=string>
		inline str ltrim(str s)
		{
			s.erase(s.begin(), find_if(s.begin(), s.end(), not1(ptr_fun<int, int>(isspace))));
			s.erase(s.begin(), find_if(s.begin(), s.end(), not1(ptr_fun<int, int>(isquote))));
			//s.erase(s.begin(), find_if(s.begin(), s.end(), not1(ptr_fun<int, int>(ispunct))));

			return s;
		}

		template <typename str=string>
		inline str rtrim(str s)
		{
			s.erase(find_if(s.rbegin(), s.rend(), not1(ptr_fun<int, int>(isspace))).base(), s.end());
			s.erase(find_if(s.rbegin(), s.rend(), not1(ptr_fun<int, int>(isquote))).base(), s.end());
			//s.erase(find_if(s.rbegin(), s.rend(), not1(ptr_fun<int, int>(ispunct))).base(), s.end());

			return s;
		}

		template <typename str=string>
		inline str trim(str s)
		{
			return rtrim(ltrim(s));
		}

		// This command reads an INI entry from the specified input-stream.
		// The return value is the new line-number, based on the input given.
		// DO NOT call this on an input-stream that has already ended; please handle this yourself.
		template <typename characterType=char, typename characterTraits=char_traits<characterType>, typename strAlloc=allocator<characterType>>
		inline lineNumber_t readEntryFromLines
		(
			basic_istream<characterType, characterTraits>& is,
			INIVariable<basic_string<characterType, characterTraits, strAlloc>>& variable,
			basic_string<characterType, characterTraits, strAlloc> line, const lineNumber_t currentLineNumber=0, const bool skipStreamCheck=true,
			const symbols separator=assignmentSymbol, const symbols commentChar=commentSymbol
		)
		{
			// Typedefs:
			typedef basic_string<characterType, characterTraits, strAlloc> strType;

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
					
					return readEntryFromLines<characterType, characterTraits>(is, variable, line, currentLineNumber+1, false, separator, commentChar);
				}

				// Change the "scope" of the line to ignore the comment.
				line = line.substr(0, comment);
			}

			auto assignment = line.find(separator);

			if (assignment == strType::npos)
			{
				variable.first = variable.first + trim(line);

				getline(is, line);

				return readEntryFromLines<characterType, characterTraits>(is, variable, line, currentLineNumber+1, false, separator, commentChar);
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

		template <typename INISecTag=INISectionTag, typename INISec=INISection<INISecTag>>
		inline void flushVariables(INIVariables<INISecTag>& variables, const INISecTag& currentSection, INISec& currentEntries)
		{
			// Insert the new variable into the 'variables' container:
			variables[currentSection] = currentEntries;
			currentEntries = INISec(); // currentEntries.clear();

			return;
		}
		
		template <typename characterType=char, typename characterTraits=char_traits<characterType>, typename strAlloc=allocator<characterType>>
		inline void read(basic_istream<characterType, characterTraits>& is, INIVariables<basic_string<characterType, characterTraits, strAlloc>>& variables)
		{
			// Typedefs:
			//typedef INIVars::key_type strType;
			typedef basic_string<characterType, characterTraits, strAlloc> strType;

			// Local variable(s):

			// The global line-number for this read operation.
			lineNumber_t lineNumber = 1;

			// The current INI section.
			strType currentSection;

			// Unfortunately, since function templates are picky, I have to use this.
			correctString("global", currentSection);

			INISection<strType> currentEntries;

			strType line;

			// Read every line of the file:
			while (getline(is, line))
			{
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
							flushVariables<strType>(variables, currentSection, currentEntries);
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
				lineNumber = readEntryFromLines<characterType, characterTraits>(is, variable, line, lineNumber);

				// Insert the newly read entry.
				currentEntries.insert(variable);
			}

			// Flush the current variables to the 'variables' container.
			flushVariables<strType>(variables, currentSection, currentEntries);

			return;
		}
		
		template <typename characterType=char, typename characterTraits=char_traits<characterType>, typename strAlloc=allocator<characterType>>
		inline void write(basic_ostream<characterType, characterTraits>& os, const INIVariables<basic_string<characterType, characterTraits, strAlloc>>& variables, const bool insertTrailingSpaces=false)
		{
			for (auto& section : variables)
			{
				os << (characterType)sectionBeginSymbol << section.first << (characterType)sectionEndSymbol << endl;

				for (auto& entry : section.second)
				{
					os << entry.first << " " << (characterType)assignmentSymbol << " " << entry.second << endl;
				}

				if (insertTrailingSpaces)
				{
					os << endl;
				}
			}

			return;
		}
		
		inline void load(const string& path, INIVariables<string>& variables)
		{
			ifstream f;

			//f.exceptions(ifstream::failbit | ifstream::badbit);

			f.open(path);

			if (f.fail())
			{
				throw fileNotFound<string::value_type>(path);

				return;
			}

			read(f, variables);

			f.close();

			return;
		}
		
		inline void save(const string& path, const INIVariables<string>& variables, const bool insertTrailingSpaces=false)
		{
			ofstream f;
			
			//f.exceptions(ofstream::failbit | ofstream::badbit);

			f.open(path);

			if (f.fail())
			{
				throw invalidWriteOperation<string::value_type>(path);

				return;
			}
			
			write(f, variables, insertTrailingSpaces);
			
			f.close();
			
			return;
		}

		inline void load(const wstring& path, INIVariables<wstring>& variables)
		{
			wifstream f;

			//f.exceptions(wifstream::failbit | wifstream::badbit);

			f.open(path);

			if (f.fail())
			{
				throw fileNotFound<wstring::value_type>(path);

				return;
			}
			
			read<wstring::value_type, wstring::traits_type>(f, variables);

			f.close();

			return;
		}
		
		inline void save(const wstring& path, const INIVariables<wstring>& variables, const bool insertTrailingSpaces=false)
		{
			wofstream f;
			
			//f.exceptions(wofstream::failbit | wofstream::badbit);

			f.open(path);

			if (f.fail())
			{
				throw invalidWriteOperation<wstring::value_type>(path);

				return;
			}
			
			write<wstring::value_type, wstring::traits_type>(f, variables, insertTrailingSpaces);
			
			f.close();
			
			return;
		}
		
		template <typename strType=string>
		inline INIVariables<strType> load(const strType& path)
		{
			INIVariables<strType> output;
			
			load(path, output);
			
			return output;
		}

		template <typename characterType=char, typename characterTraits=char_traits<characterType>, typename strAlloc=allocator<characterType>>
		inline INIVariables<basic_string<characterType, characterTraits, strAlloc>> read(basic_istream<characterType, characterTraits>& is)
		{
			INIVariables<basic_string<characterType, characterTraits, strAlloc>> output;
			
			read(is, output);
			
			return output;
		}
	}
}
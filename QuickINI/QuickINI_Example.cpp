// Preprocessor related:
//#define QINI_LOCAL_BUILD

// Includes:
#ifdef QINI_LOCAL_BUILD
	#include "Local/QuickINI.h"
#else
	#include <QuickLib/QuickINI/QuickINI.h>
#endif

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

	cout << "------------------------" << endl << endl;

	INI::write(wcout, data, true);

	system("PAUSE");

	return 0;
}
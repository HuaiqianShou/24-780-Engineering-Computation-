#pragma once
#pragma warning(disable:4996)

#include <string>
#include <vector>

//#include <msclr\marshal_cppstd.h>  //  needed for string conversions from CLI

using namespace std;

class StringPlus {
public:
	static string ltrim(const string& inString) {
		auto firstNonSpace = inString.find_first_not_of(" ");
		if (firstNonSpace == -1)
			return "";
		else
			return inString.substr(firstNonSpace);
	}
	static string rtrim(const string& inString) {
		return inString.substr(0, inString.find_last_not_of(" ") + 1);
	}
	static string trim(const string& inString) {
		return ltrim(rtrim(inString));
	}

	// useful when using CLR/CLI interface
	//static string convertString(System::String^ aString) { // needed for CLI
	//	msclr::interop::marshal_context context;
	//	std::string standardString = context.marshal_as<std::string>(aString);
	//	return standardString;
	//}

	static std::vector<std::string> split(std::string str, std::string delim);

	// adds newline characters to assure paragraph fits 
	static string wrapLongLine(const string& outString, int lineLength);

	// replace every instance of "from" with "to" in "original" string
	static string replaceAll(const string& original, 
		const string& from, const string& to);

	// convert value to string with given significant figures
	static string sigFig(double theValue, int figCount);

	// keeps asking "prompt" until user enters a valid number
	static double getDouble(std::istream& input, const std::string& prompt);

};
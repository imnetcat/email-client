#include "warn.h"
#include <sstream>
using namespace std;
using namespace Core;

Logging::Warn::Warn() : Log("WARN") { }

const std::string Logging::Warn::log(const string& text)
{
	return Log::log("\n" + text);
}
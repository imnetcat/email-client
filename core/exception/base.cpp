#include "base.h"
using namespace std;

Exceptions::base::base(const std::exception& ex) : _when(ex.what()) {}
Exceptions::base::base(const string w) : _when(w) { };
Exceptions::base::base(const string en, const string ere)
	: _when(en), _where(ere) { };


const string Exceptions::base::log() const noexcept
{
	string log;
	log += "\twhat  : " + string(what()) + "\n";
	log += "\twhen  : " + _when + "\n";
	log += "\twhere : " + _where + "\n";
	return log;
}

const string Exceptions::base::log(const std::string& when_preppend, const std::string& where_preppend) const noexcept
{
	string log;
	log += "\twhat  : " + string(what()) + "\n";
	log += "\twhen  : " + when_preppend + "\n\t\t" + _when + "\n";
	log += "\twhere : " + where_preppend + "\n\t\t" + _where + "\n";
	return log;
}
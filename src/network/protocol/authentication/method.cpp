#include "method.h"
#include "../../../core/utils.h"
#include "../../../encryption/algorithm/md5.h"
#include "../../../encryption/algorithm/base64.h"
#include "exception.h"

std::string Cout::Network::Protocol::Authentication::Method::Plain(const std::string& login, const std::string& pass)
{
	std::string s = login + "^" + login + "^" + pass;
	size_t length = s.size();
	UnsignedBinary ustrLogin;
	ustrLogin.Assign((UnsignedByte*)s.data(), s.size());
	for (unsigned int i = 0; i < length; i++)
	{
		if (ustrLogin[i] == 94) ustrLogin[i] = 0;
	}

	Encryption::Algorithm::Base64 base64;
	return Core::Utils::to_string(base64.Encode(ustrLogin).data());
}

std::string Cout::Network::Protocol::Authentication::Method::Login(const std::string& credentials)
{
	Encryption::Algorithm::Base64 base64;
	UnsignedBinary res;
	res.Assign((UnsignedByte*)credentials.data(), credentials.size());
	res = base64.Encode(res);
	res.push_back(0);
	return Core::Utils::to_string(res.data());
}

std::string Cout::Network::Protocol::Authentication::Method::CramMD5(const std::string& encoded_challenge, const std::string& login, const std::string& pass)
{
	Encryption::Algorithm::Base64 base64;
	UnsignedBinary temp;
	temp.Assign((UnsignedByte*)encoded_challenge.data(), encoded_challenge.size());
	UnsignedBinary decoded_challenge = base64.Decode(temp);
	decoded_challenge.push_back(0);
	/////////////////////////////////////////////////////////////////////
	//test data from RFC 2195
	//decoded_challenge = "<1896.697170952@postoffice.reston.mci.net>";
	//m_sLogin = "tim";
	//m_sPassword = "tanstaaftanstaaf";
	//MD5 should produce b913a602c7eda7a495b4e6e7334d3890
	//should encode as dGltIGI5MTNhNjAyYzdlZGE3YTQ5NWI0ZTZlNzMzNGQzODkw
	/////////////////////////////////////////////////////////////////////

	unsigned char *ustrChallenge = decoded_challenge.data();
	unsigned char *ustrPassword = Cout::Core::Utils::StringToUnsignedChar(pass);

	// if ustrPassword is longer than 64 bytes reset it to ustrPassword=MD5(ustrPassword)
	size_t passwordLength = pass.size();
	if (passwordLength > 64) {
		Encryption::Algorithm::MD5 md5password;
		md5password.update(ustrPassword, passwordLength);
		md5password.finalize();
		ustrPassword = md5password.raw_digest();
		passwordLength = 16;
	}

	// Storing ustrPassword in pads
	unsigned char ipad[65], opad[65];
	memset(ipad, 0, 64);
	memset(opad, 0, 64);
	memcpy(ipad, ustrPassword, passwordLength);
	memcpy(opad, ustrPassword, passwordLength);

	// XOR ustrPassword with ipad and opad values
	for (int i = 0; i < 64; i++) {
		ipad[i] ^= 0x36;
		opad[i] ^= 0x5c;
	}

	// perform inner MD5
	Encryption::Algorithm::MD5 md5pass1;
	md5pass1.update(ipad, 64);
	md5pass1.update(ustrChallenge, decoded_challenge.size());
	md5pass1.finalize();
	unsigned char *ustrResult = md5pass1.raw_digest();

	// perform outer MD5
	Encryption::Algorithm::MD5 md5pass2;
	md5pass2.update(opad, 64);
	md5pass2.update(ustrResult, 16);
	md5pass2.finalize();
	
	const std::string decoded_str = login + " " + md5pass2.hex_digest();
	decoded_challenge.Assign((UnsignedByte*)decoded_str.c_str(), decoded_str.size());
	return Core::Utils::to_string(base64.Encode(decoded_challenge).data());
}

std::string Cout::Network::Protocol::Authentication::Method::DigestMD5(
	const std::string& encoded_challenge,
	const std::string& charset, 
	const std::string& addr, 
	const std::string& login, 
	const std::string& pass)
{
	Encryption::Algorithm::Base64 base64;
	UnsignedBinary temp;
	temp.Assign((UnsignedByte*)encoded_challenge.data(), encoded_challenge.size());
	temp = base64.Decode(temp);
	temp.push_back(0);

	std::string decoded_challenge = Core::Utils::to_string(temp.data());
	/////////////////////////////////////////////////////////////////////
	//Test data from RFC 2831
	//To test jump into authenticate and read this line and the ones down to next test data section
	//decoded_challenge = "realm=\"elwood.innosoft.com\",nonce=\"OA6MG9tEQGm2hh\",qop=\"auth\",algorithm=md5-sess,charset=utf-8";
	/////////////////////////////////////////////////////////////////////

	//Get the nonce (manditory)
	size_t find = decoded_challenge.find("nonce");
	if (find < 0)
		throw Exceptions::Auth::bad_digest_response(WHERE, "decoded challenge not contains nonce");
	std::string nonce = decoded_challenge.substr(find + 7);
	find = nonce.find("\"");
	if (find < 0)
		throw Exceptions::Auth::bad_digest_response(WHERE, "invalid decoded challenge");
	nonce = nonce.substr(0, find);

	//Get the realm (optional)
	std::string realm;
	find = decoded_challenge.find("realm");
	if (find >= 0) {
		realm = decoded_challenge.substr(find + 7);
		find = realm.find("\"");
		if (find < 0)
			throw Exceptions::Auth::bad_digest_response(WHERE, "invalid decoded challenge");
		realm = realm.substr(0, find);
	}

	//Create a cnonce
	std::stringstream tempn;
	tempn << std::hex << (unsigned int)time(NULL);
	std::string cnonce = tempn.str();

	//Set nonce count
	std::string nc = "00000001";

	//Set QOP
	std::string qop = "auth";

	// set uri
	std::string uri = "smtp/" + addr;

	/////////////////////////////////////////////////////////////////////
	//test data from RFC 2831
	//m_sLogin = "chris";
	//m_sPassword = "secret";
	//snprintf(cnonce, 17, "OA6MHXh6VqTrRk");
	//uri = "imap/elwood.innosoft.com";
	//Should form the response:
	//    charset=utf-8,username="chris",
	//    realm="elwood.innosoft.com",nonce="OA6MG9tEQGm2hh",nc=00000001,
	//    cnonce="OA6MHXh6VqTrRk",digest-uri="imap/elwood.innosoft.com",
	//    response=d388dad90d4bbd760a152321f2143af7,qop=auth
	//This encodes to:
	//    Y2hhcnNldD11dGYtOCx1c2VybmFtZT0iY2hyaXMiLHJlYWxtPSJlbHdvb2
	//    QuaW5ub3NvZnQuY29tIixub25jZT0iT0E2TUc5dEVRR20yaGgiLG5jPTAw
	//    MDAwMDAxLGNub25jZT0iT0E2TUhYaDZWcVRyUmsiLGRpZ2VzdC11cmk9Im
	//    ltYXAvZWx3b29kLmlubm9zb2Z0LmNvbSIscmVzcG9uc2U9ZDM4OGRhZDkw
	//    ZDRiYmQ3NjBhMTUyMzIxZjIxNDNhZjcscW9wPWF1dGg=
	/////////////////////////////////////////////////////////////////////

	//Calculate digest response
	unsigned char *ustrRealm = Cout::Core::Utils::StringToUnsignedChar(realm);
	unsigned char *ustrUsername = Cout::Core::Utils::StringToUnsignedChar(login);
	unsigned char *ustrPassword = Cout::Core::Utils::StringToUnsignedChar(pass);
	unsigned char *ustrNonce = Cout::Core::Utils::StringToUnsignedChar(nonce);
	unsigned char *ustrCNonce = Cout::Core::Utils::StringToUnsignedChar(cnonce);
	unsigned char *ustrUri = Cout::Core::Utils::StringToUnsignedChar(uri);
	unsigned char *ustrNc = Cout::Core::Utils::StringToUnsignedChar(nc);
	unsigned char *ustrQop = Cout::Core::Utils::StringToUnsignedChar(qop);
	if (!ustrRealm || !ustrUsername || !ustrPassword || !ustrNonce || !ustrCNonce || !ustrUri || !ustrNc || !ustrQop)
		throw Exceptions::Auth::bad_credentials(WHERE, "digest-m5 invalid decoded challenge");

	Encryption::Algorithm::MD5 md5a1a;
	md5a1a.update(ustrUsername, login.size());
	md5a1a.update((unsigned char*)":", 1);
	md5a1a.update(ustrRealm, realm.size());
	md5a1a.update((unsigned char*)":", 1);
	md5a1a.update(ustrPassword, pass.size());
	md5a1a.finalize();
	unsigned char *ua1 = md5a1a.raw_digest();

	Encryption::Algorithm::MD5 md5a1b;
	md5a1b.update(ua1, 16);
	md5a1b.update((unsigned char*)":", 1);
	md5a1b.update(ustrNonce, nonce.size());
	md5a1b.update((unsigned char*)":", 1);
	md5a1b.update(ustrCNonce, strlen(cnonce.c_str()));
	//authzid could be added here
	md5a1b.finalize();
	char *a1 = md5a1b.hex_digest();

	Encryption::Algorithm::MD5 md5a2;
	md5a2.update((unsigned char*) "AUTHENTICATE:", 13);
	md5a2.update(ustrUri, uri.size());
	//authint and authconf add an additional line here	
	md5a2.finalize();
	char *a2 = md5a2.hex_digest();

	delete[] ua1;
	ua1 = Cout::Core::Utils::StringToUnsignedChar(a1);
	unsigned char *ua2 = Cout::Core::Utils::StringToUnsignedChar(a2);

	//compute KD
	Encryption::Algorithm::MD5 md5;
	md5.update(ua1, 32);
	md5.update((unsigned char*)":", 1);
	md5.update(ustrNonce, nonce.size());
	md5.update((unsigned char*)":", 1);
	md5.update(ustrNc, strlen(nc.c_str()));
	md5.update((unsigned char*)":", 1);
	md5.update(ustrCNonce, strlen(cnonce.c_str()));
	md5.update((unsigned char*)":", 1);
	md5.update(ustrQop, qop.size());
	md5.update((unsigned char*)":", 1);
	md5.update(ua2, 32);
	md5.finalize();
	decoded_challenge = md5.hex_digest();
	
	std::stringstream resstream(charset);
	resstream << "username=\"" + login + "\"";
	if (!realm.empty()) {
		resstream << ",realm=\"" + realm + "\"";
	}
	resstream << ",nonce=\"" + nonce + "\"";
	resstream << ",nc=\"" + nc + "\"";
	resstream << ",cnonce=\"" + cnonce + "\"";
	resstream << ",digest-uri=\"" + uri + "\"";
	resstream << ",response=\"" + decoded_challenge + "\"";
	resstream << ",qop=\"" + qop + "\"";
	std::string resstr = resstream.str();
	UnsignedBinary ustrDigest;
	ustrDigest.Assign((UnsignedByte*)resstr.c_str(), resstr.size());
	return Core::Utils::to_string(base64.Encode(ustrDigest).data());
}

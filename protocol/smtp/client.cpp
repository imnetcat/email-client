#include "client.h"
#include "../../core/config.h"
using namespace std;

Protocol::SMTP::Client::Client()
{
	DEBUG_LOG(3, "Initializing SMTP Client");
}

void Protocol::SMTP::Client::Send(MAIL* mail)
{
	mail->SetXPriority(Protocol::SMTP::MAIL::PRIORITY::NORMAL);
	mail->SetXMailer("My email client");
	_component->Connect(_component->host, _component->port);
	_component->Send(mail);
	_component->Disconnect();
}

void Protocol::SMTP::Client::SetLogin(const string& login)
{
	_component->SetLogin(login);
}

void Protocol::SMTP::Client::SetPassword(const string& pass)
{
	_component->SetPassword(pass);
}

void Protocol::SMTP::Client::SetServer(Server::ID id)
{
	_component->host = supported.at(id).host;
	_component->port = supported.at(id).port;
	_component->sec = supported.at(id).sec;
}

const std::map<const Protocol::SMTP::Server::ID, const Protocol::SMTP::Server> Protocol::SMTP::Client::supported = {
	{
		Protocol::SMTP::Server::GMAIL_TLS,
		{
			Secured::Type::TLS,	"smtp.gmail.com",			587
		}
	},
	{
		Protocol::SMTP::Server::GMAIL_SSL,
		{
			Secured::Type::SSL,	"smtp.gmail.com",			465
		}
	},
	{
		Protocol::SMTP::Server::HOTMAIL_TSL,
		{
			Secured::Type::TLS,	"smtp.live.com",			25
		}
	},
	{
		Protocol::SMTP::Server::AOL_TLS,
		{
			Secured::Type::TLS,	"smtp.aol.com",				587
		}
	},
	{
		Protocol::SMTP::Server::YAHOO_SSL,
		{
			Secured::Type::SSL,	"plus.smtp.mail.yahoo.com",	465
		}
	}
};

#include "email.h"
using namespace std;

EMAIL::EMAIL() { }

EMAIL::~EMAIL() { }

void EMAIL::AddAttachment(const string& Path) noexcept
{
	mail.attachments.insert(mail.attachments.end(), Path);
}

void EMAIL::AddRecipient(const string& email, const string& name)
{
	if (email.empty())
		throw CORE::UNDEF_RECIPIENT_MAIL;

	SMTP::MAIL::Recipient recipient;
	recipient.Mail = email;
	if (!name.empty()) 
		recipient.Name = name;

	mail.recipients.insert(mail.recipients.end(), recipient);
}

void EMAIL::AddCCRecipient(const string& email, const string& name)
{
	if (email.empty())
		throw CORE::UNDEF_RECIPIENT_MAIL;

	SMTP::MAIL::Recipient recipient;
	recipient.Mail = email;
	if (!name.empty()) 
		recipient.Name = name;

	mail.ccrecipients.insert(mail.ccrecipients.end(), recipient);
}

void EMAIL::AddBCCRecipient(const string& email, const string& name)
{
	if (email.empty())
		throw CORE::UNDEF_RECIPIENT_MAIL;

	SMTP::MAIL::Recipient recipient;
	recipient.Mail = email;
	if (!name.empty()) 
		recipient.Name = name;

	mail.bccrecipients.insert(mail.bccrecipients.end(), recipient);
}

void EMAIL::AddMsgLine(const string& Text) noexcept
{
	mail.body.insert(mail.body.end(), Text);
}

void EMAIL::DelMsgLine(unsigned int Line)
{
	if (Line >= mail.body.size())
		throw CORE::OUT_OF_VECTOR_RANGE;
	mail.body.erase(mail.body.begin() + Line);
}

void EMAIL::DelRecipients() noexcept
{
	mail.recipients.clear();
}

void EMAIL::DelBCCRecipients() noexcept
{
	mail.bccrecipients.clear();
}

void EMAIL::DelCCRecipients() noexcept
{
	mail.ccrecipients.clear();
}

void EMAIL::DelMsgLines() noexcept
{
	mail.body.clear();
}

void EMAIL::DelAttachments() noexcept
{
	mail.attachments.clear();
}

void EMAIL::ModMsgLine(unsigned int Line, const char* Text)
{
	if (Line >= mail.body.size())
		throw CORE::OUT_OF_VECTOR_RANGE;
	mail.body.at(Line) = std::string(Text);
}

void EMAIL::ClearMessage() noexcept
{
	DelRecipients();
	DelBCCRecipients();
	DelCCRecipients();
	DelAttachments();
	DelMsgLines();
}


size_t EMAIL::GetRecipientCount() const noexcept
{
	return mail.recipients.size();
}

size_t EMAIL::GetBCCRecipientCount() const noexcept
{
	return mail.bccrecipients.size();
}

size_t EMAIL::GetCCRecipientCount() const noexcept
{
	return mail.ccrecipients.size();
}

const char* EMAIL::GetReplyTo() const noexcept
{
	return mail.replyTo.c_str();
}

const char* EMAIL::GetMailFrom() const noexcept
{
	return mail.senderMail.c_str();
}

const char* EMAIL::GetSenderName() const noexcept
{
	return mail.senderName.c_str();
}

const char* EMAIL::GetSubject() const noexcept
{
	return mail.subject.c_str();
}

const char* EMAIL::GetXMailer() const noexcept
{
	return mail.mailer.c_str();
}

SMTP::MAIL::PRIORITY EMAIL::GetXPriority() const noexcept
{
	return mail.priority;
}

const char* EMAIL::GetMsgLineText(unsigned int Line) const noexcept
{
	return mail.body.at(Line).c_str();
}

size_t EMAIL::GetMsgLines() const noexcept
{
	return mail.body.size();
}

void EMAIL::SetCharSet(const string& sCharSet) noexcept
{
	mail.charSet = sCharSet;
}

const map<EMAIL::SERVER_ID, EMAIL::SUPPORTED_SERVER>& EMAIL::GetSupportedServers() const noexcept
{
	return supported_servers;
}

void EMAIL::SetXPriority(SMTP::MAIL::PRIORITY priority) noexcept
{
	mail.priority = priority;
}

void EMAIL::SetReplyTo(const string& ReplyTo) noexcept
{
	mail.replyTo = ReplyTo;
}

void EMAIL::SetReadReceipt(bool requestReceipt) noexcept
{
	mail.readReceipt = requestReceipt;
}

void EMAIL::SetSenderMail(const string& SMail) noexcept
{
	mail.senderMail = SMail;
}

void EMAIL::SetSenderName(const string& Name) noexcept
{
	mail.senderName = Name;
}

void EMAIL::SetSubject(const string& Subject) noexcept
{
	mail.subject = Subject;
}

void EMAIL::SetXMailer(const string& XMailer) noexcept
{
	mail.mailer = XMailer;
}

void EMAIL::SetAuth(const string& login, const string& pass) noexcept
{
	mail.senderLogin = login;
	mail.senderPass = pass;
}

void EMAIL::SetSecurity(ESMTPSA::SMTP_SECURITY_TYPE type) noexcept
{
	security = type;
}

void EMAIL::SetServer(SERVER_ID id) noexcept
{
	smtp_server = id;
	reqExt = supported_servers.at(id).reqExt;
	reqSecure = supported_servers.at(id).sec != ESMTPS::NO_SECURITY;
	reqAuth = supported_servers.at(id).isAuth;
	SetSecurity(supported_servers.at(id).sec);
}

bool EMAIL::IsAuthRequired() const noexcept
{
	return reqAuth;
}
bool EMAIL::IsExtRequired() const noexcept
{
	return reqExt;
}
bool EMAIL::IsEncrypRequired() const noexcept
{
	return reqSecure;
}

shared_ptr<SMTP> EMAIL::getOptimalProtocol() const noexcept
{
	if (!reqExt)
	{
		return make_shared<SMTP>();
	}

	if (!reqAuth && reqSecure)
	{
		return make_shared<ESMTPS>();
	}
	else if (reqAuth && reqSecure)
	{
		return make_shared<ESMTPSA>();
	}

	return make_shared<ESMTP>();

}

void EMAIL::send() const
{
	if (mail.senderMail.empty())
		throw CORE::EMAIL_UNDEF_SENDER;
	if (mail.recipients.empty())
		throw CORE::EMAIL_UNDEF_RECEIVER;
	
	if (reqSecure && security == ESMTPSA::NO_SECURITY)
		throw CORE::SMTP_CREATE_HEADER; // TODO: another error name
	if (reqAuth && !mail.senderLogin.size())
		throw CORE::SMTP_CREATE_HEADER; // TODO: another error name

	const SUPPORTED_SERVER server = supported_servers.at(smtp_server);
	
	shared_ptr<SMTP> mailer = getOptimalProtocol();
	
	mailer->Connect();
	
	mailer->SetSMTPServer(server.port, server.address);

	mailer->SendMail(mail);
	
	mailer->Disconnect();
}

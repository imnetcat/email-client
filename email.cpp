#include "email.h"
using namespace std;

EMAIL::EMAIL()
{
	mail = {};
}

EMAIL::~EMAIL()
{
	
}

void EMAIL::AddAttachment(const char *Path)
{
	mail.attachments.insert(mail.attachments.end(), Path);
}

RETCODE EMAIL::AddRecipient(const string& email, const string& name)
{
	if (email.empty())
		return FAIL(UNDEF_RECIPIENT_MAIL);

	SMTP::MAIL::Recipient recipient;
	recipient.Mail = email;
	if (!name.empty()) 
		recipient.Name = name;

	mail.recipients.insert(mail.recipients.end(), recipient);

	return SUCCESS;
}

RETCODE EMAIL::AddCCRecipient(const string& email, const string& name)
{
	if (email.empty())
		return FAIL(UNDEF_RECIPIENT_MAIL);

	SMTP::MAIL::Recipient recipient;
	recipient.Mail = email;
	if (!name.empty()) 
		recipient.Name = name;

	mail.ccrecipients.insert(mail.ccrecipients.end(), recipient);

	return SUCCESS;
}

RETCODE EMAIL::AddBCCRecipient(const string& email, const string& name)
{
	if (email.empty())
		return FAIL(UNDEF_RECIPIENT_MAIL);

	SMTP::MAIL::Recipient recipient;
	recipient.Mail = email;
	if (!name.empty()) 
		recipient.Name = name;

	mail.bccrecipients.insert(mail.bccrecipients.end(), recipient);

	return SUCCESS;
}

void EMAIL::AddMsgLine(const char* Text)
{
	mail.body.insert(mail.body.end(), Text);
}

RETCODE EMAIL::DelMsgLine(unsigned int Line)
{
	if (Line >= mail.body.size())
		return FAIL(OUT_OF_VECTOR_RANGE);
	mail.body.erase(mail.body.begin() + Line);

	return SUCCESS;
}

void EMAIL::DelRecipients()
{
	mail.recipients.clear();
}

void EMAIL::DelBCCRecipients()
{
	mail.bccrecipients.clear();
}

void EMAIL::DelCCRecipients()
{
	mail.ccrecipients.clear();
}

void EMAIL::DelMsgLines()
{
	mail.body.clear();
}

void EMAIL::DelAttachments()
{
	mail.attachments.clear();
}

RETCODE EMAIL::ModMsgLine(unsigned int Line, const char* Text)
{
	if (Text)
	{
		if (Line >= mail.body.size())
			return FAIL(OUT_OF_VECTOR_RANGE);
		mail.body.at(Line) = std::string(Text);
	}

	return SUCCESS;
}

void EMAIL::ClearMessage()
{
	DelRecipients();
	DelBCCRecipients();
	DelCCRecipients();
	DelAttachments();
	DelMsgLines();
}


size_t EMAIL::GetRecipientCount() const
{
	return mail.recipients.size();
}

size_t EMAIL::GetBCCRecipientCount() const
{
	return mail.bccrecipients.size();
}

size_t EMAIL::GetCCRecipientCount() const
{
	return mail.ccrecipients.size();
}

const char* EMAIL::GetReplyTo() const
{
	return mail.replyTo.c_str();
}

const char* EMAIL::GetMailFrom() const
{
	return mail.senderMail.c_str();
}

const char* EMAIL::GetSenderName() const
{
	return mail.senderName.c_str();
}

const char* EMAIL::GetSubject() const
{
	return mail.subject.c_str();
}

const char* EMAIL::GetXMailer() const
{
	return mail.mailer.c_str();
}

SMTP::MAIL::CSmptXPriority EMAIL::GetXPriority() const
{
	return mail.priority;
}

const char* EMAIL::GetMsgLineText(unsigned int Line) const
{
	return mail.body.at(Line).c_str();
}

size_t EMAIL::GetMsgLines() const
{
	return mail.body.size();
}

void EMAIL::SetCharSet(const string& sCharSet)
{
	mail.charSet = sCharSet;
}


void EMAIL::SetXPriority(SMTP::MAIL::CSmptXPriority priority)
{
	mail.priority = priority;
}

void EMAIL::SetReplyTo(const string& ReplyTo)
{
	mail.replyTo = ReplyTo;
}

void EMAIL::SetReadReceipt(bool requestReceipt/*=true*/)
{
	mail.readReceipt = requestReceipt;
}

void EMAIL::SetSenderMail(const string& SMail)
{
	mail.senderMail = SMail;
}

void EMAIL::SetSenderName(const string& Name)
{
	mail.senderName = Name;
}

void EMAIL::SetSubject(const string& Subject)
{
	mail.subject = Subject;
}

void EMAIL::SetXMailer(const string& XMailer)
{
	mail.mailer = XMailer;
}

RETCODE EMAIL::createHeader()
{
	char month[][4] = { "Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec" };
	size_t i;
	stringstream to;
	stringstream cc;
	stringstream bcc;
	stringstream sheader;
	time_t rawtime;
	struct tm* timeinfo = nullptr;

	// date/time check
	if (time(&rawtime) > 0)
		localtime_s(timeinfo, &rawtime);
	else
		return FAIL(TIME_ERROR);

	// check for at least one recipient
	if (mail.recipients.size())
	{
		for (i = 0; i < mail.recipients.size(); i++)
		{
			if (i > 0)
				to << ',';
			to << mail.recipients[i].Name;
			to << '<';
			to << mail.recipients[i].Mail;
			to << '>';
		}
	}
	else
		return FAIL(UNDEF_RECIPIENTS);

	if (mail.ccrecipients.size())
	{
		for (i = 0; i < mail.ccrecipients.size(); i++)
		{
			if (i > 0)
				cc << ',';
			cc << mail.ccrecipients[i].Name;
			cc << '<';
			cc << mail.ccrecipients[i].Mail;
			cc << '>';
		}
	}

	if (mail.bccrecipients.size())
	{
		for (i = 0; i < mail.bccrecipients.size(); i++)
		{
			if (i > 0)
				bcc << ',';
			bcc << mail.bccrecipients[i].Name;
			bcc << '<';
			bcc << mail.bccrecipients[i].Mail;
			bcc << '>';
		}
	}

	// Date: <SP> <dd> <SP> <mon> <SP> <yy> <SP> <hh> ":" <mm> ":" <ss> <SP> <zone> <CRLF>
	sheader << "Date: " <<
		timeinfo->tm_mday << " " <<
		month[timeinfo->tm_mon] << " " <<
		timeinfo->tm_year + 1900 << " " <<
		timeinfo->tm_hour << ":" <<
		timeinfo->tm_min << ":" <<
		timeinfo->tm_sec << "\r\n";
	// From: <SP> <sender>  <SP> "<" <sender-email> ">" <CRLF>
	if (!mail.senderMail.size()) return FAIL(UNDEF_MAIL_FROM);

	sheader << "From: ";
	if (mail.senderName.size()) sheader << mail.senderName;

	sheader << " <";
	sheader << mail.senderMail;
	sheader << ">\r\n";

	// X-Mailer: <SP> <xmailer-app> <CRLF>
	if (mail.mailer.size())
	{
		sheader << "X-Mailer: ";
		sheader << mail.mailer;
		sheader << "\r\n";
	}

	// Reply-To: <SP> <reverse-path> <CRLF>
	if (mail.replyTo.size())
	{
		sheader << "Reply-To: ";
		sheader << mail.replyTo;
		sheader << "\r\n";
	}

	// Disposition-Notification-To: <SP> <reverse-path or sender-email> <CRLF>
	if (mail.readReceipt)
	{
		sheader << "Disposition-Notification-To: ";
		if (mail.replyTo.size()) sheader << mail.replyTo;
		else sheader << mail.senderName;
		sheader << "\r\n";
	}

	// X-Priority: <SP> <number> <CRLF>
	switch (mail.priority)
	{
	case SMTP::MAIL::XPRIORITY_HIGH:
		sheader << "X-Priority: 2 (High)\r\n";
		break;
	case SMTP::MAIL::XPRIORITY_NORMAL:
		sheader << "X-Priority: 3 (Normal)\r\n";
		break;
	case SMTP::MAIL::XPRIORITY_LOW:
		sheader << "X-Priority: 4 (Low)\r\n";
		break;
	default:
		sheader << "X-Priority: 3 (Normal)\r\n";
	}

	// To: <SP> <remote-user-mail> <CRLF>
	sheader << "To: ";
	sheader << to.str();
	sheader << "\r\n";

	// Cc: <SP> <remote-user-mail> <CRLF>
	if (mail.ccrecipients.size())
	{
		sheader << "Cc: ";
		sheader << cc.str();
		sheader << "\r\n";
	}

	if (mail.bccrecipients.size())
	{
		sheader << "Bcc: ";
		sheader << bcc.str();
		sheader << "\r\n";
	}

	// Subject: <SP> <subject-text> <CRLF>
	if (!mail.subject.size())
		sheader << "Subject:  ";
	else
	{
		sheader << "Subject: ";
		sheader << mail.subject;
	}
	sheader << "\r\n";

	// MIME-Version: <SP> 1.0 <CRLF>
	sheader << "MIME-Version: 1.0\r\n";
	if (!mail.attachments.size())
	{ // no attachments
		if (mail.html) sheader << "Content-Type: text/html; charset=\"";
		else sheader << "Content-type: text/plain; charset=\"";
		sheader << mail.charSet;
		sheader << "\"\r\n";
		sheader << "Content-Transfer-Encoding: 7bit\r\n";
		sheader << "\r\n";
	}
	else
	{ // there is one or more attachments
		sheader << "Content-Type: multipart/mixed; boundary=\"";
		sheader << SMTP::BOUNDARY_TEXT;
		sheader << "\"\r\n";
		sheader << "\r\n";
		// first goes text message
		sheader << "--";
		sheader << SMTP::BOUNDARY_TEXT;
		sheader << "\r\n";
		if (mail.html) sheader << "Content-type: text/html; charset=";
		else sheader << "Content-type: text/plain; charset=";
		sheader << mail.charSet;
		sheader << "\r\n";
		sheader << "Content-Transfer-Encoding: 7bit\r\n";
		sheader << "\r\n";
	}

	sheader << '\0';

	mail.header = sheader.str();
	return SUCCESS;
}

void EMAIL::SetAuth(const string& login, const string& pass)
{
	mail.senderLogin = login;
	mail.senderPass = pass;
}

void EMAIL::SetSecurity(ESMTPSA::SMTP_SECURITY_TYPE type)
{
	security = type;
}

void EMAIL::useGmail()
{
	smtp_server = GMAIL;
	reqExt = true;
	reqSecure = true;
	reqAuth = true;
}
void EMAIL::useHotmail()
{
	smtp_server = HOTMAIL;
	reqExt = true;
	reqSecure = true;
	reqAuth = true;
}
void EMAIL::useAol()
{
	smtp_server = AOL;
	reqExt = true;
	reqSecure = true;
	reqAuth = true;
}
void EMAIL::useYahoo()
{
	smtp_server = YAHOO;
	reqExt = true;
	reqSecure = true;
	reqAuth = true;
}

shared_ptr<SMTP> EMAIL::getOptimalProtocol()
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

RETCODE EMAIL::send() {
	if (mail.senderMail.empty())
		return FAIL(EMAIL_UNDEF_SENDER);
	if (mail.recipients.empty())
		return FAIL(EMAIL_UNDEF_RECEIVER);
	if (createHeader())
		return FAIL(SMTP_CREATE_HEADER);
	
	if (reqSecure && security == ESMTPSA::NO_SECURITY)
		return FAIL(SMTP_CREATE_HEADER); // TODO: another error name
	if (reqAuth && !mail.senderLogin.size())
		return FAIL(SMTP_CREATE_HEADER); // TODO: another error name

	const SUPPORTED_SERVERS_ADDR server = supported_servers[smtp_server][security];
	
	shared_ptr<SMTP> mailer = getOptimalProtocol();
	
	mailer->Connect();
	
	mailer->SetSMTPServer(server.port, server.name);

	mailer->SendMail(mail);
	
	return SUCCESS;
}

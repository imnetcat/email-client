# **Email client**
  Реализация фреймворка для отправки емейлов на почтовые сервисы (список поддерживаемых ниже) по протоколу SMTP.
  Поддерживаются протоколы SMTP(1), ESMTP (2), ESMTPA (3) и ESMTPS с использованием OpenSSL, а так же ESMTPSA (ESMTP with authentication & security plagins)
  
(1) Simple Mail Transfer Protocol (в соответствии с спецификацией [RFC 5321](https://tools.ietf.org/html/rfc5321 "Documentation"), [RFC 7504](https://tools.ietf.org/html/rfc7504 "Documentation"))

(2) Extended SMTP (в соответствии с спецификацией [RFC 1869](https://tools.ietf.org/html/rfc1869 "Documentation"))

(3) ESMTP with authentication plagin (в соответствии с спецификацией [RFC 4954](http://www.rfc-editor.org/rfc/rfc4954 "Documentation"))

(3) ESMTP with security plagin SSL/TLS (в соответствии с спецификацией [RFC 3207](https://tools.ietf.org/html/rfc3207 "Documentation"), [RFC 7817](https://tools.ietf.org/html/rfc7817 "Documentation"))
## **Platform**
Desktop Windows-based x32-64

## tested
- Desktop Windows 7 x64
## not tested (но должно работать:/)
- Desktop Windows XP x64
- Desktop Windows XP x32
- Desktop Windows 7 x32
- Desktop Windows 8 x64
- Desktop Windows 8 x32
- Desktop Windows 8.1 x64
- Desktop Windows 8.1 x32
- Desktop Windows 10 x64
- Desktop Windows 10 x32

## **Dependencies**
- c++17 и выше
- STL
- OpenSSL

## **Functional**
Поддерживаемые почтовые сервисы:
- gmail 
- hotmail
- aol
- yahoo

Поддержка обычной, ВСС (Blind Carbon Copy) и СС (Carbon Copy) рассылки

Поддержка прикриплённых файлов всех возможных форматов

## **Docs**
Необходимый заголовочный файл
```
#include "email-client/email.h"
```

*Ремарочка: компилировать с помощью MVSC++ 2017 или выше


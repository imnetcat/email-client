#pragma once
#include "../../core/types/binary.h"
namespace Cout
{
	namespace Network
	{
		namespace Protocol
		{
			class IProtocol
			{
			public:
				virtual void Connect(const std::string& host, unsigned short port) = 0;
				virtual void Disconnect() = 0;
				virtual void Send() = 0;
				virtual void Receive() = 0;
			};
		}
	}
}
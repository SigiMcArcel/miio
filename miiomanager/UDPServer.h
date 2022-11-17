#pragma once
#include <string>
#include <array>
#include <mi/miutils/Event.h>

namespace miDriver
{
	typedef enum class UdpServerResults_t
	{
		Ok,
		ErrorSetup

	}UdpServerResults;

	typedef std::string UDPServerIp;
	typedef uint8_t UDPServerPort;

	class UDPackage
	{
	private:
	public:
		UDPackage(std::string byteString, UDPServerPort port, UDPServerIp address);
		UDPackage(uint8_t* byteArray, UDPServerPort port, UDPServerIp address);
	};

	class UDPServer
	{
	private:
		

		UDPServerIp _BindAddress;
		UDPServerPort _Port;

	public:
		UDPServer(UDPServerPort port, UDPServerIp bind)
			:_BindAddress(bind)
			, _Port(port)
		{

		}
		UdpServerResults Start();
		UdpServerResults Stop();
		UdpServerResults Send();


	};
}



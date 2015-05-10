// Preprocessor related:
//#define QSOCK_LOCAL_BUILD

// Includes:
#ifdef QSOCK_LOCAL_BUILD
	#include "Local/QuickSock.h"
	
	#if QSOCK_THROW_EXCEPTIONS
		#include "Local/Exceptions.h"
	#endif
#else
	#include <QuickLib/QuickSock/QuickSock.h>
	
	#if QSOCK_THROW_EXCEPTIONS
		#include <QuickLib/QuickSock/Exceptions.h>
	#endif
#endif

#ifndef WORDS_BIGENDIAN
	#if defined(QSOCK_WINDOWS_LEGACY) // || defined(CFG_GLFW_USE_MINGW) && defined(QSOCK_MONKEYMODE)
		uqint htonf(const qfloat inFloat)
		{
			// Local variable(s):
			uqint retVal = 0;

			uqchar* floatToConvert = (uqchar*)&inFloat;
			uqchar* returnFloat = (uqchar*)&retVal;
		
			for (uqchar i = 0; i < sizeof(inFloat); i++)
				returnFloat[i] = floatToConvert[(sizeof(inFloat)-1)-i];
								
			return retVal;
		}
		
		qfloat ntohf(const uqint inFloat)
		{
			// Local variable(s):
			qfloat retVal = 0.0;

			uqchar* floatToConvert = (uqchar*) & inFloat;
			uqchar* returnFloat = (uqchar*) & retVal;
		
			for (uqchar i = 0; i < sizeof(inFloat); i++)
				returnFloat[i] = floatToConvert[(sizeof(inFloat)-1)-i];
								
			return retVal;
		}
		
		inline QSOCK_UINT64 htond(const double inFloat)
		{
			// Local variable(s):
			QSOCK_UINT64 retVal = 0;

			uqchar* floatToConvert = (uqchar*)&inFloat;
			uqchar* returnFloat = (uqchar*)&retVal;
		
			for (uqchar i = 0; i < sizeof(inFloat); i++)
				returnFloat[i] = floatToConvert[(sizeof(inFloat)-1)-i];
								
			return retVal;
		}
		
		inline qdouble ntohd(const QSOCK_UINT64 inFloat)
		{
			// Local variable(s):
			qdouble retVal = 0.0;

			uqchar* floatToConvert = (uqchar*)&inFloat;
			uqchar* returnFloat = (uqchar*)&retVal;
		
			for (uqchar i = 0; i < sizeof(inFloat); i++)
				returnFloat[i] = floatToConvert[(sizeof(inFloat)-1)-i];
								
			return retVal;
		}
	#endif
#endif

// Namespace(s):
namespace quickLib
{
	namespace sockets
	{
		// Classes:

		// Exceptions:

		// QSOCK_EXCEPTION:

		// Constructor(s):
		QSOCK_EXCEPTION::QSOCK_EXCEPTION(const QSocket* target, const std::string& exception_name)
			: std::runtime_error(exception_name), sock(target) { /* Nothing so far. */ }

		// QSOCK_CONSTRUCTION_EXCEPTION:

		// Constructor(s):
		QSOCK_CONSTRUCTION_EXCEPTION::QSOCK_CONSTRUCTION_EXCEPTION(const QSocket* target) : QSOCK_EXCEPTION(target, "QuickSock: Construction exception.") { /* Nothing so far. */ }

		// QSOCK_OUT_OF_BOUNDS_EXCEPTION:

		// Constructor(s):
		QSOCK_OUT_OF_BOUNDS_EXCEPTION::QSOCK_OUT_OF_BOUNDS_EXCEPTION(const QSocket* target, void* targetedBuffer, size_type bufferSize, size_type offsetInBuffer, const std::string& exception_name)
			: QSOCK_EXCEPTION(target, exception_name), buffer(targetedBuffer), sizeofBuffer(bufferSize), relativePosition(offsetInBuffer) { /* Nothing so far. */ }

		// QSOCK_SEEK_EXCEPTION:

		// Constructor(s):
		QSOCK_SEEK_EXCEPTION::QSOCK_SEEK_EXCEPTION(const QSocket* target, size_type bufferSize, size_type offsetInBuffer, seekMode mode, void* targetedBuffer, const std::string& exception_name)
			: QSOCK_OUT_OF_BOUNDS_EXCEPTION(target, targetedBuffer, bufferSize, offsetInBuffer, exception_name) { /* Nothing so far. */ }

		QSOCK_SEEK_EXCEPTION::QSOCK_SEEK_EXCEPTION(const QSocket* target, seekMode mode, size_type position, const std::string& exception_name)
			: QSOCK_OUT_OF_BOUNDS_EXCEPTION
			(
				target,
				(mode == SEEK_MODE_IN) ?
					(target->inbuffer) : (target->outbuffer),
				(mode == SEEK_MODE_IN) ?
					(target->inbufferlen) : (target->outbufferlen),
				position,

				exception_name
			) { /* Nothing so far. */ }

		// QSOCK_READ_EXCEPTION:

		// Constructor(s):
		QSOCK_READ_EXCEPTION::QSOCK_READ_EXCEPTION(const QSocket* target, void* targetedBuffer, size_type bufferSize, size_type offsetInBuffer, const std::string& exception_name)
			: QSOCK_OUT_OF_BOUNDS_EXCEPTION(target, targetedBuffer, bufferSize, offsetInBuffer, exception_name) { /* Nothing so far. */ }

		// QSOCK_WRITE_EXCEPTION:

		// Constructor(s):
		QSOCK_WRITE_EXCEPTION::QSOCK_WRITE_EXCEPTION(const QSocket* target, void* targetedBuffer, size_type bufferSize, size_type offsetInBuffer, const std::string& exception_name)
			: QSOCK_OUT_OF_BOUNDS_EXCEPTION(target, targetedBuffer, bufferSize, offsetInBuffer, exception_name) { /* Nothing so far. */ }

		// QStream:

		// Constructor(s) (Public):
		QStream::QStream(uqchar* inputBuffer, uqchar* outputBuffer, size_type inBuffer_size, size_type outBuffer_size, bool fixBOrder)
			: inbuffer(inputBuffer), outbuffer(outputBuffer), inbufferSize(inBuffer_size), outbufferSize(outBuffer_size), fixByteOrder(fixBOrder) { /* Nothing so far. */ }

		QStream::QStream(size_type inBuffer_size, size_type outBuffer_size, bool fixBOrder)
			: inbufferSize(inBuffer_size), outbufferSize(outBuffer_size), fixByteOrder(fixBOrder)
		{
			// Allocate the requested buffers.
			allocateBuffers(inBuffer_size, outBuffer_size);
		}

		QStream::QStream(uqchar* inputBuffer, size_type inBuffer_size, bool fixBOrder)
			: QStream(inputBuffer, nullptr, inBuffer_size, 0, fixByteOrder) { /* Nothing so far. */ }

		QStream::QStream(size_type buffer_size, bool fixBOrder)
			: QStream(buffer_size, buffer_size, fixByteOrder) { /* Nothing so far. */ }

		QStream::QStream(const QStream& copyDataFrom)
		{
			// Make a shallow copy of the input.
			*this = copyDataFrom;

			// Allocate parody buffers.
			allocateBuffers(inbufferSize, outbufferSize);

			// Copy the contents of the input's buffers:
			memcpy(inbuffer, copyDataFrom.inbuffer, inbufferSize);
			memcpy(outbuffer, copyDataFrom.outbuffer, outbufferSize);
		}

		// Destructor(s):
		QStream::~QStream()
		{
			// Deallocate the internal buffers.
			deallocateBuffers();
		}

		// Methods (Public):

		// Stream management routines:
		bool QStream::clearInBuffer() { std::memset(inbuffer, 0, inbufferSize); flushInput(); return true; }
		bool QStream::clearOutBuffer() { std::memset(outbuffer, 0, outbufferSize); flushOutput(); return true; }

		// Input routines:
		bool QStream::readData(void* output, size_type size, size_type output_offset, bool checkRead)
		{
			// Ensure we can read more of the internal buffer:
			#ifndef QSOCK_THROW_EXCEPTIONS
				if (!checkRead || canRead(size))
			#endif
				{
					// Hold the current input-position, so we can attempt to seek safely.
					auto currentOffset = readOffset;

					// Seek forward the amount we're going to transfer.
					inSeekForward(size);
					
					// Transfer from the input-buffer to the output.
					size_type transferred = rawTransfer(inbuffer, output, size, currentOffset, output_offset);

					return (transferred == size); // true;
				}

			// Return the default response.
			return false;
		}

		bool QStream::UreadBytes(uqchar* output, size_type count, size_type output_offset, bool checkRead)
		{
			return readData(output, (count == 0) ? inBytesLeft() : count, output_offset, checkRead);
		}

		uqchar* QStream::UreadBytes(size_type count, bool zero_ended)
		{
			if (!canRead(count))
				return nullptr;

			uqchar* data;

			// Create a new array to use as an output:
			if (zero_ended)
			{
				data = new uqchar[count + 1];
				data[count] = (uqchar)'\0';
			}
			else
			{
				data = new uqchar[count];
			}

			// Call the main implementation.
			UreadBytes(data, count, 0, false);

			return data;
		}

		nativeIP QStream::readIP()
		{
			switch (read<addressType>())
			{
				case ADDRESS_TYPE_INTEGER:
					#if !defined(QSOCK_IPVABSTRACT)
						return read<nativeIP>();
					#else
						return basic_socket::nonNativeToNativeIP(read<nonNativeIP>());
					#endif
				case ADDRESS_TYPE_STRING:
					#if !defined(QSOCK_IPVABSTRACT)
						return basic_socket::nonNativeToNativeIP(readString());
					#else
						return (nativeIP)readString();
					#endif
			}

			// If all else fails, return a blank IP address.
			return nativeIP();
		}

		// Line related:
		nativeString QStream::readLine()
		{
			// Namespace(s):
			using namespace std;

			// Definitions:
			auto count = inbufferlen-readOffset;

			if (count <= 0)
				return nativeString();

			QSOCK_UINT32 strLen = 0;
			uqchar* bytePosition = nullptr;

			for (size_type index = readOffset; index < inbufferlen; index++)
			{
				bytePosition = inbuffer+index;
		
				// Search for the correct bytes to end the line:
				if (*bytePosition == (uqchar)'\r' && (index+1) <= inbufferlen)
				{
					// If we've found the final byte, break.
					if (*(bytePosition++) == (uqchar)'\n')
						break;
				}

				// Add to the final string-length.
				strLen++;
			}

			// Allocate the output-string.
			nativeString output;

			// Copy part of the contents of 'inbuffer' to our output 'string'.
			output.assign((QSOCK_CHAR*)(inbuffer+readOffset), strLen);

			// Add to the offset.
			readOffset += strLen;

			// Return the output.
			return output;
		}

		// Output routines:

		// The generic writing command for raw data ('size' is in bytes):
		bool QStream::writeData(const void* input, size_type size, size_type input_offset)
		{
			// Ensure we can write more into the internal buffer:
			#ifndef QSOCK_THROW_EXCEPTIONS
				if (canWrite(size))
			#endif
				{
					// Hold the current output-position, so we can attempt to seek safely.
					auto currentOffset = writeOffset;

					// Seek forward the amount we're going to transfer.
					outSeekForward(size);

					// Transfer from the input to the output-buffer.
					size_type transferred = rawTransfer(input, outbuffer, size, input_offset, currentOffset);

					return (transferred == size); // true;
				}

			// Return the default response.
			return false;
		}

		bool QStream::writeBytes(const qchar* data, size_type dataSize) { return writeData(data, (dataSize == 0) ? strlen((const QSOCK_CHAR*)data) : dataSize); }
		bool QStream::UwriteBytes(const uqchar* data, size_type dataSize) { return writeData(data, (dataSize == 0) ? strlen((const QSOCK_CHAR*)data) : dataSize); }

		bool QStream::padBytes(size_type amount)
		{
			if (amount == 0)
			{
				if (writeOffset == outbufferSize)
					return false;

				amount = (outbufferSize-writeOffset);
			}

			// Make sure we can write the amount specified:
			if (!canWrite(amount))
				return false;

			// Flush/pad-out the number of bytes specified.
			flushOutputRegion(amount);

			// Return the default response.
			return true;
		}

		// Line related:
		bool QStream::writeLine(const QSOCK_CHAR* strIn, size_type length)
		{
			// Definition(s):
			bool response = false;

			// Write all of the bytes in strIn to the 'outbuffer'.
			response = writeBytes(strIn, length);
			if (!response) return response;

			// Setup the end of the line:
			response = writeChar('\r');
			if (!response) return response;
	
			response = writeChar('\n');

			// Return the response.
			return response;
		}

		// Constructor(s) (Protected):
		void QStream::allocateBuffers(size_type inBuffer_size, size_type outBuffer_size)
		{
			// Generate the requested buffers.
			if (inBuffer_size > 0)
				inbuffer = new uqchar[inBuffer_size];
			else
				inbuffer = nullptr;

			if (outBuffer_size > 0)
				outbuffer = new uqchar[outBuffer_size];
			else
				outbuffer = nullptr;

			return;
		}

		// Destructor(s) (Protected):
		void QStream::deallocateBuffers()
		{
			if (inbuffer != nullptr)
			{
				flushInput();

				delete[] inbuffer; inbuffer = nullptr;

				inbufferSize = 0;
			}

			if (outbuffer != nullptr)
			{
				flushOutput();

				delete[] outbuffer; outbuffer = nullptr;

				outbufferSize = 0;
			}

			return;
		}

		// Methods (Protected):
		// Nothing so far.

		// basic_socket:

		// Global variables:
		#if defined(QSOCK_WINDOWS)
			// An internal data-structure required for 'WinSock'. (Used for meta-data)
			WSADATA* basic_socket::WSA_Data = nullptr;
		#endif

		bool basic_socket::socketsInitialized = false;

		// Functions (Public):
		bool basic_socket::initSockets()
		{
			// Check if we've initialized sockets already, if we have, do nothing:
			if (socketsInitialized)
				return false;
	
			#if defined(QSOCK_WINDOWS)
				WSA_Data = new WSADATA();

				QSOCK_INT32 iResult = WSAStartup(MAKEWORD(2,2), WSA_Data);

				if (iResult != 0)
				{
					// Cleanup everything related to WSA:

					// Delete the 'WSA_Data' variable.
					delete WSA_Data;

					WSACleanup();

					return false;
				}
			#endif

			socketsInitialized = true;

			// Return the default response.
			return true;
		}

		bool basic_socket::deinitSockets()
		{
			if (!socketsInitialized)
				return false;

			// Free all of the objects we need to:
			#if defined(QSOCK_WINDOWS)
				delete WSA_Data; WSA_Data = nullptr;

				// Cleanup WinSock functionality.
				WSACleanup();
			#endif

			// Asign the socketsInitialized variable back to false.
			socketsInitialized = false;

			// Return something
			return true;
		}

		QSOCK_UINT32_LONG basic_socket::StringToIntIP(nativeString IP)
		{
			// Definition(s):
			QSOCK_UINT32_LONG intIP = 0;
	
			// Calculate the integer for the IP address:
			#ifdef QSOCK_IPV4_RESOLVE_HOSTNAMES
				// This implementation is rather...
				// Well, let's just say it's a terrible hack:
				addrinfo hints;
				addrinfo* info = nullptr;

				ZeroVar(hints);

				hints.ai_family = AF_INET;
				hints.ai_socktype = SOCK_DGRAM;
				hints.ai_protocol = IPPROTO_UDP;

				if (getaddrinfo(IP.c_str(), nullptr, &hints, &info) != 0)
					return 0;

				auto* v4 = ((sockaddr_in*)(info->ai_addr));

				intIP =

				#if defined(QSOCK_WINDOWS)
					v4->sin_addr.S_un.S_addr;
				#else
					v4->sin_addr.s_addr;
				#endif

				freeaddrinfo(info);
			#else
				intIP = inet_addr(IP.c_str());
			#endif

			// Return the calculated IP address.
			return ntohl(intIP);
		}

		nativeString basic_socket::IntToStringIP(QSOCK_UINT32_LONG IP)
		{
			// Definition(s):
			struct in_addr address;

			// Add the IP to 'address':
			#if defined(QSOCK_WINDOWS)
				address.S_un.S_addr =
			#else
				address.s_addr =
			#endif

			htonl(IP);

			// Return a nativeString of the IP address:
			return inet_ntoa(address);
		}

		// Constructor(s):
		basic_socket::basic_socket() : canCloseSocket(true)
		{
			// Nothing so far.
		}

		basic_socket::basic_socket(SOCKET internalSocket, bool full_control)
			: _socket(internalSocket), canCloseSocket(full_control)
		{
			// Nothing so far.
		}

		// Destructor(s):
		basic_socket::~basic_socket()
		{
			closeSocket();
		}

		// Functions (Protected):
		void basic_socket::begin_readRemoteMessages(basic_socket* instance, uqchar* buffer, const size_type bufferSize)
		{
			instance->readRemoteMessages(buffer, bufferSize);

			return;
		}

		// Methods (Public):
		nativeString basic_socket::strMsgIP() const
		{
			#if !defined(QSOCK_IPVABSTRACT)
				return basic_socket::IntToStringIP(msgIP());
			#else
				// Namespace(s):
				using namespace std;
		
				// Local variable(s):
				//size_type outputLength(0);
				//void* dataPosition = nullptr;

				// Allocate the needed c-string(s).
				QSOCK_CHAR output_cstr[NI_MAXHOST];

				// Allocate the output 'nativeString'.
				nativeString output;

				// Convert the address to a string:
				//inet_ntop(si_Destination.sa_family, dataPosition, output_cstr, outputLength); // (void*)&si_Destination.sa_data

				if (getnameinfo(&so_Destination, sizeof(socketAddress), output_cstr, sizeof(output_cstr), nullptr, 0, 0) != 0) // sizeof(sockaddr_in6)
				{
					// Nothing so far.
				}

				// Convert the c-string to a 'nativeString':
				output = (nativeString)output_cstr;

				// Return the newly generated 'nativeString'.
				return output;
			#endif
		}

		QSOCK_UINT32_LONG basic_socket::intMsgIP() const
		{
			#if !defined(QSOCK_IPVABSTRACT)
				return ntohl
				(
					#if defined(QSOCK_WINDOWS)
						si_Destination.sin_addr.S_un.S_addr
					#else
						si_Destination.sin_addr.s_addr
					#endif
				);
			#else
				return (si_Destination.sa_family == AF_INET) ? StringToIntIP(strMsgIP()) : 0;
			#endif
		}

		nativePort basic_socket::msgPort() const
		{
			// Check the destination's family, and based on that, return the internal port.
			#if !defined(QSOCK_IPVABSTRACT)
				/*
				switch (si_Destination.sin_family)
				{
					default: // case AF_INET:
						return (nativePort)ntohs(((sockaddr_in*)&si_Destination)->sin_port);

						break;
				}
				*/
		
				return (nativePort)ntohs(((sockaddr_in*)&si_Destination)->sin_port);
			#else
				// Allocate the needed c-string(s).
				QSOCK_CHAR serverInfo[NI_MAXSERV];

				// Convert the service name to a 'nativePort'
				if (getnameinfo((const socketAddress*)&si_Destination, sizeof(socketAddress), nullptr, 0, serverInfo, NI_MAXSERV, NI_DGRAM|NI_NUMERICSERV) != 0)
				{
					return (nativePort)0;
				}

				return (nativePort)atol(serverInfo);
			#endif

			// If we couldn't calculate the port, return zero.
			return (nativePort)0;
		}

		// Initialization related:
		bool basic_socket::connect(nativeString address, const nativePort externalPort, const nativePort internalPort)
		{
			if (!socketClosed)
				return false;
			
			// Definitions:
	
			// Set the port variable.
			this->port = externalPort;
	
			// Set the 'isServer' variable.
			this->_isServer = false;

			// Bind and initialize the internal socket.
			if (!bindSocket(internalPort)) return false;
	
			// Setup our destination using the external port, and the address:
			if (!setupDestination(address, this->port)) return false;

			// Finish doing any needed operations:

			// Set the socket as 'open'.
			this->socketClosed = false;

			// Return the default response.
			return true;
		}

		bool basic_socket::host(nativePort externalPort)
		{
			// Namespace(s):
			using namespace std;

			// Definitions:
	
			// Set the port variable.
			this->port = externalPort;

			// Set the isServer variable:
			this->_isServer = true;

			// Bind and initialize the internal socket.
			if (!bindSocket(externalPort)) return false;

			// Finish doing any needed operations:
			this->socketClosed = false;

			// Return the default response.
			return true;
		}

		// Deinitialization related:
		bool basic_socket::close()
		{
			return closeSocket();
		}

		bool basic_socket::setupDestinationV4(QSOCK_UINT32_LONG address, nativePort externalPort)
		{
			#if !defined(QSOCK_IPVABSTRACT)
				if (externalPort == (nativePort)0)
				{
					externalPort = msgPort();

					// If we still don't have a port to work with, tell the user:
					if (externalPort == (nativePort)0)
						return false;
				}

				// If the address is set to zero, and we can't broadcast, try the IP of the last message received:
				if (address == 0 && !broadcastSupported)
				{
					// Get the IP from the last packet received.
					address = msgIP();

					// If we still don't have an IP to work with, tell the user:
					if (address == 0)
						return false;
				}
		
				// Assign the output-family to IPV4.
				so_Destination.sin_family = AF_INET;

				// Assign the output-port to 'externalPort'.
				so_Destination.sin_port = htons((uqshort)externalPort);

				// Assign the output-address to 'address':
				#if defined(QSOCK_WINDOWS)
					so_Destination.sin_addr.S_un.S_addr =
				#else
					so_Destination.sin_addr.s_addr =
				#endif

				htonl(address);

				// No errors were found, return as usual.
				return true;
			#else
				// Call the main 'QSOCK_IPVABSTRACT' compliant version of 'setupDestination'.
				return setupDestination(IntToStringIP(address), externalPort);
			#endif

			// This point should never be reached.
			return false;
		}

		bool basic_socket::setupDestination(std::string address, nativePort externalPort)
		{
			// Namespace(s):
			using namespace std;

			// Local variable(s):
			bool response(false);

			#if !defined(QSOCK_IPVABSTRACT)
				// Call the IPV4 version of 'setupDestination'.
				response = setupDestination(StringToIntIP(address), externalPort);
			#else
				// If I end up adding to this function later, I'll want what's in here to be cleaned up first:
				{
					if (address.length() == 0)
					{
						address = (std::string)msgIP();

						if (address.length() == 0)
						{
							// We were unable to find a suitable address.
							return false;
						}
					}

					if (externalPort == (nativePort)0)
					{
						externalPort = msgPort();

						// Check if we've been given a valid port, and if not, return false:
						if (externalPort == (nativePort)0)
							return false;
					}

					// Local variable(s):
					addrinfo* so_Destination_result = nullptr;
					stringstream portSStream; portSStream << externalPort;

					// inet_pton(hints.sa_family, address, &so_Destination)

					if (getaddrinfo(address.c_str(), portSStream.str().c_str(), &hints, &so_Destination_result) != 0)
					{
						// Clean up the address-list.
						freeaddrinfo(so_Destination_result);

						// Since we can't continue, tell the user.
						return false;
					}

					// Copy the result from 'getaddrinfo' to 'so_Destination', then delete the old copy of the result:
					memcpy(&so_Destination, so_Destination_result->ai_addr, sizeof(so_Destination));

					/*
					if (!isBigEndian())
					{
						if (so_Destination.sa_family == AF_INET)
						{
							// Swap the byte-order of the IPV4 segment:
							QSOCK_UINT32_LONG* V4Address = (QSOCK_UINT32_LONG*)(so_Destination.sa_data+sizeof(nativePort));

							*V4Address = htonl(*V4Address);
						}
					}
					*/

					// Clean up the address-list.
					freeaddrinfo(so_Destination_result);

					// Set the response to 'true'.
					response = true;
				}
			#endif

			// Return the default response.
			return response;
		}

		// Input related:
		void basic_socket::readAvail(uqchar* buffer, const size_type bufferSize, rawReceiveCallback onMessageReceived)
		{
			// No need to continue, we're already covered:
			if (incomingThreadSwitch)
				return;

			// Assign the message call-back.
			this->onMessageReceived = onMessageReceived;

			// Set the "thread-switch".
			incomingThreadSwitch = true;

			// Begin executing the message-reading thread.
			incomingThread = std::thread(begin_readRemoteMessages, this, buffer, bufferSize);
			
			return;
		}

		QSOCK_INT32 basic_socket::readAvail_Blocking(uqchar* buffer, const size_type bufferSize)
		{
			// Definitions:
			QSOCK_INT32 response = 0;

			// Unfortunately, this has to be an 'int', instead of a 'size_type'.
			#ifdef QSOCK_WINDOWS
				int socketAddress_Length = (int)sizeof(socketAddress);
			#else
				socklen_t socketAddress_Length = (socklen_t)sizeof(socketAddress);
			#endif
			
			response = (recvfrom(_socket, (QSOCK_CHAR*)buffer, (int)bufferSize, 0, (sockaddr*)&si_Destination, &socketAddress_Length));

			// Check for errors:
			if (response == SOCKET_ERROR && QSOCK_ERROR != WSAENETRESET && QSOCK_ERROR != WSAECONNRESET)
			{
				return SOCKET_ERROR;
			}
	
			// Return the calculated response.
			return response;
		}

		// Output related:
		QSOCK_INT32 basic_socket::broadcastMsg(uqchar* outputBuffer, size_type outputBufferLength, nativePort port)
		{
			// Namespace(s):
			using namespace std;

			if (!isServer() || !broadcastSupported)
			{
				return sendMsg(outputBuffer, outputBufferLength, msgIP(), port);
			}
			else if (isServer() && broadcastSupported)
			{
				// Set the IPV4-destination.
				if (!setupDestination(INADDR_BROADCAST, port)) return SOCKET_ERROR;

				// Output the message.
				return outputMessage(outputBuffer, outputBufferLength);
			}

			return 0;
		}

		QSOCK_INT32 basic_socket::sendMsg(uqchar* outputBuffer, size_type outputBufferLength, QSOCK_UINT32_LONG IP, nativePort port)
		{
			// Namespace(s):
			using namespace std;

			// Setup the output-address structure with the needed information.
			if (!setupDestination(IP, port))
				return SOCKET_ERROR;

			// Output the message.
			return outputMessage(outputBuffer, outputBufferLength);
		}

		QSOCK_INT32 basic_socket::sendMsg(uqchar* outputBuffer, size_type outputBufferLength, nativeString strIP, nativePort port)
		{
			#if !defined(QSOCK_IPVABSTRACT)
				return sendMsg(outputBuffer, outputBufferLength, StringToIntIP(strIP), port);
			#else
				if (!setupDestination(strIP, port)) return SOCKET_ERROR;

				// Output the message.
				return outputMessage(outputBuffer, outputBufferLength);
			#endif

			// Return the default response.
			return 0;
		}

		QSOCK_INT32 basic_socket::sendMsg(uqchar* outputBuffer, size_type outputBufferLength)
		{
			if
			(
				#if !defined(QSOCK_IPVABSTRACT)
					so_Destination.sin_family == 0
				#else
					so_Destination.sa_family == 0
				#endif
			)
			{
				if (!setupDestination())
				{
					return SOCKET_ERROR;
				}
			}

			return outputMessage(outputBuffer, outputBufferLength);
		}

		QSOCK_INT32 basic_socket::outputMessage(uqchar* outputBuffer, size_type outputBufferLength)
		{
			// Namespace(s):
			using namespace std;

			// Ensure we have an address to work with.
			if
			(
				#if !defined(QSOCK_IPVABSTRACT)
					so_Destination.sin_family == 0
				#else
					so_Destination.sa_family == 0
				#endif
			)
			{
				return SOCKET_ERROR;
			}

			// Send the current 'outbuffer' to the IP address specified, on the port specified:
			QSOCK_INT32 transferred = sendto
			(
				_socket, (const QSOCK_CHAR*)outputBuffer, (int)outputBufferLength,
				0, (const sockaddr*)&so_Destination, sizeof(sockaddr) // No flags added for now.
			);

			// Tell the user how many bytes were transferred.
			return transferred;
		}

		// Methods (Protected):
		qint basic_socket::bindInternalSocket(qint filter, const timeval* recvTimeout)
		{
			// Local variable(s):
			qint bindResult = 0;

			// Used for socket options:
			QSOCK_INT32 bAllow = 1;
			QSOCK_INT32 bDisable = 0;

			#if !defined(QSOCK_IPVABSTRACT)
				_socket = ::socket(hints.ai_family, hints.ai_socktype, hints.ai_protocol);

				if (recvTimeout != nullptr)
				{
					setsockopt(_socket, SOL_SOCKET, SO_RCVTIMEO, (const char*)recvTimeout, sizeof(timeval));
				}

				bindResult = ::bind(_socket, (const sockaddr*)boundAddress, sizeof(socketAddress));
			#else
				// Bind the socket using the one of the addresses in 'result':
				for (boundAddress = result; boundAddress != nullptr; boundAddress = boundAddress->ai_next)
				{
					if (filter != -1 && boundAddress->ai_family != filter)
						continue;

					// Try to create a socket.
					_socket = ::socket(boundAddress->ai_family, boundAddress->ai_socktype, boundAddress->ai_protocol);

					// Check for errors while attempting socket creation:
					if (_socket == INVALID_SOCKET)
					{
						// Unable to create a socket, skip this attempt.
						continue;
					}

					// Assgin various socket options:
					#if defined(QSOCK_IPVABSTRACT)
						if
						(
							#if defined(QSOCK_IPVABSTRACT)
								boundAddress->ai_family == AF_INET6
							#else
								boundAddress->sin_family == AF_INET6
							#endif
						)
						{
							// Add support for IPV4:
							if (setsockopt(_socket, IPPROTO_IPV6, IPV6_V6ONLY, (const QSOCK_CHAR*)&bDisable, sizeof(bDisable)) != 0)
							{
								//return false;
							}
						}
					#endif

					if (isServer())
					{
						if
						(
							#if defined(QSOCK_IPVABSTRACT)
								boundAddress->ai_family != AF_INET
							#else
								boundAddress->sin_family != AF_INET
							#endif
							|| setsockopt(_socket, SOL_SOCKET, SO_BROADCAST, (const QSOCK_CHAR*)&bAllow, sizeof(bAllow)) < 0
						)
						{
							broadcastSupported = false;
        
							//return false;
						}
						else
						{
							broadcastSupported = true;
						}
					}

					// Attempt to bind the socket.
					bindResult = bind(_socket, boundAddress->ai_addr, (int)boundAddress->ai_addrlen);

					// Check if the bind attempt was successful.
					if (bindResult == 0)
					{
						// The socket was successfully bound, exit the loop.
						break;
					}
					else
					{
						// Close the socket.
						shutdownInternalSocket();
					}
				}
			#endif

			// Return the error code produced from 'bind'.
			return bindResult;
		}

		bool basic_socket::bindSocket(const nativePort internalPort, qint nativeFilter, const timeval* nativeRecvTimeout)
		{
			// Setup the socket's information:
			#if !defined(QSOCK_IPVABSTRACT)
				hints.ai_family = AF_INET;
			#else
				// This will be changed to 'AF_UNSPEC' later on, assuming an IPV4 address couldn't be found.
				//hints.ai_family = AF_INET;
				hints.ai_family = AF_UNSPEC;
			#endif

			// Set the information for 'hints':
			hints.ai_socktype = SOCK_DGRAM;
			hints.ai_protocol = IPPROTO_UDP;

			hints.ai_flags = AI_PASSIVE;

			// Setup the 'result' socket 'address_in' with 'hints', the internal port, and 'INADDR_ANY' as the address:
			#if !defined(QSOCK_IPVABSTRACT)
				result = new socketAddress(); // Also known as 'sockaddr_in'

				#if defined(QSOCK_WINDOWS)
					result->sin_addr.S_un.S_addr =
				#else
					result->sin_addr.s_addr =
				#endif

				htonl(INADDR_ANY);

				result->sin_family = hints.ai_family;
				result->sin_port = htons(internalPort);
			#else
				{
					// Map the port agnostically to the 'result' address.

					// Convert the port to a string:
					stringstream portSStream; portSStream << internalPort;

					/*
					QSOCK_CHAR* portStr = new QSOCK_CHAR[portSStream.str().length()+1];
					memcpy(portStr, portSStream.str().c_str(), portSStream.str().length());
					portStr[portSStream.str().length()] = '\0';
					*/
			
					/*
					// Check for IPV4 addresses first:
					if (getaddrinfo(nullptr, (const QSOCK_CHAR*)portStr, &hints, &result) != 0) // portSStream.str().c_str()
					{
						// We weren't able to find an IPV4 address, clean things up, and try with any IP-version:
						if (result != nullptr) freeaddrinfo(result); result = nullptr; // delete [] result;
				
						// Assign the ip-family to unspecific.
						hints.ai_family = AF_INET;
					*/

					if (getaddrinfo(nullptr, portSStream.str().c_str(), &hints, &result) != 0) // portStr
					{
						// Delete the address-info result.
						if (result != nullptr) freeaddrinfo(result); //delete [] result;

						// Since we can't continue, tell the user.
						return false;
					}

					//delete [] portStr;
					//}

					//hints.ai_flags = 0;
				}
			#endif

			// Create the socket using 'hints':
			{
				// Local variable(s):

				// The result-variable used for the bind-result.
				qint bindResult = 0;

				#if !defined(QSOCK_IPVABSTRACT)
					// Assign the bound-address variable to 'result'.
					boundAddress = result;

					// Bind the socket, using the address 'boundAddress' is pointing to.
					bindResult = bindInternalSocket();
				#else
					// Try IPV6 before attempting to use any protocol.
					bindResult = bindInternalSocket(AF_INET6);

					// We weren't able to use IPV6, try any protocol:
					if (bindResult != 0)
					{
						bindResult = bindInternalSocket();
					}
				#endif

				// Check for any errors while creating the socket:
				if (_socket == INVALID_SOCKET)
				{
					// Close the internal socket.
					close(); // closeSocket();

					return false;
				}

				// Check if our bind-attempt was successful:
				if (bindResult == SOCKET_ERROR)
				{
					// Close the internal socket.
					close(); // closeSocket();

					// Return a negative response.
					return false;
				}

				///* Do not set the family of so_Destination first, this will screw up checks made later on:
				// Assign the IO addresses' families:
				#if !defined(QSOCK_IPVABSTRACT)
					si_Destination.sin_family = boundAddress->sin_family;
					//so_Destination.sin_family = si_Destination.sin_family;
				#else
					si_Destination.sa_family = boundAddress->ai_addr->sa_family; //boundAddress->ai_family;
					//so_Destination.sa_family = si_Destination.sa_family;
				#endif
				//*/
			}

			// Return the default response.
			return true;
		}

		bool basic_socket::bindSocket(const nativePort port)
		{
			return bindSocket(port, -1, nullptr);
		}

		bool basic_socket::closeSocket()
		{
			// Check if this socket has been closed already:
			if (socketClosed)
			{
				return false;
			}

			// Delete any extra data, clean things up, etc:
			#if defined(QSOCK_IPVABSTRACT)
				freeaddrinfo((addrinfo*)result); // delete [] result;
			#else
				delete result;
			#endif
	
			result = nullptr;
			boundAddress = nullptr;

			ZeroVar(si_Destination);
			ZeroVar(so_Destination);

			if (canCloseSocket)
			{
				shutdownInternalSocket();

				// Free the internal socket:
				#if defined(QSOCK_WINDOWS)
					if (_socket != 0 && ::closesocket(_socket) == SOCKET_ERROR)
				#else
					if (_socket != 0 && ::close(_socket) == SOCKET_ERROR)
				#endif
					{
						return false;
					}
			}

			_socket = SOCKET_ERROR; // 0;

			// Set the socket as closed.
			socketClosed = true;

			// Return the default response.
			return true;
		}

		// Packet-management related:
		void basic_socket::onIncomingMessage(uqchar* buffer, const size_type length, const size_type bufferSize)
		{
			if (onMessageReceived)
			{
				onMessageReceived(buffer, length, bufferSize);
			}

			return;
		}

		void basic_socket::readRemoteMessages(uqchar* buffer, const size_type bufferSize)
		{
			// Keep going, only if we weren't stopped:
			while (incomingThreadSwitch)
			{
				// Check for an incoming packet:
				auto response = readAvail_Blocking(buffer, bufferSize);

				if (incomingThreadSwitch)
				{
					// Check the response-code:
					if (response != SOCKET_ERROR) // > 0
					{
						onIncomingMessage(buffer, (size_type)response, bufferSize);
					}
				}
			}

			return;
		}

		// QSocket:

		// Functions:
		// Nothing so far.

		// Constructor(s):
		QSocket::QSocket(size_type bufferlen, bool fixByteOrder)
			: basic_socket(), QStream(bufferlen, bufferlen, fixByteOrder), messageState(MESSAGE_STATE_WAITING)
		{
			#if defined(QSOCK_AUTOINIT_SOCKETS)
				if (!initSockets())
					return false;
			#endif

			// Initialize everything else we need to:
			ZeroVar(hints);
			ZeroVar(si_Destination);
			ZeroVar(so_Destination);
		}

		QSocket::~QSocket()
		{
			// Call our local close-routine.
			QSocket::closeSocket();
		}

		// Methods (Public):

		// Deinitialization related:
		bool QSocket::close()
		{
			// Call the super-class's implementation.
			if (!basic_socket::close())
				return false;

			// Call our own implementation.
			return QSocket::closeSocket();
		}

		// Input related:
		bool QSocket::msgAvail()
		{
			if (messageState == MESSAGE_STATE_READING)
			{
				return canRead();
			}

			// Return the default response.
			return false;
		}

		QSOCK_INT32 QSocket::readAvail_Blocking(uqchar* buffer, const size_type bufferSize)
		{
			// Call the super-class's implementation, then hold its response.
			auto responseCode = basic_socket::readAvail_Blocking(buffer, bufferSize);

			// Check if a message was read into 'inbuffer':
			if (responseCode > 0 && buffer == inbuffer)
			{
				// Set the internal length.
				inbufferlen = (size_type)responseCode;

				// Reset the length and offset for the input-buffer:
				readOffset = 0;
			}

			return responseCode;
		}

		void QSocket::readAvail(uqchar* buffer, const size_type bufferSize, rawReceiveCallback onMessageReceived, bool callbackOnRemoteThread)
		{
			this->callbackOnRemoteThread = callbackOnRemoteThread;

			basic_socket::readAvail(buffer, bufferSize, onMessageReceived);

			return;
		}

		void QSocket::readAvail(uqchar* buffer, const size_type bufferSize, rawReceiveCallback onMessageReceived)
		{
			// Apply default any arguments for the main implementation.
			readAvail(buffer, bufferSize, onMessageReceived, false);

			return;
		}

		QSOCK_INT32 QSocket::readAvail()
		{
			// Ensure we're operating in the proper environment:
			if (callbackOnRemoteThread)
			{
				return SOCKET_ERROR;
			}

			// Check if the remote-thread is running:
			if (!incomingThreadSwitch)
			{
				// Start the remote-thread.
				readAvail(rawReceiveCallback(), false);

				// We can't continue yet, tell the user.
				return 0;
			}

			// Check if there's a message for us:
			if (messageState == MESSAGE_STATE_AVAILABLE)
			{
				messageState = MESSAGE_STATE_READING;

				// Check for a message call-back:
				if (onMessageReceived)
				{
					onMessageReceived(inbuffer, inbufferlen, inbufferSize);
				}

				// Return the number of bytes read:
				return (QSOCK_INT32)inbufferlen;
			}
			else if (messageState == MESSAGE_STATE_READING)
			{
				// Post-message cleanup:
				messageState = MESSAGE_STATE_DONE;

				// Tell the other thread we're done with the message.
				incomingThreadWait.notify_all(); // notify_one();
			}

			// A message could not be found.
			return 0;
		}

		// Methods (Protected):

		// Initialization related:
		bool QSocket::bindSocket(const nativePort port)
		{
			struct timeval defaultTimeValue;

			ZeroVar(defaultTimeValue);

			defaultTimeValue.tv_sec = DEFAULT_RECV_TIMEOUT_SEC;
			defaultTimeValue.tv_usec = DEFAULT_RECV_TIMEOUT_USEC;

			return basic_socket::bindSocket(port, -1, &defaultTimeValue);
		}

		// Deinitialization related:
		bool QSocket::closeSocket()
		{
			if (incomingThreadSwitch)
			{
				// Set the incoming-thread switch to 'false'.
				incomingThreadSwitch = false;

				messageState = MESSAGE_STATE_DONE;

				// Just in case another thread was waiting for us, notify them.
				incomingThreadWait.notify_all();

				// Wait until the packet-thread has finished safely.
				incomingThread.join();
				//incomingThread.detach();

				//packetResponseMutex.unlock();

				//packetMutex.unlock();
			}

			// Return the default response.
			return true;
		}

		// Input related:
		void QSocket::readRemoteMessages(uqchar* buffer, const size_type bufferSize)
		{
			// Execute the main routine.
			basic_socket::readRemoteMessages(buffer, bufferSize);

			return;
		}

		void QSocket::onIncomingMessage(uqchar* buffer, const size_type length, const size_type bufferSize)
		{
			// Check if this thread can execute call-backs:
			if (callbackOnRemoteThread)
			{
				// Call the super-class's implementation.
				basic_socket::onIncomingMessage(buffer, length, bufferSize);
			}
			else
			{
				// Local variable(s):

				// Lock the control-mutex.
				std::unique_lock<std::mutex> readerLock(packetMutex);

				messageState = MESSAGE_STATE_AVAILABLE;

				// Wait for a response from the main thread.
				incomingThreadWait.wait(readerLock, [this] { return (messageState == MESSAGE_STATE_DONE); });

				// Make sure we can continue:
				if (!incomingThreadSwitch)
				{
					return;
				}

				messageState = MESSAGE_STATE_WAITING;
			}

			return;
		}
	}
}

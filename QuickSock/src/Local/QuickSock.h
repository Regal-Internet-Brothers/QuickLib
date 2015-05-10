#pragma once
	
// Preprocessor related:

// QuickSock related:
#if defined(_WIN32) || defined(_WIN64)
	#define QSOCK_WINDOWS
#endif
	
#if !defined(QSOCK_WINDOWS_LEGACY)
	#if defined(QSOCK_WINDOWS) && defined(_MSC_VER) && (_MSC_VER <= 1600)
		#define QSOCK_WINDOWS_LEGACY
	#endif
#endif

// Check if exceptions are enabled:
#if defined(QSOCK_THROW_EXCEPTIONS)
	#define qthrow(X) throw X
#else
	#define qthrow(X)
#endif

#if !defined(QSOCK_IPVABSTRACT)
	// This allows the IPV4-backend to resolve hostnames.
	// Using this may cause extra overhead, but this isn't
	// used very often by the IPV4 implementation to begin with.
	#define QSOCK_IPV4_RESOLVE_HOSTNAMES
	
	// By defining this, 'QuickSock' will use a 'modern' abstract approach to IP-version handling (May cause problems).
	//#define QSOCK_IPVABSTRACT
#else
	#if defined(QSOCK_IPV6)
		#undef QSOCK_IPV6
	#endif
#endif

#ifndef QSOCK_ERROR
	// These can be found in the includes:
	#if defined(QSOCK_WINDOWS)
		#define QSOCK_ERROR WSAGetLastError()
	#else
		#define QSOCK_ERROR errno
	#endif
#endif

#ifdef WORDS_BIGENDIAN
	#define QSOCK_BIGENDIAN
#endif

// Includes (Internal):
#include "QuickTypes.h"
#include "Exceptions.h"

// Includes (External):

// Standard Library:

// C Standard library:
#include <cstdlib>
#include <cstring>
//#include <cstddef>

// C++ Standard Library:
#include <string>
#include <sstream>
#include <algorithm>

#include <thread>
#include <atomic>
#include <condition_variable>
#include <mutex>
#include <chrono>

// Windows specific includes:
#if defined(QSOCK_WINDOWS)
	// WinSock related:
	#ifndef WIN32_LEAN_AND_MEAN
		#define WIN32_LEAN_AND_MEAN
		#define QSOCK_WIN32_L_A_M
	#endif

	//#if (!defined(QSOCK_IPV6) && !defined(QSOCK_IPVABSTRACT))
	#define _WINSOCK_DEPRECATED_NO_WARNINGS
	//#endif

	#define NOMINMAX
	
	// Make sure to link with the required library files:
	#pragma comment (lib, "Ws2_32.lib")
	//#pragma comment (lib, "Mswsock.lib")
	//#pragma comment (lib, "AdvApi32.lib")
	
	// Windows Includes (Much nicer than the Linux includes section):
	#include <windows.h>
	
	#include <winsock2.h>
	#include <ws2tcpip.h>
	#include <WSPiApi.h>
	#include <iphlpapi.h>

	#if defined(QSOCK_IPVABSTRACT)
		#include <mstcpip.h>
	#endif
	
	#ifdef QSOCK_WIN32_L_A_M
		#undef QSOCK_WIN32_L_A_M

		#undef WIN32_LEAN_AND_MEAN
	#endif
#endif
//#endif

// Linux, BSD, Mac OS X, etc:
#if !defined(QSOCK_WINDOWS)
	// Includes:

	// Oh god, what am I looking at:
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <sys/time.h>
	#include <netinet/in.h>
	#include <netdb.h>
	#include <unistd.h>
	#include <stdio.h>
	#include <arpa/inet.h>
	#include <stdint.h>
	#include <stdlib.h>
	#include <errno.h>

	// Don't mind me, just supporting things I don't need to:
		
	// Special thanks to Nanno Langstraat for this:
	#if defined(__linux__)
		// Includes:
		#include <endian.h>
	#elif defined(__FreeBSD__) || defined(__NetBSD__)
		// Includes:
		#include <sys/endian.h>
	#elif defined(__OpenBSD__)
		// Includes:
		//#include <sys/types.h>

		#define be16toh(x) betoh16(x)
		#define be32toh(x) betoh32(x)
		#define be64toh(x) betoh64(x)
	#endif

	// For the sake of laziness, I want to keep 'htonll' and 'ntohll':
	#if !defined(__APPLE__) && !defined(__MACH__)
		#define htonll(x) htobe64(x)
		#define ntohll(x) be64toh(x)
	#endif

	// Special thanks to Adam Banko for this part of the header.
	// In the end, I still needed to use this:
	
	// If we're compiling for a processor that's big-endian, disregard these commands:
	#ifdef WORDS_BIGENDIAN
		#define htons(x) (x)
		#define ntohs(x) (x)
		#define htonl(x) (x)
		#define ntohl(x) (x)
		#define htonf(x) (x)
		#define ntohf(x) (x)
		#define htond(x) (x)
		#define ntohd(x) (x)
	#else
		#ifdef PHP_WIN32
			#ifndef WINNT
				#define WINNT 1
			#endif
		#endif

		#ifndef htonf
			union float_long
			{
				QSOCK_FLOAT32 f;
				QSOCK_UINT32_LONG l;
			};

			inline QSOCK_UINT32_LONG htonf(QSOCK_FLOAT32 x)
			{
				union float_long fl;

				fl.f = x;

				return htonl(fl.l);
			}

			inline QSOCK_FLOAT32 ntohf(QSOCK_UINT32_LONG x)
			{
				union float_long fl;

				fl.l = ntohl(x);

				return fl.f;
			}
		#endif

		#ifndef htond
			#ifdef LINUX
				// Includes:
				#include <asm/byteorder.h>
					
				#define htond(x) __arch__swab64(x)
				#define ntohd(x) __arch__swab64(x)
			#else
				static inline QSOCK_FLOAT64 safe_swab64(QSOCK_FLOAT64 in)
				{
					QSOCK_FLOAT64 out;

					qchar* inP  = (qchar*)&in;
					qchar* outP = ((qchar*)&out) + sizeof(QSOCK_FLOAT64);

					for (uqint i=0; i< sizeof(QSOCK_FLOAT64); i++)
					{
						*(inP++) = *(--outP);
					}

					return out;
				}

				#define htond(x) safe_swab64(x)
				#define ntohd(x) safe_swab64(x)
			#endif
		#endif
	#endif
#endif

// Compatibility definitions:

// Just for the sake of laziness, we're keeping the WinSock naming-scheme for errors:
#ifndef WSAENETRESET
	#define WSAENETRESET 10052
#endif

#ifndef WSAECONNRESET
	#define WSAECONNRESET 10054
#endif

#ifndef INET_ADDRSTRLEN
	#define INET_ADDRSTRLEN 22
#endif

#ifndef INET6_ADDRSTRLEN
	#define INET6_ADDRSTRLEN 65
#endif

#ifndef SOCKET_ERROR
	#define SOCKET_ERROR -1
#endif

#ifndef INVALID_SOCKET
	#define INVALID_SOCKET -1
#endif

// WinSock's manifest constants for the 'shutdown' command:
#ifndef _WINSOCK2API_
	#define SD_RECEIVE      0x00
	#define SD_SEND         0x01
	#define SD_BOTH         0x02
#endif

// Namespace(s):
namespace quickLib
{
	namespace sockets
	{
		// Forward declarations:
		class QSocket;
		class QStream;

		// Typedefs:
		typedef unsigned short nativePort;

		typedef std::string nativeString;

		typedef wchar_t Char;

		#if !defined(QSOCK_IPVABSTRACT)
			typedef QSOCK_UINT32_LONG nativeIP;
			typedef nativeString nonNativeIP;
		#else
			typedef nativeString nativeIP;
			typedef QSOCK_UINT32_LONG nonNativeIP;
		#endif

		// The address used internally by 'QuickSock':
		typedef

		#if !defined(QSOCK_IPVABSTRACT)
			sockaddr_in
		#else
			sockaddr
		#endif

		socketAddress;

		// The type used for stream locations/offset.
		typedef size_type streamLocation;

		// Functions:

		// Check if the 64-bit byte-order commands are already supported, if not, implement them:
		#ifndef WORDS_BIGENDIAN
			#if defined(__APPLE__) && defined(__MACH__) || defined(QSOCK_WINDOWS_LEGACY) // || defined(QSOCK_MONKEYMODE) && defined(CFG_GLFW_USE_MINGW)
				inline QSOCK_UINT64 htonll(QSOCK_UINT64 inInt)
				{
					// Check if we're running on a big-endian system,
					// even if the compiler didn't know if we were:
					if (isBigEndian())
						return inInt;

					// Local variable(s):

					// The 64-bit integer used as a return-value.
					QSOCK_UINT64 retVal = 0;

					uqchar* intToConvert = (qchar*)&inInt;
					uqchar* returnInt = (qchar*)&retVal;
	
					for (uqchar i = 0; i < 7; i++)
					{
						returnInt[i] = intToConvert[7-i];
					}

					return retVal;
				}
		
				inline QSOCK_UINT64 ntohll(QSOCK_UINT64 inInt)
				{
					// Check if we're running on a big-endian system,
					// even if the compiler didn't know if we were:
					if (isBigEndian())
						return inInt;

					// Local variable(s):

					// The 64-bit integer used as a return-value.
					QSOCK_UINT64 retVal = htonll(inInt);

					((uqchar*)&retVal)[0] = (((uqchar*)&inInt)[0] >> 1);
	
					return retVal;
				}
			#endif
		#endif
		
		// If we don't have prototypes for these byte-order related commands, declare them here:
		#ifndef WORDS_BIGENDIAN
			#if defined(QSOCK_WINDOWS_LEGACY) // || defined(QSOCK_MONKEYMODE) && defined(CFG_GLFW_USE_MINGW)
				unsigned long htonf(float inFloat);
				float ntohf(unsigned long inFloat);
				unsigned long long htond(double inFloat);
				double ntohd(unsigned long long inFloat);
			#endif
		#endif

		inline bool isBigEndian()
		{
			#if defined(QSOCK_BIGENDIAN)
				return true;
			#else
				// Local variable(s):
				QSOCK_INT32 temp(1);

				return ((*(char*)&temp) != 1);
			#endif
		}

		template <typename T>
		inline void ZeroVar(T& X)
		{
			std::memset(&X, 0, sizeof(X));

			return;
		}

		inline size_type lengthOfString(nativeString s)
		{
			return s.length();
		}

		inline size_type lengthOfString(std::wstring wstr)
		{
			return wstr.length();
		}

		// Namespace(s) (Prototypes):

		// I/O related:
		namespace IO
		{
			// Input:
			inline size_type readLengthOfString(QStream& packet);

			// This command will read the type specified, but it will not
			// automatically swap the appropriate bytes of the type.
			// To do this, please use the 'read' command.
			template <typename type> inline type rawRead(QStream& packet);
			template <> inline QSOCK_UCHAR rawRead<QSOCK_UCHAR>(QStream& packet);

			template <> inline QSOCK_CHAR rawRead<QSOCK_CHAR>(QStream& packet)
			{
				return (QSOCK_CHAR)rawRead<QSOCK_UCHAR>(packet);
			}

			template <> inline bool rawRead<bool>(QStream& packet)
			{
				return ((rawRead<QSOCK_UCHAR>(packet) != 0) ? true : false);
			}

			// This acts as an automatic reading command for the type specified.
			// When retrieving the data requested, the proper byte-order command will be used.
			// If the speicfied type isn't directly supported, it will not be automatically byte-swapped.
			template <typename type> inline type read(QStream& packet);

			template <> inline QSOCK_UINT16 read<QSOCK_UINT16>(QStream& packet);
			template <> inline QSOCK_INT16 read<QSOCK_INT16>(QStream& packet) { return (QSOCK_INT16)read<QSOCK_UINT16>(packet); }

			template <> inline QSOCK_UINT32_LONG read<QSOCK_UINT32_LONG>(QStream& packet);
			template <> inline QSOCK_INT32_LONG read<QSOCK_INT32_LONG>(QStream& packet) { return (QSOCK_INT32_LONG)read<QSOCK_UINT32_LONG>(packet); }

			template <> inline QSOCK_UINT32 read<QSOCK_UINT32>(QStream& packet) { return (QSOCK_UINT32)read<QSOCK_UINT32_LONG>(packet); }
			template <> inline QSOCK_INT32 read<QSOCK_INT32>(QStream& packet) { return (QSOCK_INT32)read<QSOCK_INT32_LONG>(packet); }

			template <> inline QSOCK_UINT64 read<QSOCK_UINT64>(QStream& packet);
			template <> inline QSOCK_INT64 read<QSOCK_INT64>(QStream& packet) { return (QSOCK_INT64)read<QSOCK_UINT64>(packet); }

			template <> inline QSOCK_FLOAT32 read<QSOCK_FLOAT32>(QStream& packet);
			template <> inline QSOCK_FLOAT64 read<QSOCK_FLOAT64>(QStream& packet);

			template <> inline nativeString read<nativeString>(QStream& packet);
			template <> inline std::wstring read<std::wstring>(QStream& packet);

			// Output:
			inline bool writeLengthOfString(QStream& packet, size_type length);

			// This command will write the type specified, but it will not
			// automatically swap the appropriate bytes of the type.
			// To do this, please use the 'write' command.
			template <typename type> inline bool rawWrite(QStream& packet, type data);

			template <> inline bool rawWrite<QSOCK_UCHAR>(QStream& packet, QSOCK_UCHAR data);

			template <> inline bool rawWrite<QSOCK_CHAR>(QStream& packet, QSOCK_CHAR data)
			{
				return rawWrite<QSOCK_UCHAR>(packet, (QSOCK_UCHAR)data);
			}

			template <> inline bool rawWrite<bool>(QStream& packet, bool data)
			{
				return rawWrite<QSOCK_UCHAR>(packet, (data) ? 1 : 0);
			}

			template <typename type> inline bool write(QStream& packet, type data)
			{
				return rawWrite<type>(packet, data);
			}

			template <> inline bool write<QSOCK_UINT16>(QStream& packet, QSOCK_UINT16 data);
			template <> inline bool write<QSOCK_INT16>(QStream& packet, QSOCK_INT16 data);

			template <> inline bool write<QSOCK_UINT32_LONG>(QStream& packet, QSOCK_UINT32_LONG data);
			template <> inline bool write<QSOCK_INT32_LONG>(QStream& packet, QSOCK_INT32_LONG data) { return write<QSOCK_UINT32_LONG>(packet, (QSOCK_UINT32_LONG)data); }

			template <> inline bool write<QSOCK_UINT32>(QStream& packet, QSOCK_UINT32 data) { return write<QSOCK_UINT32_LONG>(packet, (QSOCK_UINT32_LONG)data); }
			template <> inline bool write<QSOCK_INT32>(QStream& packet, QSOCK_INT32 data) { return write<QSOCK_INT32_LONG>(packet, (QSOCK_INT32_LONG)data); }

			template <> inline bool write<QSOCK_UINT64>(QStream& packet, QSOCK_UINT64 data);
			template <> inline bool write<QSOCK_INT64>(QStream& packet, QSOCK_INT64 data) { return write<QSOCK_UINT64>(packet, (QSOCK_UINT64)data); }

			template <> inline bool write<QSOCK_FLOAT32>(QStream& packet, QSOCK_FLOAT32 data);
			template <> inline bool write<QSOCK_FLOAT64>(QStream& packet, QSOCK_FLOAT64 data);

			template <> inline bool write<nativeString>(QStream& packet, nativeString str);

			template <> inline bool write<std::wstring>(QStream& packet, std::wstring wstr);
		}

		// Classes:

		// DO NOT pass 'QStream' objects by data, unless you want to perform a deep-copy.
		typedef class QStream
		{
			public:
				// Enumerator(s):
				enum addressType : uqchar
				{
					ADDRESS_TYPE_INTEGER = 0,
					ADDRESS_TYPE_STRING = 1,
				};

				// Functions:

				// This will transfer the contents of 'source' to 'destination', using the sub-script operator.
				template <typename TypeA, typename TypeB = TypeA>
				static inline size_type smartTransfer(TypeA source, TypeB destination, size_type count, size_type sourceOffset = 0, size_type destinationOffset = 0)
				{
					for (size_type i = 0; i < count; i++)
						source[i + sourceOffset] = destination[i + destinationOffset];

					return count;
				}

				// The return value of this command is the number of bytes transferred.
				static inline size_type rawTransfer(const void* source, void* destination, size_type count, size_type sourceOffsetInBytes=0, size_type destinationOffsetInBytes=0)
				{
					memcpy((uqchar*)destination + destinationOffsetInBytes, (const uqchar*)source + sourceOffsetInBytes, count);

					return count;
				}

				// Fields (Public):
				
				// A buffer dedicated to inbound data.
				uqchar* inbuffer; // = nullptr;

				// A buffer dedicated to outbound data.
				uqchar* outbuffer; // = nullptr;

				// The real size of the input-buffer. (Maximum)
				size_type inbufferSize;
				
				// The real size of the output-buffer. (Maximum)
				size_type outbufferSize;

				// The currently allocated area of the 'inbuffer'.
				size_type inbufferlen = 0;

				// The current size of the output-area.
				size_type outbufferlen = 0;

				// The current read-position in the 'inbuffer'.
				streamLocation readOffset = 0;

				// The current write-position in the 'outbuffer'.
				streamLocation writeOffset = 0;

				// If enabled, the byte order of explicitly
				// supported types will be corrected. (Network byte-order)
				bool fixByteOrder = true;

				// Constructor(s) (Public):

				// This class reserves the right to mutate the buffers specified.
				QStream(uqchar* inputBuffer, uqchar* outputBuffer, size_type inBuffer_size, size_type outBuffer_size, bool fixBOrder=true);
				
				// This overload acts as shorthand for setting up a 'QStream' without an output buffer.
				QStream(uqchar* inputBuffer, size_type inBuffer_size, bool fixBOrder=true);

				// This will automatically allocate "managed" buffers.
				QStream(size_type inBuffer_size, size_type outBuffer_size, bool fixBOrder=true);
				QStream(size_type buffer_size, bool fixBOrder=true);

				// This constructor performs a deep copy.
				explicit QStream(const QStream& copyDataFrom);

				// Destructor(s):
				~QStream();

				// Methods (Public):

				// Meta:
				inline bool canRead(size_type count) const
				{
					return ((count != 0) && (((size_type)readOffset) + count) <= inbufferlen);
				}

				inline bool canWrite(size_type count) const
				{
					return ((count != 0) && (((size_type)writeOffset) + count) <= outbufferSize);
				}

				inline bool canRead() const
				{
					return (readOffset < inbufferlen);
				}

				inline bool canWrite() const
				{
					return (writeOffset < outbufferSize);
				}

				size_type inBytesLeft() const
				{
					return (inbufferlen - (size_type)readOffset);
				}

				size_type outBytesLeft() const
				{
					return (outbufferSize - (size_type)writeOffset);
				}

				// Stream management routines:

				// This command will not "zero out" the internal input-buffer.
				// Instead, it will simply act as if the buffer doesn't contain anything.
				inline void flushInput()
				{
					inbufferlen = 0;
					readOffset = 0;

					return;
				}

				// This command will not "zero out" the internal output-buffer.
				// Instead, it will simply act as if the buffer doesn't contain anything.
				inline void flushOutput()
				{
					outbufferlen = 0;
					writeOffset = 0;

					return;
				}

				// This command will safely flush a region of the input-buffer.
				// This is useful when seeking around the input "stream".
				inline void flushInputRegion(streamLocation position, size_type amount)
				{
					if (position+amount > inbufferlen)
						amount = (inbufferlen-position);

					memset(inbuffer + position, 0, amount);

					return;
				}

				// This will "flush" the amount specified, starting at the current input-offset.
				inline void flushInputRegion(size_type amount)
				{
					memset(inbuffer + readOffset, 0, amount);

					return;
				}

				// This will flush all bytes following the current read-offset.
				inline void flushInputRegion()
				{
					flushInputRegion(inbufferlen-readOffset);

					return;
				}

				// This will set the input-length to the length specified.
				// The input-offset will be changed to the
				// length specified if it was out of bounds.
				inline void setInputLength(size_type length)
				{
					inbufferlen = std::min(length, inbufferlen);
					readOffset = std::min(readOffset, inbufferlen);

					return;
				}

				inline size_type inSeek(streamLocation position=0)
				{
					if (position > inbufferlen)
					{
						qthrow(QSOCK_SEEK_EXCEPTION(this, QSOCK_SEEK_EXCEPTION::seekMode::SEEK_MODE_IN, position));

						return 0;
					}

					// Assign the new read-offset, then return it.
					return readOffset = position;
				}

				inline streamLocation inSeekForward(streamLocation bytesForward)
				{
					return inSeek(readOffset+bytesForward);
				}

				inline streamLocation inSeekBackward(streamLocation bytesBackward)
				{
					return inSeek(readOffset-bytesBackward);
				}

				// This command resets the read-offset to the default/zero.
				inline void resetRead() { inSeek(0); return; }

				inline streamLocation outSeek(streamLocation position)
				{
					if (position > outbufferSize)
					{
						qthrow(QSOCK_SEEK_EXCEPTION(this, QSOCK_SEEK_EXCEPTION::seekMode::SEEK_MODE_OUT, position));

						return 0;
					}

					// Assign the new write-offset, then return it:
					writeOffset = position;

					outbufferlen = std::max(outbufferlen, writeOffset);

					return writeOffset;
				}

				inline streamLocation outSeekForward(streamLocation bytesForward)
				{
					return outSeek(writeOffset+bytesForward);
				}

				inline streamLocation outSeekBackward(streamLocation bytesBackward)
				{
					return outSeek(writeOffset-bytesBackward);
				}

				// This command resets the write-offset to the default/zero.
				inline void resetWrite() { outSeek(0); return; }

				// This command manually clears the input-buffer. (High overhead)
				bool clearInBuffer();

				// This command manually clears the output-buffer. (High overhead)
				bool clearOutBuffer();

				// Input routines:
				bool readData(void* output, size_type size, size_type output_offset = 0, bool checkRead = true);

				// This command will read the type specified, but it will not
				// automatically swap the appropriate bytes of the type.
				// To do this, please use the 'read' command.
				template <typename type> inline type rawRead()
				{
					return IO::rawRead<type>(*this);
				}

				template <typename type> inline type read()
				{
					return IO::read<type>(*this);
				}

				// Type-specific macros:
				inline uqchar readOct() { return read<uqchar>(); }
				inline uqchar readByte() { return readOct(); }
				inline qchar readChar() { return read<qchar>(); }
				inline bool readBool() { return read<bool>(); }

				inline qshort readShort() { return read<qshort>(); }
				inline qint readInt() { return read<qint>(); }
				inline qlong readLong() { return read<qlong>(); }

				inline qfloat readFloat() { return read<qfloat>(); }
				inline qdouble readDouble() { return read<qdouble>(); }

				inline nativePort readPort() { return read<nativePort>(); }

				inline size_type readLengthOfString()
				{
					return IO::readLengthOfString(*this);
				}

				nativeIP readIP();

				// This overload returns 'true' if operations were successful.
				// The 'checkRead' argument is generally reserved, and should not be used externally.
				// This is not enforced, but please take safety into account before using it.
				bool UreadBytes(uqchar* output, size_type count, size_type output_offset = 0, bool checkRead = true);

				// This overload of 'UreadBytes' produces a new 'uqchar' array, this array should be managed/deleted by the caller.
				// If an array was not produced, reading could not be done.
				uqchar* UreadBytes(size_type count=0, bool zero_ended=false);

				// Like 'UreadBytes', 'readBytes' produces a new 'qchar' array, and should be managed/deleted by the caller.
				qchar* readBytes(size_type count=0, bool zero_ended=false) { return (qchar*)UreadBytes(count, zero_ended); }

				inline bool readBytes(uqchar* output, size_type count, size_type output_offset = 0)
				{
					return UreadBytes(output, count, output_offset);
				}

				/*
					Simulated reading should only be done with full understanding
					that the memory provided may not be modified or deleted.
					Only use these commands for temporary representation of data.
					
					If reading can not be done, unlike a normal read,
					these will simply provide 'nullptr' as a return value.
					
					DO NOT perform any reading operations which may mutate the data pointed to.
					This mainly applies to checking for messages, and similar routines.
					Seeking should work without any problems.
				*/

				inline uqchar* simulatedUReadBytes(size_type count=0)
				{
					// Check for errors:
					if (count == 0 && !canRead(inBytesLeft()) || !canRead(count))
						return nullptr;

					uqchar* data = (uqchar*)(inbuffer+readOffset);

					// Seek forward the amount we're going to "transfer".
					inSeekForward(count);

					return data;
				}

				// This command simply wraps 'simulatedUReadBytes',
				// please read that command's documentation.
				inline qchar* simulatedReadBytes(size_type count = 0)
				{
					return (qchar*)simulatedUReadBytes(count);
				}

				// Other types:
				inline nativeString readNativeString(size_type length)
				{
					// Simulate a read operation based on the length specified.
					// Then, use the output to build a 'nativeString'.
					return nativeString((const char*)simulatedReadBytes(length), (const char*)inbuffer);
				}

				inline nativeString readString()
				{
					return read<nativeString>();
				}

				inline nativeString readString(size_type length)
				{
					return readNativeString(length);
				}

				inline std::wstring readWideString()
				{
					return read<std::wstring>();
				}
				
				// Line related:
				nativeString readLine();

				inline nativeString readstdLine() { return readLine(); }

				// Output routines:

				// This command will manually set the output-offset, blanking memory as it becomes "out of scope".
				// The region between the current offset, and the position specified will be "zeroed out".
				// Use this with caution; use 'outSeek' if the intention is to normally "seek".
				inline streamLocation setWrite(streamLocation position)
				{
					position = std::min(position, outbufferSize);

					if (position > writeOffset)
						flushOutputRegion(position-writeOffset);
					else
						flushOutputRegion(position, (writeOffset-position));

					return outSeek(position);
				}

				// This command will "snap" the output-length to the current offset.
				// This does not have the extra overhead of blanking memory.
				// This is different from 'setWrite', which simply seeks to the specified position, then blanks memory.
				// Use this to "snap" the output-length to the current offset.
				// This command reserves the right to change the output-offset, if it is out of bounds.
				inline streamLocation snapWriteLength()
				{
					// Assign the output-buffer's length, then return it:
					writeOffset = std::min(writeOffset, outbufferlen);
					outbufferlen = writeOffset;

					return outbufferlen;
				}

				// Buffer writing related:

				// This command will safely flush a region of the output-buffer.
				// This is useful when seeking around the output "stream".
				inline void flushOutputRegion(streamLocation position, size_type amount)
				{
					if (position+amount > outbufferSize)
						amount = (outbufferSize-position);

					memset(outbuffer + position, 0, amount);

					return;
				}

				// This will "flush" the amount specified, starting at the current write-offset.
				inline void flushOutputRegion(size_type amount)
				{
					memset(outbuffer + writeOffset, 0, amount);

					return;
				}

				// This will flush all bytes following the current write-offset.
				inline void flushOutputRegion()
				{
					flushOutputRegion(outbufferSize-writeOffset);

					return;
				}

				// This will set the output-length to the length specified.
				// The write-offset will be changed to the
				// length specified if it was out of bounds.
				inline void setOutputLength(streamLocation length)
				{
					outbufferlen = std::min(length, outbufferSize);
					writeOffset = std::min(writeOffset, outbufferlen);

					return;
				}

				// This will set the output-length to the current write-offset.
				inline void setOutputLength()
				{
					setOutputLength(writeOffset);

					return;
				}
				
				// The generic writing command for raw data. ('size' is in bytes)
				bool writeData(const void* input, size_type size, size_type input_offset=0);

				// This command will write the type specified, but it will not
				// automatically swap the appropriate bytes of the type.
				// To do this, please use the 'write' command.
				template <typename type> inline bool rawWrite(type data)
				{
					return IO::rawWrite<type>(*this, data);
				}

				template <typename type> inline bool write(type data)
				{
					return IO::write<type>(*this, data);
				}

				inline bool write(nativeString str, size_type length)
				{
					// Write the string and its length:
					writeLengthOfString(length);

					return writeNativeString(str.c_str(), (uqint)length);
				}

				// Wide strings are serialized and deserialized as 16-bit.
				inline bool write(std::wstring wstr, size_type length)
				{
					writeLengthOfString(length);

					std::basic_string<QSOCK_INT16> outStr(wstr.begin(), wstr.end());

					return writeData(outStr.c_str(), length*sizeof(QSOCK_INT16)); // outStr.size();
				}

				inline bool writeLengthOfString(size_type length)
				{
					return IO::writeLengthOfString(*this, length);
				}

				inline bool writeLengthOfString(nativeString s)
				{
					return writeLengthOfString(lengthOfString(s));
				}

				inline bool writeLengthOfString(std::wstring wstr)
				{
					return writeLengthOfString(lengthOfString(wstr));
				}

				inline bool writeIP(QSOCK_UINT32_LONG address)
				{
					write<addressType>(ADDRESS_TYPE_INTEGER);

					return write<QSOCK_UINT32_LONG>(address);
				}

				inline bool writeIP(nativeString address)
				{
					write<addressType>(ADDRESS_TYPE_STRING);
					return writeString(address);
				}

				inline bool writePort(nativePort port)
				{
					return write<nativePort>(port);
				}

				inline bool writeOct(uqchar data) { return write<uqchar>(data); }
				inline bool writeByte(uqchar data) { return writeOct(data); }
				inline bool writeChar(qchar data) { return write<qchar>(data); }
				inline bool writeBool(bool data) { return write<bool>(data); }

				inline bool writeShort(qshort data) { return write<qshort>(data); }
				inline bool writeInt(qint data) { return write<qint>(data); }
				inline bool writeLong(qlong data) { return write<qlong>(data); }

				inline bool writeFloat(qfloat data) { return write<qfloat>(data); }
				inline bool writeDouble(qdouble data) { return write<qdouble>(data); }
		
				bool UwriteBytes(const uqchar* data, size_type dataSize=0);
				bool writeBytes(const qchar* data, size_type dataSize=0);

				// This will "pad" the output by the amount specified.
				// This generally means inserting zeroes. To do this
				// with fewer safety checks, use 'flushOutputRegion'.
				bool padBytes(size_type amount=0);

				// Other types:

				// This does not serialize the length of the string, to do this, either use 'writeString', or use 'writeLine'.
				inline bool writeNativeString(nativeString str) { return writeBytes((const qchar*)str.c_str(), str.length()); }

				inline bool writeNativeString(const QSOCK_CHAR* str, uqint length=0) { return writeBytes((const qchar*)str, (length != 0) ? length : strlen(str)); }

				inline bool writeString(nativeString str)
				{
					return write<nativeString>(str);
				}

				inline bool writeString(nativeString str, size_type length)
				{
					return write(str, length);
				}

				inline bool writeWideString(std::wstring wstr)
				{
					return write<std::wstring>(wstr);
				}

				inline bool writeWideString(std::wstring wstr, size_type length)
				{
					return write(wstr, length);
				}

				// Line related:
				bool writeLine(const QSOCK_CHAR* strIn, size_type length=0);

				inline bool writeLine(std::string str) { return writeLine(str.c_str(), str.length()); }
				inline bool writestdLine(std::string str) { return writeLine(str); }

				// Operators:
				template <typename T>
				inline QStream& operator<<(T data)
				{
					write<T>(data);

					return *this;
				}

				template <typename T>
				inline QStream& operator>>(T& outputVariable)
				{
					outputVariable = read<T>();

					return *this;
				}
			protected:
				// Constructor(s) (Protected):

				// This command is completely "unsafe". Please call 'deallocateBuffers'
				// before using this on an already constructed 'QStream'.
				void allocateBuffers(size_type inBuffer_size, size_type outBuffer_size);

				// This works the same as 'allocateBuffers', the only difference is, this will assign the internal max-sizes.
				inline void allocateAndApplyBuffers(size_type inBuffer_size, size_type outBuffer_size)
				{
					// Allocate the requested buffers.
					allocateBuffers(inBuffer_size, outBuffer_size);

					// Set the maximum sizes of these buffers.
					inbufferSize = inBuffer_size;
					outbufferSize = outBuffer_size;

					return;
				}

				// Destructor(s) (Protected):

				// This command will deallocate the internal buffers,
				// as well as change any relevant meta-data.
				void deallocateBuffers();

				// Methods (Protected):
				// Nothing so far.
		} QPacket;

		// This class provides the base functionality for network-sockets.
		// For a fully featured socket class, use 'QSocket'.
		class basic_socket
		{
			public:
				// Typedefs:

				// This is used to represent when a packet is received. The arguments are:
				// The input-buffer, the amount used from that buffer, and the maximum size of the buffer.
				typedef std::function<void(uqchar*, size_type, size_type)> rawReceiveCallback;

				// Global variable(s):

				// A global boolean stating if sockets have been initialized or not.
				static bool socketsInitialized;

				// All meta-data specified by WinSock upon initialization (Windows only).
				#if defined(QSOCK_WINDOWS)
					static WSADATA* WSA_Data;
				#endif

				// Functions (Public):
				static bool initSockets();
				static bool deinitSockets();

				// IP format-conversion commands:
				static QSOCK_UINT32_LONG StringToIntIP(nativeString IP);
				static nativeString IntToStringIP(QSOCK_UINT32_LONG IP);

				// These commands are here for the sake of compatibility:
				static inline QSOCK_UINT32_LONG StringToIntIP(QSOCK_UINT32_LONG IP)
				{
					// Return the input.
					return IP;
				}

				static inline nativeString IntToStringIP(nativeString IP)
				{
					// Return the input.
					return IP;
				}

				static inline nonNativeIP nativeToNonNativeIP(nativeIP input)
				{
					#if !defined(QSOCK_IPVABSTRACT)
						return IntToStringIP(input);
					#else
						return StringToIntIP(input);
					#endif
				}

				static inline nativeIP nonNativeToNativeIP(nonNativeIP input)
				{
					#if !defined(QSOCK_IPVABSTRACT)
						return StringToIntIP(input);
					#else
						return IntToStringIP(input);
					#endif
				}

				// These commands are here for the sake of compatibility:
				static inline nativeIP nonNativeToNativeIP(nativeIP input)
				{
					return input;
				}

				static inline nonNativeIP nativeToNonNativeIP(nonNativeIP input)
				{
					return input;
				}

				static inline nativeString representIP(nativeIP input)
				{
					return nativeToNonNativeIP(input);
				}

				// Constructor(s):
				basic_socket();
				basic_socket(SOCKET internalSocket, bool full_control=false);

				// Destructor(s):
				virtual ~basic_socket();

				// Fields (Public):

				// The destinations used for sending and receiving packets.
				socketAddress si_Destination, so_Destination;

				// Methods (Public):
				inline bool isServer() const { return _isServer; }
				inline bool isClosed() const { return socketClosed; }

				// Internal (Don't use this unless you want platform dependent access):
				// This command is subject to change in the future.
				#if defined(QSOCK_WINDOWS)
					inline SOCKET& getSocket() { return _socket; }
				#else
					inline unsigned int& getSocket() { return _socket; }
				#endif

				inline nativeIP msgIP() const
				{
					return nativeMsgIP();
				}

				nativeString strMsgIP() const;
				QSOCK_UINT32_LONG intMsgIP() const;

				inline nativeIP nativeMsgIP() const
				{
					#if !defined(QSOCK_IPVABSTRACT)
						return intMsgIP();
					#else
						return strMsgIP();
					#endif
				}

				inline nonNativeIP nonNativeMsgIP() const
				{
					#if !defined(QSOCK_IPVABSTRACT)
						return strMsgIP();
					#else
						return intMsgIP();
					#endif
				}

				nativePort msgPort() const;

				// This returns the native address this socket last received from.
				inline socketAddress msgAddr() const
				{
					return si_Destination;
				}

				// Input related:

				// This is the preferred overload for call-back-based message detection.
				virtual void readAvail(uqchar* buffer, const size_type bufferSize, rawReceiveCallback onMessageReceived);

				// This acts as the primary implementation for 'readAvail'.
				// The current system used is partially asynchronous.
				// This means calling this manually isn't ideal, and may result in undefined behavior.
				virtual QSOCK_INT32 readAvail_Blocking(uqchar* buffer, const size_type bufferSize);

				// Output related:

				// This command only works on servers. Clients automatically call 'sendMsg':
				QSOCK_INT32 broadcastMsg(uqchar* outputBuffer, size_type outputBufferLength, nativePort port=(nativePort)0);
				
				// This overload is used for raw/native IPV4 addresses.
				QSOCK_INT32 sendMsg(uqchar* outputBuffer, size_type outputBufferLength, QSOCK_UINT32_LONG IP, nativePort port=(nativePort)0);

				// This overload is used for string IP addresses. (IPV4, IPV6)
				QSOCK_INT32 sendMsg(uqchar* outputBuffer, size_type outputBufferLength, nativeString strIP, nativePort port=(nativePort)0);

				QSOCK_INT32 sendMsg(uqchar* outputBuffer, size_type outputBufferLength);

				inline QSOCK_INT32 sendMsg(uqchar* outputBuffer, size_type outputBufferLength, socketAddress* outboundAddress)
				{
					if (!setupDestination(outboundAddress))
						return SOCKET_ERROR;

					return outputMessage(outputBuffer, outputBufferLength);
				}

				// Initialization related:
				bool connect(nativeString address, const nativePort ePort, nativePort iPort=(nativePort)0);
				bool host(const nativePort ePort);

				inline bool connect(QSOCK_UINT32_LONG address, const nativePort ePort, const nativePort iPort=(nativePort)0)
				{
					return connect(IntToStringIP(address), (nativePort)ePort, (nativePort)iPort);
				}

				inline bool directConnect(QSOCK_UINT32_LONG address, const nativePort ePort, const nativePort iPort=(nativePort)0)
				{
					return connect(address, (nativePort)ePort, (nativePort)iPort);
				}

				// Deinitialization related:
				virtual bool close();
			protected:
				// Functions (Protected):

				// This should be used to call 'readRemoteMessages' from another thread.
				// This will not create a new thread, but it will act as an easy to use entry-point.
				static void begin_readRemoteMessages(basic_socket* instance, uqchar* buffer, const size_type bufferSize);

				// Fields (Protected):

				// The final address-result(s):

				// 'boundAddress' points to the element of 'result' which is used to bind the internal socket.
				#if !defined(QSOCK_IPVABSTRACT)
					socketAddress* result = nullptr;
					socketAddress* boundAddress = nullptr;
				#else
					addrinfo* result = nullptr;
					addrinfo* boundAddress = nullptr;
				#endif

				// The hints used to evaluate addresses (Protocols, 'IP families', etc).
				addrinfo hints;

				// Internal socket:
				#if defined(QSOCK_WINDOWS)
					SOCKET _socket = INVALID_SOCKET;
				#else
					unsigned int _socket = INVALID_SOCKET;
				#endif

				// The connection port (Host: Local/Server port, Client: External / Remote server's port).
				nativePort port = 0;

				// A thread dedicated to waiting for (And potentially reading) inbound packets.
				std::thread incomingThread;

				// A call-back for packet/message notifications.
				rawReceiveCallback onMessageReceived;

				// Booleans / Flags:

				// A boolean describing the connection-mode; server or client.
				bool _isServer = false;

				// A simple boolean for the state of the socket.
				bool socketClosed = true; // false;

				// This specifies if the '_socket' variable may be modified.
				bool canCloseSocket = true;

				// This specifies if broadcasting is supported by this socket.
				bool broadcastSupported = false;

				// A boolean used to manually close 'incomingThread'.
				std::atomic<bool> incomingThreadSwitch;

				// Methods (Protected):

				// Initialization related:
				qint bindInternalSocket(qint filter=-1, const timeval* recvTimeout=nullptr);
				bool bindSocket(const nativePort port, qint nativeFilter, const timeval* nativeRecvTimeout);

				virtual bool bindSocket(const nativePort port);

				// Deinitialization related:
				inline qint shutdownInternalSocket()
				{
					qint response = shutdown(_socket, SD_BOTH);

					if (response == 0)
					{
						// Set the internal socket to an "invalid socket".
						_socket = INVALID_SOCKET;
					}

					return response;
				}

				bool closeSocket();

				// Packet-management related:

				// This is called every time a packet/message is received from 'incomingThread'.
				virtual void onIncomingMessage(uqchar* buffer, const size_type length, const size_type bufferSize);

				// This acts as the entry-point for asynchronous message reading.
				virtual void readRemoteMessages(uqchar* buffer, const size_type bufferSize);

				// This will output the buffer specified to this socket's destination.
				QSOCK_INT32 outputMessage(uqchar* outputBuffer, size_type outputBufferLength);

				// Destination related:
				bool setupDestinationV4(QSOCK_UINT32_LONG address, nativePort externalPort);
				bool setupDestination(std::string address, nativePort externalPort);

				inline bool setupDestination(QSOCK_UINT32_LONG address, nativePort externalPort)
				{
					return setupDestinationV4(address, externalPort);
				}

				inline bool setupDestination(socketAddress* outboundAddress)
				{
					memcpy(&so_Destination, outboundAddress, sizeof(socketAddress));

					// Return the default response.
					return true;
				}

				inline bool setupDestination()
				{
					return setupDestination(&si_Destination); // msgIP(), msgPort()
				}
		};

		class QSocket : public basic_socket, public QStream
		{
			public:
				// Constant variable(s):

				// The default max buffer-length for sockets.
				static const size_type DEFAULT_BUFFERLEN = 1024;

				// Fields (Public):

				// Constructor(s):
				QSocket(size_type bufferLength=DEFAULT_BUFFERLEN, bool fixByteOrder=true);

				// Destructor(s):
				~QSocket();

				// Methods (Public):

				// Deinitialization related:
				virtual bool close() override;

				// Input related:

				// This method is no longer 'const'.
				bool msgAvail();

				virtual QSOCK_INT32 readAvail_Blocking(uqchar* buffer, const size_type bufferSize) override;

				// This is the preferred overload for call-back-based message detection.
				// The 'callbackOnRemoteThread' argument specifies if the call-back
				// will be executed remotely, or when the main 'readAvail' overload is called.
				void readAvail(uqchar* buffer, const size_type bufferSize, rawReceiveCallback onMessageReceived, bool callbackOnRemoteThread);
				virtual void readAvail(uqchar* buffer, const size_type bufferSize, rawReceiveCallback onMessageReceived) override;

				inline void readAvail(rawReceiveCallback onMessageReceived, bool callbackOnRemoteThread)
				{
					// Call the thread-start routine.
					readAvail(inbuffer, inbufferSize, onMessageReceived, callbackOnRemoteThread);

					return;
				}

				// This acts as the front-end for the main message-detection routine.
				// This may be called continuously in order to read packets.
				// However, you may also call this once, then use the message call-back.
				QSOCK_INT32 readAvail();

				// Output related:

				// This command only works on servers. Clients automatically call 'sendMsg':
				inline QSOCK_INT32 broadcastMsg(nativePort port=(nativePort)0, bool resetLength=true)
				{
					return handleOutputOperation(basic_socket::broadcastMsg(outbuffer, outbufferlen, port), resetLength);
				}

				inline QSOCK_INT32 sendBroadcastMsg(nativePort port=(nativePort)0, bool resetLength=true)
				{
					return handleOutputOperation(broadcastMsg(port), resetLength);
				}
				
				// This overload is used for raw/native IPV4 addresses.
				inline QSOCK_INT32 sendMsg(QSOCK_UINT32_LONG IP, nativePort port = (nativePort)0, bool resetLength = true)
				{
					return handleOutputOperation(basic_socket::sendMsg(outbuffer, outbufferlen, IP, port), resetLength);
				}

				// This overload is used for string IP addresses. (IPV4, IPV6)
				inline QSOCK_INT32 sendMsg(nativeString strIP, nativePort port=(nativePort)0, bool resetLength=true)
				{
					return handleOutputOperation(basic_socket::sendMsg(outbuffer, outbufferlen, strIP, port), resetLength);
				}

				inline QSOCK_INT32 sendMsg(bool resetLength=true)
				{
					return handleOutputOperation(basic_socket::sendMsg(outbuffer, outbufferlen), resetLength);
				}

				inline QSOCK_INT32 sendMsg(socketAddress* outboundAddress, bool resetLength=true)
				{
					return handleOutputOperation(basic_socket::sendMsg(outbuffer, outbufferlen, outboundAddress), resetLength);
				}

				// When calling this method, or calling any of the general purpose send
				// methods, the output-offset will be restored to its default position.
				inline QSOCK_INT32 outputMessage(bool resetLength=true)
				{
					return handleOutputOperation(basic_socket::outputMessage(outbuffer, outbufferlen), resetLength);
				}

				// Macros:
				inline QSOCK_INT32 listen() { return readAvail(); }
				inline QSOCK_INT32 update() { return readAvail(); }
			protected:
				// Enumerator(s):
				enum msgState : unsigned char
				{
					MESSAGE_STATE_READING,
					MESSAGE_STATE_DONE,
					MESSAGE_STATE_WAITING,
					MESSAGE_STATE_AVAILABLE,
				};

				// Constant variable(s) (Protected):

				// The default message-polling timeout (Used for 'readAvail_Blocking', and similar functionality):
				static const long DEFAULT_RECV_TIMEOUT_SEC = 0;
				static const long DEFAULT_RECV_TIMEOUT_USEC = 25000; // ~250ms.

				// Fields (Protected):
				
				// A mutex used to represent that a message has been received.
				std::mutex packetMutex;
				
				// Used to block 'incomingThread' when handling this socket's internal buffer.
				std::condition_variable incomingThreadWait;

				// This is used to represent the primary input-buffer's state.
				std::atomic<msgState> messageState;

				// This specifies if callbacks should be executed remotely,
				// rather than on an explicit check on a local thread.
				// Please, DO NOT modify this while remote threads are running.
				bool callbackOnRemoteThread = false;

				// Methods (Protected):

				// Initialization related:
				virtual bool bindSocket(const nativePort port) override;

				// Deinitialization related:
				bool closeSocket();

				// Input related:
				void readRemoteMessages(uqchar* buffer, const size_type bufferSize) override;

				// Please view the 'basic_socket' class's documentation for details.
				virtual void onIncomingMessage(uqchar* buffer, const size_type length, const size_type bufferSize) override;

				// Output related:
				inline QSOCK_INT32 handleOutputOperation(QSOCK_INT32 responseCode, bool resetLength=true)
				{
					if (responseCode == SOCKET_ERROR)
						return SOCKET_ERROR; // response;

					// Flush the output-stream:
					if (resetLength)
						flushOutput();

					// Return the calculated response.
					return responseCode;
				}
		};

		// Namespace(s) (Implementations):
		namespace IO
		{
			// Functions:

			// Input:
			inline size_type readLengthOfString(QStream& packet)
			{
				return (size_type)packet.read<uqshort>();
			}

			template <typename type> inline type rawRead(QStream& packet)
			{
				type data;
			
				packet.readData(&data, sizeof(type));

				return data;
			}

			template <> inline QSOCK_UCHAR rawRead<QSOCK_UCHAR>(QStream& packet)
			{
				#ifndef QSOCK_THROW_EXCEPTIONS
					if (packet.canRead(sizeof(QSOCK_UCHAR)))
				#endif
					{
						// Hold the current read-offset.
						auto currentOffset = packet.readOffset;

						// Attempt to seek forward.
						packet.inSeekForward(sizeof(QSOCK_UCHAR));

						// Return the requested information.
						return (QSOCK_UCHAR)packet.inbuffer[currentOffset];
					}

				return 0;
			}

			template <typename type> inline type read(QStream& packet)
			{
				return packet.rawRead<type>();
			}

			template <> inline QSOCK_UINT16 read<QSOCK_UINT16>(QStream& packet) { return (packet.fixByteOrder) ? ntohs(packet.rawRead<QSOCK_UINT16>()) : packet.rawRead<QSOCK_UINT16>(); }

			template <> QSOCK_UINT32_LONG read<QSOCK_UINT32_LONG>(QStream& packet)
			{
				return ((packet.fixByteOrder) ? ntohl(packet.rawRead<QSOCK_UINT32_LONG>()) : packet.rawRead<QSOCK_UINT32_LONG>());
			}

			template <> inline QSOCK_UINT64 read<QSOCK_UINT64>(QStream& packet) { return (packet.fixByteOrder) ? ntohll(packet.rawRead<QSOCK_UINT64>()) : packet.rawRead<QSOCK_UINT64>(); }

			template <> inline QSOCK_FLOAT32 read<QSOCK_FLOAT32>(QStream& packet) { return (packet.fixByteOrder) ? ntohf(packet.rawRead<QSOCK_UINT32>()) : packet.rawRead<QSOCK_FLOAT32>(); }
			template <> inline QSOCK_FLOAT64 read<QSOCK_FLOAT64>(QStream& packet) { return (packet.fixByteOrder) ? ntohd(packet.rawRead<QSOCK_UINT64>()) : packet.rawRead<QSOCK_FLOAT64>(); }

			template <> inline nativeString read<nativeString>(QStream& packet)
			{
				// Read the string's length, then the string itself.
				return packet.readNativeString(readLengthOfString(packet));
			}

			// Wide strings are serialized and deserialized as 16-bit.
			template <> inline std::wstring read<std::wstring>(QStream& packet)
			{
				auto length = readLengthOfString(packet);
				
				std::basic_string<QSOCK_INT16> rawString((const QSOCK_INT16*)packet.simulatedReadBytes(length*sizeof(QSOCK_INT16)), length); // uqshort // sizeof(std::wstring::value_type)

				return std::wstring(rawString.begin(), rawString.end());
			}

			// Output:
			inline bool writeLengthOfString(QStream& packet, size_type length)
			{
				return packet.write<uqshort>((uqshort)length);
			}

			template <typename type> inline bool rawWrite(QStream& packet, type data)
			{
				return packet.writeData(&data, sizeof(type));
			}

			template <> inline bool rawWrite<QSOCK_UCHAR>(QStream& packet, QSOCK_UCHAR data)
			{
				#ifndef QSOCK_THROW_EXCEPTIONS
					if (packet.canWrite(sizeof(data)))
				#endif
					{
						// Hold the current output-offset.
						auto currentOffset = packet.writeOffset;

						// Attempt to seek forward.
						packet.outSeekForward(sizeof(data));

						// Copy the specified data.
						packet.outbuffer[currentOffset] = (uqchar)data;

						// Tell the user writing was successful.
						return true;
					}

				return false;
			}

			template <> inline bool write<QSOCK_UINT16>(QStream& packet, QSOCK_UINT16 data) { return (packet.fixByteOrder) ? packet.rawWrite<QSOCK_UINT16>(htons(data)) : packet.rawWrite<QSOCK_UINT16>(data); }
			template <> inline bool write<QSOCK_INT16>(QStream& packet, QSOCK_INT16 data) { return write<QSOCK_UINT16>(packet, (QSOCK_UINT16)data); }

			template <> inline bool write<QSOCK_UINT32_LONG>(QStream& packet, QSOCK_UINT32_LONG data)
			{
				return ((packet.fixByteOrder) ? packet.rawWrite<QSOCK_UINT32_LONG>(htonl(data)) : packet.rawWrite<QSOCK_UINT32_LONG>(data));
			}

			template <> inline bool write<QSOCK_UINT64>(QStream& packet, QSOCK_UINT64 data)
			{
				return packet.rawWrite<QSOCK_UINT64>((packet.fixByteOrder) ? htonll(data) : data);
			}

			template <> inline bool write<QSOCK_FLOAT32>(QStream& packet, QSOCK_FLOAT32 data) { return (packet.fixByteOrder) ? write<QSOCK_UINT32>(packet, htonf(data)) : rawWrite<QSOCK_FLOAT32>(packet, data); }
			template <> inline bool write<QSOCK_FLOAT64>(QStream& packet, QSOCK_FLOAT64 data) { return (packet.fixByteOrder) ? write<QSOCK_UINT64>(packet, htond(data)) : rawWrite<QSOCK_FLOAT64>(packet, data); }

			template <> inline bool write<nativeString>(QStream& packet, nativeString str)
			{
				return packet.write(str, lengthOfString(str));
			}

			template <> inline bool write<std::wstring>(QStream& packet, std::wstring wstr)
			{
				return packet.write(wstr, lengthOfString(wstr));
			}
		}
	}
}
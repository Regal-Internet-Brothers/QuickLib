#pragma once

// Includes:
#include <stdexcept>
#include <string>
#include <sstream>

// Namespace(s):
namespace quickLib
{
	namespace sockets
	{
		// Classes:

		// 'QSocket' forward declaration.
		class QSocket;

		// Exception classes (Boilerplate, placeholders, etc):
		class QSOCK_EXCEPTION : public std::runtime_error
		{
			public:
				// Constructor(s):
				QSOCK_EXCEPTION(const QSocket* target, const std::string& exception_name="QuickSock: Exception.");

				// Methods:
				virtual const std::string message() const throw() = 0;

				// Fields:
				const QSocket* sock;
		};

		class QSOCK_CONSTRUCTION_EXCEPTION final : public QSOCK_EXCEPTION
		{
			public:
				// Constructor(s):
				QSOCK_CONSTRUCTION_EXCEPTION(const QSocket* target);
		};

		// This kind of error should be thrown when an invalid read or write was performed.
		class QSOCK_OUT_OF_BOUNDS_EXCEPTION : public QSOCK_EXCEPTION
		{
			public:
				// Constructor(s):
				QSOCK_OUT_OF_BOUNDS_EXCEPTION(const QSocket* target, void* targetedBuffer, size_t bufferSize, size_t offsetInBuffer, const std::string& exception_name="QuickSock: Out of bounds exception.");

				virtual const std::string message() const throw() override
				{
					// Local variable(s):
					std::stringstream ss;

					ss << "Out of bounds exception: " << makeInfoStr();
					ss << " -- Invalid operations at relative location: ";
					ss << relativePosition;

					return ss.str();
				}

				virtual const std::string makeInfoStr() const
				{
					std::stringstream ss;

					ss << "AT ADDRESS: " << buffer << ", LENGTH: " << sizeofBuffer;

					return ss.str();
				}

				// Fields:
				void* buffer;

				size_t sizeofBuffer;
				size_t relativePosition;
		};

		class QSOCK_SEEK_EXCEPTION : public QSOCK_OUT_OF_BOUNDS_EXCEPTION
		{
			public:
				// Enumerator(s):
				enum seekMode : uqchar
				{
					SEEK_MODE_IN,
					SEEK_MODE_OUT,
				};

				// Constructor(s):
				QSOCK_SEEK_EXCEPTION(const QSocket* target, size_t bufferSize, size_t offsetInBuffer, seekMode mode, void* targetedBuffer=nullptr, const std::string& exception_name="QuickSock: Seek exception.");
				QSOCK_SEEK_EXCEPTION(const QSocket* target, seekMode mode, size_t position=0, const std::string& exception_name="QuickSock: Seek exception.");

				// Methods:
				virtual const std::string makeInfoStr() const
				{
					std::stringstream ss;

					ss << "AT MEMORY POSITION: " << buffer << ", LENGTH: " << sizeofBuffer;

					return ss.str();
				}

				// Fields:
				seekMode mode;
		};

		// This acts as a more descriptive error-type when dealing with read safety.
		// It is recommended that you use this when throwing read-exceptions.
		class QSOCK_READ_EXCEPTION : public QSOCK_OUT_OF_BOUNDS_EXCEPTION
		{
			public:
				// Constructor(s):
				QSOCK_READ_EXCEPTION(const QSocket* target, void* targetedBuffer, size_t bufferSize, size_t offsetInBuffer, const std::string& exception_name="QuickSock: Read exception.");

				// Methods:
				virtual const std::string message() const throw() override
				{
					std::stringstream ss;

					ss << "Read exception: " << makeInfoStr();
					ss << " -- Attempted to read at invalid position:";
					ss << " Relative: " << relativePosition;
					ss << " Global: " << (void*)((size_t)buffer + relativePosition);

					return ss.str();
				}
		};

		// This acts as a more descriptive error-type when dealing with write safety.
		// It is recommended that you use this when throwing write-exceptions.
		class QSOCK_WRITE_EXCEPTION : public QSOCK_OUT_OF_BOUNDS_EXCEPTION
		{
			public:
				// Constructor(s):
				QSOCK_WRITE_EXCEPTION(const QSocket* target, void* targetedBuffer, size_t bufferSize, size_t offsetInBuffer, const std::string& exception_name="QuickSock: Write exception.");

				// Methods:
				virtual const std::string message() const throw() override
				{
					std::stringstream ss;

					ss << "Write exception: " << makeInfoStr();
					ss << " -- Attempted to write at invalid position:";
					ss << " Relative: " << relativePosition;
					ss << " Global: " << (void*)((size_t)buffer + relativePosition);

					return ss.str();
				}
		};
	}
}
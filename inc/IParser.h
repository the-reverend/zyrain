/// @file IParser.h
/// @author rev

#pragma once

#include <string>

namespace RPW
{
	namespace TW
	{

		class IParser
		{
		public:
			virtual ~IParser( void ) {}
			virtual bool Parse( const std::string& data, bool eof = false ) = 0;
			virtual bool Parse( const char* const data, unsigned long len, bool eof = false ) = 0;
			virtual bool IsInitialized( void ) = 0;
		};

	}
}

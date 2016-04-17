#pragma once

#include "Socket.h"
#include "FileSystem.h"

#include <sstream>

class TwProxy;

namespace RPW
{
	namespace TW
	{
		class ZMenu
		{
		public:
			#pragma region ansi_scripts
			static const char* welcome;
			static const int welcome_len;
			static const char* newline;
			static const int newline_len;
			static const char* endhail;
			static const int endhail_len;
			static const char* goodbye;
			static const int goodbye_len;
			static const char* preamble;
			static const int preamble_len;
			static const char* prefix;
			static const int prefix_len;
			#pragma endregion

			ZMenu( void );
			ZMenu( const ZMenu& copy );
			virtual ~ZMenu( void );

			ZMenu& operator=( const ZMenu& copy );
			bool operator<( const ZMenu& rhs ) const;
			void Process( RPW::Core::Socket& sock, char c = 0 );
			void Process( RPW::Core::Socket& sock, const std::string& command );

			void ProcessLs( RPW::Core::Socket &sock, const std::string& glob );
			void ProcessList( RPW::Core::Socket& sock, const std::vector<std::string>& tokens, unsigned short permission );
			void PublishIpList( RPW::Core::Socket &sock, unsigned short permission  );
			void ProcessQuery( RPW::Core::Socket &sock, const std::string& command, const char* const delim, bool header, const char* const prefix, int prefix_len );

			bool IsInitialized( void ) const;
			bool Initialize( TwProxy* proxy );
			void Cleanup( void );
			bool InMenu( void );
			void UpdateLastLine( const char* const data, int len );
			std::ostringstream& Buffer();

		protected:
			bool m_inMenu;

			TwProxy* m_pProxy;
			RPW::Core::FileSystem m_fs;
			std::ostringstream buffer;
			std::ostringstream lastline;
			std::ostringstream commandline;
		};

	}
}
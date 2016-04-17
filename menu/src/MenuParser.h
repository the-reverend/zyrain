/// @file MenuParser.h
/// @author rev

#pragma once

#include "IParser.h"
#include "IMenuManager.h"
#include "Socket.h"

#include <vector>
#include <iostream>
#include <sstream>
#include <assert.h>

extern "C"
{
#include "FlexScannerData2.h"
}

typedef void* yyscan_t;

namespace RPW
{
	namespace TW
	{

		class MenuParser : public RPW::TW::IParser
		{
		public:
			MenuParser( void );
			MenuParser( const MenuParser& copy )
				: m_yyWrapText( copy.m_yyWrapText )
				, m_pParser( copy.m_pParser )
				, m_token0( copy.m_token0 )
				, m_pScannerData( copy.m_pScannerData )
				, m_scanner( copy.m_scanner )
				, m_pMenuManager( copy.m_pMenuManager )
				, inMenu( copy.inMenu )
			{
				if ( NULL != copy.m_pParser || NULL != m_pParser )
				{
					throw std::logic_error( "don't copy this object after it is initialized" );
				}
			}

			virtual ~MenuParser( void );

			MenuParser& operator=( const MenuParser& copy )
			{
				if ( this != &copy )
				{
					m_yyWrapText = copy.m_yyWrapText;
					m_pParser = copy.m_pParser;
					m_token0 = copy.m_token0;
					m_pScannerData = copy.m_pScannerData;
					m_scanner = copy.m_scanner;
					m_pMenuManager = copy.m_pMenuManager;
					inMenu = copy.inMenu;
				}

				if ( NULL != copy.m_pParser || NULL != m_pParser )
				{
					throw std::logic_error( "don't copy this object after it is initialized" );
				}

				return *this;
			}

			bool operator<( const MenuParser& rhs )
			{
				return reinterpret_cast<const void*>( this ) < reinterpret_cast<const void*>( &rhs );
			}

			void Inject( IMenuManager& menuManager );
			bool Initialize( void );
			void Cleanup( void );

			// IParser Interface
			virtual bool Parse( const std::string& data, bool eof = false );
			virtual bool Parse( const char* const data, unsigned long len, bool eof = false );
			virtual bool IsInitialized( void );

			// lemon support
			void ParseAccept( void );
			void SyntaxError( void );
			void ParseFailure( void );

			void EnterMenu( RPW::Core::Socket& sock )
			{
				const char* welcome = "\r"
				                      "\x1b[0m\n"
				                      "\r\n"
				                      "\x1b[32mHailing frequencies: [\x1b[1;33m<Enter> to request secure channel\x1b[;32m]\r"
				                      "\x1b[0m\n"
				                      "\r\n"
				                      "\x1b[35m=\x1b[33mCaptain Zyrain\r"
				                      "\x1b[0m\n"
				                      "\r\n"
				                      "\x1b[35mRequesting comm-link with \x1b[1;36mCaptain Zyrain\r"
				                      "\x1b[0m\n"
				                      "\x1b[35mSecure comm-link established, Captain.\r"
				                      "\x1b[0m\n"
				                      "\r\n"
				                      "\x1b[32mType private message [\x1b[1;33m<ENTER> to send line. Blank line to end transmission\x1b[0;32m]\r\n"
				                      "\x1b[35mP: \x1b[33m";
				const int len = ::strlen( welcome );

				assert( !inMenu );
				inMenu = true;

				sock.send( welcome, len );
			}

			void NewLine( RPW::Core::Socket& sock )
			{
				const char* newline = "\r"
				                      "\x1b[0m\n"
				                      "\x1b[35mP: \x1b[33m";
				const int len = ::strlen( newline );
				sock.send( newline, len );
			}

			bool InMenu( void ) { return inMenu; }
			void LeaveMenu( void )
			{
				inMenu = false;
			}
			void LeaveMenu( RPW::Core::Socket& sock )
			{
				const char* goodbye = "\r"
				                      "\x1b[0m\n"
				                      "\r\n"
				                      "\x1b[35mSecure comm-link terminated\r"
				                      "\x1b[0m\n"
				                      "\r\n";
				const int len = ::strlen( goodbye );

				sock.send( goodbye, len );
				sock.send( lastline.str() );
				sock.send( buffer.str() );
				buffer.str( "" ); // clear the stream
				inMenu = false;
			}

			void UpdateLastLine( const char* const data, int len )
			{
				char cr = 13;
				std::string s( data, len );
				std::string::size_type i = s.find_last_of( cr );

				if ( std::string::npos == i )
				{
					lastline << s;
				}
				else
				{
					lastline.str( "" ); // clear the stream
					UpdateLastLine( data + i + 1, len - i - 1 );
				}
			}

			std::ostringstream& Buffer() { return buffer; }

			FlexScannerData2& ScannerData() { return *m_pScannerData; }
			IMenuManager& MenuManager( void );

		protected:
			std::string m_yyWrapText;
			void* m_pParser;         ///< parser pointer
			long m_token0;           ///< default token type
			FlexScannerData2* m_pScannerData;
			yyscan_t m_scanner;

			RPW::TW::IMenuManager* m_pMenuManager;
			bool inMenu;
			std::ostringstream buffer;
			std::ostringstream lastline;
		};

	}
}

/// @file LemonParser.cpp
/// @brief parser main body code block
/// @author rev
/// @see http://www.hwaci.com/sw/lemon/

// Declare these parser globals here.  They are not declared in any
// other header files.

#include "StdAfx.h"

#include "parser.c" // include the lemon C output here

#include "LemonParser.h"
#include <string>
#include <iostream>

typedef struct yy_buffer_state *YY_BUFFER_STATE;

extern "C"
{
	int             yylex( yyscan_t );
	YY_BUFFER_STATE yy_scan_string( const char*, yyscan_t );
	void            yy_delete_buffer( YY_BUFFER_STATE, yyscan_t );
	int             yylex_init_extra( FlexScannerData1*, yyscan_t* );
	int             yylex_destroy( yyscan_t );
}

/// @brief constructor
RPW::TW::LemonParser::LemonParser( void )
	: m_token0( 0 )
	, m_pParser( 0 )
	, m_yyWrapText()
	, m_pActor( NULL )
	, m_pScannerData( NULL )
	, m_scanner( NULL )
{
}

/// @brief destructor
RPW::TW::LemonParser::~LemonParser()
{
	Cleanup();
}

void RPW::TW::LemonParser::Inject( IDataActor& actor )
{
	m_pActor = &actor;
}

bool RPW::TW::LemonParser::Parse( const std::string& data, bool eof )
{
	return Parse( data.c_str(), data.length(), eof );
}

bool RPW::TW::LemonParser::Parse( const char* const data, unsigned long len, bool eof )
{
	static int yv;

	if ( IsInitialized() )
	{
		// initialize the flex-generated lexical analyzer using the substring
		m_yyWrapText += std::string( data, len );
		size_t posEsc = m_yyWrapText.find_last_of( "\x1b" );
		if ( posEsc == std::string::npos ) { posEsc = 0; }

		size_t posCrLf = m_yyWrapText.find_last_of( "\r\n" );
		if ( posCrLf != std::string::npos ) { posCrLf++; }

		size_t pos = ( posEsc > posCrLf ) ? posEsc : posCrLf;

		if ( eof )
		{
			pos = m_yyWrapText.length();
		}

		if ( pos != std::string::npos )
		{
			YY_BUFFER_STATE yybs = yy_scan_string( m_yyWrapText.substr( 0, pos ).c_str(), m_scanner );

			m_yyWrapText = m_yyWrapText.substr( pos );

			// on EOF yylex will return 0
			while (( yv = yylex( m_scanner ) ) != 0 )
			{
				// call the lemon-generated parser
				TwParse( m_pParser, yv, m_token0, this );
			}

			// force the lemon-generated parser to flush it's state and discard any look-aheads
			//TwParse( m_pParser, 0, m_token0, this );

			yy_delete_buffer( yybs, m_scanner );
		}
	}

	return IsInitialized();
}

bool RPW::TW::LemonParser::Initialize( void )
{
	if ( !IsInitialized() )
	{
		// init scanner
		m_pScannerData = new FlexScannerData1();
		::memset( m_pScannerData, 0, sizeof( FlexScannerData1 ) );
		yylex_init_extra( m_pScannerData, &m_scanner );

		// init parser
		m_pParser = TwParseAlloc( malloc );
	}

	return IsInitialized();
}

void RPW::TW::LemonParser::Cleanup( void )
{
	if ( IsInitialized() )
	{
		// destroy parser
		Parse( "", 0, true );
		TwParse( m_pParser, 0, m_token0, this );
		TwParseFree( m_pParser, free );
		m_pParser = NULL;

		// destroy scanner
		yylex_destroy( m_scanner );
		delete m_pScannerData;
		m_pScannerData = NULL;
	}
}

bool RPW::TW::LemonParser::IsInitialized( void )
{
	return NULL != m_pParser && NULL != m_pActor;
}

/// @brief parser cleanup
void RPW::TW::LemonParser::ParseAccept( void )
{
	std::cout << "lemon parse accept" << std::endl;
}

/// @brief action on syntax error
void RPW::TW::LemonParser::SyntaxError( void )
{
	std::cout << "lemon syntax error" << std::endl;
}

void RPW::TW::LemonParser::ParseFailure( void )
{
	std::cout << "lemon parse failure" << std::endl;
}

RPW::TW::IDataActor& RPW::TW::LemonParser::Actor( void )
{
	if ( NULL == m_pActor )
	{
		throw std::logic_error( "LemonParser has not been initialized" );
	}

	return *m_pActor;
}

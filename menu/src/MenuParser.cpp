/// @file MenuParser.cpp
/// @brief parser main body code block
/// @author rev
/// @see http://www.hwaci.com/sw/lemon/

// Declare these parser globals here.  They are not declared in any
// other header files.

#include "StdAfx.h"

#include "parser.c" // include the lemon C output here

#include "MenuParser.h"
#include <string>

typedef struct yy_buffer_state *YY_BUFFER_STATE;

extern "C"
{
	int             yylex( yyscan_t );
	YY_BUFFER_STATE yy_scan_string( const char*, yyscan_t );
	void            yy_delete_buffer( YY_BUFFER_STATE, yyscan_t );
	int             yylex_init_extra( FlexScannerData2*, yyscan_t* );
	int             yylex_destroy( yyscan_t );
}

/// @brief constructor
RPW::TW::MenuParser::MenuParser( void )
	: m_token0( 0 )
	, m_pParser( 0 )
	, m_yyWrapText()
	, m_pScannerData( NULL )
	, m_scanner( NULL )
	, m_pMenuManager( NULL )
	, inMenu( false )
{
}

/// @brief destructor
RPW::TW::MenuParser::~MenuParser()
{
	Cleanup();
}

void RPW::TW::MenuParser::Inject( IMenuManager& menuManager )
{
	m_pMenuManager = &menuManager;
}

bool RPW::TW::MenuParser::Parse( const std::string& data, bool eof )
{
	return Parse( data.c_str(), data.length(), eof );
}

bool RPW::TW::MenuParser::Parse( const char* const data, unsigned long len, bool eof )
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
			///  @todo remove this line
			std::string temp = m_yyWrapText.substr( 0, pos );

			YY_BUFFER_STATE yybs = yy_scan_string( m_yyWrapText.substr( 0, pos ).c_str(), m_scanner );

			m_yyWrapText = m_yyWrapText.substr( pos );

			// on EOF yylex will return 0
			while (( yv = yylex( m_scanner ) ) != 0 )
			{
				// call the lemon-generated parser
				MenuParse( m_pParser, yv, m_token0, this );
			}

			// force the lemon-generated parser to flush it's state and discard any look-aheads
			MenuParse( m_pParser, 0, m_token0, this );
			yy_delete_buffer( yybs, m_scanner );
		}
	}

	return IsInitialized();
}

bool RPW::TW::MenuParser::Initialize( void )
{
	if ( !IsInitialized() )
	{
		// init scanner
		m_pScannerData = new FlexScannerData2();
		::memset( m_pScannerData, 0, sizeof( FlexScannerData2 ) );
		yylex_init_extra( m_pScannerData, &m_scanner );

		// init parser
		m_pParser = MenuParseAlloc( malloc );
	}

	return IsInitialized();
}

/// @todo rename this to Cleanup()
void RPW::TW::MenuParser::Cleanup( void )
{
	if ( IsInitialized() )
	{
		// destroy parser
		Parse( "", 0, true );
		MenuParse( m_pParser, 0, m_token0, this );
		MenuParseFree( m_pParser, free );
		m_pParser = NULL;

		// destroy scanner
		yylex_destroy( m_scanner );
		delete m_pScannerData;
		m_pScannerData = NULL;
	}
}

/// @todo rename this to IsInitialized()
bool RPW::TW::MenuParser::IsInitialized( void )
{
	return NULL != m_pParser && NULL != m_pMenuManager;
}

/// @brief parser cleanup
void RPW::TW::MenuParser::ParseAccept( void )
{
	MenuManager().accept();
}

/// @brief action on syntax error
void RPW::TW::MenuParser::SyntaxError( void )
{
	MenuManager().error();
}

void RPW::TW::MenuParser::ParseFailure( void )
{
	MenuManager().fail();
}

RPW::TW::IMenuManager& RPW::TW::MenuParser::MenuManager( void )
{
	if ( NULL == m_pMenuManager )
	{
		throw std::logic_error( "MenuManager is NULL" );
	}

	return *m_pMenuManager;
}

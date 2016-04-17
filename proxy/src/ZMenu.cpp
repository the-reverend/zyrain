#include "StdAfx.h"
#include "ZMenu.h"
#include "TwProxy.h"

#include "StringEx.h"
#include "LastErrorException.h"
#include "IpEndpoint.h"
#include "Utilities.h"
#include "SqliteDatabase.h"
#include "SqliteStatement.h"
#include "statements.h"

#include "rpw.tw.proxy_manifest.h"
#include "rpw.tw.proxy_manifest.uuid.h"
#include "rpw.core_manifest.h"
#include "rpw.core_manifest.uuid.h"
#include "rpw.sqlite_manifest.h"
#include "rpw.sqlite_manifest.uuid.h"

#include <algorithm>
#include <iomanip>

#pragma region ansi_scripts

const char* RPW::TW::ZMenu::welcome = "\r"
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
const int RPW::TW::ZMenu::welcome_len = ::strlen( welcome );

const char* RPW::TW::ZMenu::newline = "\r"
                                      "\x1b[0m\n"
                                      "\x1b[35mP: \x1b[33m";
const int RPW::TW::ZMenu::newline_len = ::strlen( newline );

const char* RPW::TW::ZMenu::endhail = "\r"
                                      "\x1b[0m\n"
                                      "\r\n"
                                      "\x1b[35mP: \x1b[33m";
const int RPW::TW::ZMenu::endhail_len = ::strlen( endhail );

const char* RPW::TW::ZMenu::goodbye = "\r"
                                      "\x1b[0m\n"
                                      "\r\n"
                                      "\x1b[35mSecure comm-link terminated\r"
                                      "\x1b[0m\n";
const int RPW::TW::ZMenu::goodbye_len = ::strlen( goodbye );

const char* RPW::TW::ZMenu::preamble = "\r\x1b[K";
const int RPW::TW::ZMenu::preamble_len = ::strlen( preamble );

const char* RPW::TW::ZMenu::prefix =  "\r\x1b[0m\n\x1b[32mP \x1b[1;36mZyrain\x1b[37m\x1b[1;36m \x1b[33m";
const int RPW::TW::ZMenu::prefix_len = ::strlen( prefix );

#pragma endregion

RPW::TW::ZMenu::ZMenu( void )
	: m_pProxy( NULL )
	, m_inMenu( false )
{
}

RPW::TW::ZMenu::ZMenu( const RPW::TW::ZMenu& copy )
	: m_pProxy( copy.m_pProxy )
	, m_inMenu( copy.m_inMenu )
{
}

RPW::TW::ZMenu::~ZMenu( void )
{
	Cleanup();
}

RPW::TW::ZMenu& RPW::TW::ZMenu::operator=( const RPW::TW::ZMenu & copy )
{
	if ( this != &copy )
	{
		m_pProxy = copy.m_pProxy;
		m_inMenu = copy.m_inMenu;
	}

	return *this;
}

bool RPW::TW::ZMenu::operator<( const RPW::TW::ZMenu& rhs ) const
{
	return reinterpret_cast<const void*>( this ) < reinterpret_cast<const void*>( &rhs );
}

void RPW::TW::ZMenu::Process( RPW::Core::Socket& sock, char c )
{
	if ( !m_inMenu )
	{
		sock.send( welcome, welcome_len );
		m_inMenu = true;
	}

	switch ( c )
	{
		case 0:
			// no character was passed
			break;

		case 27:
			if ( commandline.str().length() == 0 )
			{
				// commandline was empty, so exit the menu
				sock.send( goodbye, goodbye_len );
				sock.send( lastline.str() );
				sock.send( buffer.str() );
				buffer.str( "" ); // clear the stream
				m_inMenu = false;
			}
			else
			{
				std::ostringstream oss;
				std::string::size_type len = commandline.str().length();
				oss << std::setw( len ) << std::setfill( '\b' ) << '\b';
				oss << std::setw( len ) << std::setfill( ' ' ) << ' ';
				oss << std::setw( len ) << std::setfill( '\b' ) << '\b';
				sock.send( oss.str() );
				commandline.str( "" );
			}
			break;

		case '\n':
		case '\t':
			// ignore these keys
			break;

		case '\r':
			if ( commandline.str().length() == 0 )
			{
				// commandline was empty, so exit the menu
				sock.send( goodbye, goodbye_len );
				sock.send( lastline.str() );
				sock.send( buffer.str() );
				buffer.str( "" ); // clear the stream
				m_inMenu = false;
			}
			else
			{
				sock.send( newline, newline_len );
				Process( sock, commandline.str() );
				sock.send( endhail, endhail_len );
			}
			commandline.str( "" ); // erase the current commandline;
			break;

		case '\b': // handle backspace
		case 127:
			if ( commandline.str().length() > 0 )
			{
				sock.send( "\b \b", 3 );
				std::string cmd( commandline.str() );
				commandline.str( "" );
				commandline << cmd.substr( 0, cmd.length() - 1 );
			}
			break;

		default:
			commandline << c;
			sock.send( c );
			break;
	}
}

void RPW::TW::ZMenu::Process( RPW::Core::Socket& sock, const std::string& command )
{
	std::vector<std::string> tokens;
	RPW::Core::StringEx::split1( command, tokens );

	if ( tokens.size() > 0 )
	{
		sock.send( preamble, preamble_len );
		sock.send( prefix, prefix_len );

		// switch based on the first character of the first command
		bool syntax_error = true;
		std::string cmd = tokens[0];
		std::transform( cmd.begin(), cmd.end(), cmd.begin(), ::tolower );
		switch ( cmd[0] )
		{
			/// @todo show who is currently connected
			/// @todo tag an ip with a name for reference

			case 'b': // black
				if ( 0 == std::string( "black" ).find( cmd ) && tokens.size() <= 2 )
				{
					syntax_error = false;
					ProcessList( sock, tokens, TwProxy::BLACK );
				}
				break;

			case 'c': // cd
				if ( 0 == std::string( "cd" ).find( cmd ) && tokens.size() <= 2 )
				{
					syntax_error = false;
					try
					{
						if ( tokens.size() > 1 )
						{
							m_fs.cd( tokens[1].c_str() );
						}
						sock.send( m_fs.GetActivePath() );
						//sock.send( prefix, prefix_len );
						//sock.send( std::string( "and your momma says hi." ) );
					}
					catch ( const RPW::Core::LastErrorException& e )
					{
						std::ostringstream oss;
						oss << "error #" << e.getErrorCode() << " " << e.shortErrorText();
						sock.send( oss.str() );
					}
				}
				break;

			case 'g': // gray, grey
				if (( 0 == std::string( "gray" ).find( cmd )
				      || 0 == std::string( "grey" ).find( cmd ) ) && tokens.size() <= 2 )
				{
					syntax_error = false;
					ProcessList( sock, tokens, TwProxy::GRAY );
				}
				break;

			case '?': // help
			case 'h': // help, home
				if ( 0 == std::string( "help" ).find( cmd )
				      || 0 == std::string( "?" ).find( cmd ) )
				{
					syntax_error = false;
					std::ostringstream oss;
					oss << "zyrain commands";
					oss << prefix << std::setw( 70 ) << std::setfill( '-' ) << '-' << std::setfill( ' ' );
					oss << prefix << "black           = show all black list ip addresses";
					oss << prefix << "black clear     = clear the black list";
					oss << prefix << "black [ip]      = add an ip address to the black list";
					oss << prefix << "cd              = show the active directory";
					oss << prefix << "cd [path]       = change the active directory";
					oss << prefix << "gray            = show all gray list ip addresses";
					oss << prefix << "gray clear      = clear the gray list";
					oss << prefix << "gray [ip]       = add an ip address to the gray list";
					oss << prefix << "help            = show this help screen";
					oss << prefix << "home            = change the active directory to the home folder";
					oss << prefix << "ip              = show a list of all known ip addresses";
					oss << prefix << "kill            = list all currently running scripts by number";
					oss << prefix << "kill [number]   = kill a currently running script by number";
					oss << prefix << "kick            = show a list of all connected client addresses";
					oss << prefix << "kick [ip]       = kick a currently connected client";
					oss << prefix << "ls [glob]       = list the files in the active directory";
					oss << prefix << "root            = show all root list ip addresses";
					oss << prefix << "root clear      = clear the root list";
					oss << prefix << "root [ip]       = add an ip address to the root list";
					oss << prefix << "run             = list all scripts in the active directory";
					oss << prefix << "run [script]    = run a script";
					oss << prefix << "select ...      = perform a select sql query on the database";
					oss << prefix << "stats           = report some statistics";
					oss << prefix << "tag [ip] [name] = report some statistics";
					oss << prefix << "warps           = show the warp data from the database";
					oss << prefix << "white           = show all white list ip addresses";
					oss << prefix << "white clear     = clear the white list";
					oss << prefix << "white [ip]      = add an ip address to the white list";
					sock.send( oss.str() );
				}
				else if ( 0 == std::string( "home" ).find( cmd ) )
				{
					if ( tokens.size() == 1 )
					{
						syntax_error = false;
						m_fs.cd( m_fs.GetExecutingAssemblyPath().c_str() );
						sock.send( m_fs.GetActivePath() );
					}
				}
				break;

			case 'i': // ip
				if ( 0 == std::string( "ip" ).find( cmd ) && tokens.size() <= 2 )
				{
					syntax_error = false;
					ProcessList( sock, tokens, TwProxy::BLACK + 1 );
				}
				break;

			case 'k': // kick, kill
				if ( 0 == std::string( "kick" ).find( cmd ) && tokens.size() <= 2 )
				{
					syntax_error = false;
					
					if ( tokens.size() == 1 )
					{
						std::ostringstream oss;
						oss << "connected client addresses";
						oss << prefix;
						oss << std::setw( 70 ) << std::setfill( '-' ) << '-' << std::setfill( ' ' );
						std::set<unsigned long> list = m_pProxy->LhsIpList();
						for ( std::set<unsigned long>::iterator it = list.begin(); it != list.end(); ++it )
						{
							oss << prefix << RPW::Core::IpEndpoint::getAddressString( *it );
						}
						sock.send( oss.str() );
					}
					else
					{
						std::ostringstream oss;
						unsigned long ip = RPW::Core::IpEndpoint::getAddressULong( tokens[1].c_str() );
						if ( INADDR_NONE == ip )
						{
							oss << "invalid ip address : ";
						}
						else if ( TwProxy::LOCAL_UP_ULONG == ip )
						{
							oss << "cannot kick localhost : ";
						}
						else if ( m_pProxy->SocketIsRoot( sock ) )
						{
							if ( m_pProxy->KickIp( ip ) )
							{
								oss << "ip connection kicked : ";
							}
							else
							{
								oss << "ip connection not found : ";
							}
						}
						else
						{
							oss << "only root users can kick : ";
						}

						oss << tokens[1];
						sock.send( oss.str() );
					}
				}
				else if ( 0 == std::string( "kill" ).find( cmd ) && tokens.size() <= 2 )
				{
					syntax_error = false;

					if ( tokens.size() == 1 )
					{
						std::ostringstream oss;
						oss << "current active scripts";
						oss << prefix << std::setw( 70 ) << std::setfill( '-' ) << '-' << std::setfill( ' ' );
						oss << prefix << "-num- -sock- ------name-------";

						if ( NULL != m_pProxy )
						{
							for ( TwProxy::ScriptMapT::const_iterator it = m_pProxy->ScriptMap().begin(); it != m_pProxy->ScriptMap().end(); ++it )
							{
								if ( it->second.IsRunning() )
								{
									oss << prefix << it->second.ScriptInfo();
								}
							}
						}
						else
						{
							oss << prefix << "ZMenu not initialized";
						}

						sock.send( oss.str() );
					}
					else // tokens.size == 2
					{
						unsigned long number = 0;
						try
						{
							RPW::Core::Utilities::FromStringValue( tokens[1], number );
						}
						catch ( const std::runtime_error& )
						{
							syntax_error = true;
							break;
						}

						// start a named script in the current active directory
						if ( NULL != m_pProxy )
						{
							std::string name;
							if ( m_pProxy->KillScript( number, name ) )
							{
								std::ostringstream oss;
								oss << "killed script " << tokens[1];
								oss << " (" << name << ")";
								sock.send( oss.str() );
							}
							else
							{
								std::ostringstream oss;
								oss << "failed to kill script " << tokens[1];
								sock.send( oss.str() );
							}
						}
					}
				}
				break;

			case 'l': // ls
				if ( 0 == std::string( "ls" ).find( cmd ) && tokens.size() <= 2 )
				{
					syntax_error = false;
					if ( tokens.size() == 2 )
					{
						ProcessLs( sock, tokens[1] );
					}
					else
					{
						ProcessLs( sock, std::string( "*" ) );
					}
				}
				break;

			case 'r': // root, run
				if ( 0 == std::string( "root" ).find( cmd ) && tokens.size() <= 2 )
				{
					syntax_error = false;
					ProcessList( sock, tokens, TwProxy::ROOT );
				}
				else if ( 0 == std::string( "run" ).find( cmd ) && tokens.size() <= 2 )
				{
					syntax_error = false;

					if ( tokens.size() == 1 )
					{
						ProcessLs( sock, std::string( "*.lua" ) );
					}
					else // tokens.size == 2
					{
						// start a named script in the current active directory
						if ( NULL != m_pProxy )
						{
							if ( m_fs.FileExists( tokens[1].c_str() ) )
							{
								if ( m_pProxy->RunScript(
								         ZScript( sock.getSocket()
								                  , m_fs.GetActivePath().c_str()
								                  , tokens[1].c_str() ) ) )
								{
									sock.send( std::string( "started script: " ) );
									sock.send( tokens[1] );
								}
								else
								{
									sock.send( std::string( "failed to start script: " ) ) ;
									sock.send( tokens[1] );
								}
							}
							else
							{
								sock.send( std::string( "script not found: " ) ) ;
								sock.send( tokens[1] );
							}
						}
					}
				}
				break;

			case 's': // select, stats
				if ( std::string( "select" ) == cmd )
				{
					syntax_error = false;
					ProcessQuery( sock, command, " | ", true, prefix, prefix_len );
				}
				else if ( 0 == std::string( "stats" ).find( cmd ) && tokens.size() == 1 )
				{
					syntax_error = false;
					std::ostringstream oss;
					oss << "___ stats ___";

					if ( NULL != m_pProxy )
					{
						RPW::Core::IpEndpoint rep;
						m_pProxy->Rhs().getPeerName( rep );
						RPW::Core::IpEndpoint lep( m_pProxy->Rhs().getSocket() );

						oss << prefix;
						oss << "rhs socket " << static_cast<unsigned long>( m_pProxy->Rhs().getSocket() )
						    << " " << lep.getAddressString() << ":" << lep.getPort()
						    << " > " << rep.getAddressString() << ":" << rep.getPort();
					}

					if ( NULL != m_pProxy )
					{
						RPW::Core::IpEndpoint rep;
						sock.getPeerName( rep );
						RPW::Core::IpEndpoint lep( sock.getSocket() );

						oss << prefix;
						oss << "lhs socket " << static_cast<unsigned long>( sock.getSocket() )
						    << " " << lep.getAddressString() << ":" << lep.getPort()
						    << " > " << rep.getAddressString() << ":" << rep.getPort();
					}

					oss << prefix;
					oss << "manifest date : " << RPW_TW_MANIFEST_DATE;
					oss << prefix;
					oss << "manifest uuid : " << RPW_TW_MANIFEST_UUID;
					oss << prefix;
					oss << "core library uuid : " << RPW_CORE_MANIFEST_UUID;
					oss << prefix;
					oss << "sqlite library uuid : " << RPW_SQLITE_MANIFEST_UUID;
					sock.send( oss.str() );
				}
				break;

			case 't': // tag
				if ( 0 == std::string( "tag" ).find( cmd ) && tokens.size() == 3 )
				{
					syntax_error = false;
					/// @todo implement this
					sock.send( "not implemented yet..." );
				}
				break;

			case 'w': // warps, white
				if ( 0 == std::string( "warps" ).find( cmd ) && tokens.size() == 1 )
				{
					syntax_error = false;
					ProcessQuery( sock, SQL_SELECT_WARP_CIM, " ", true, prefix, prefix_len );
				}
				else if ( 0 == std::string( "white" ).find( cmd ) && tokens.size() <= 2 )
				{
					syntax_error = false;
					ProcessList( sock, tokens, TwProxy::WHITE );
				}
				break;

			default:
				break;
		}

		if ( syntax_error )
		{
			sock.send( std::string( "syntax error" ) );
		}
	}
}

void RPW::TW::ZMenu::ProcessLs( RPW::Core::Socket &sock, const std::string& glob )
{
	std::ostringstream oss;
	oss << "path = " << m_fs.GetActivePath() << '\\';
	oss << glob;
	oss << prefix << std::setw( 70 ) << std::setfill( '-' ) << '-' << std::setfill( ' ' );
	oss << prefix << "--size--- --name--------";

	try
	{
		std::vector<std::string> list;
		m_fs.ls( list, 1, glob.c_str() );

		for ( std::vector<std::string>::size_type i = 0; i < list.size(); ++i )
		{
			oss << prefix << list[i];
		}
	}
	catch ( const RPW::Core::LastErrorException& e )
	{
		oss << prefix << "error #" << e.getErrorCode() << " " << e.shortErrorText();
	}

	sock.send( oss.str() );
}

void RPW::TW::ZMenu::ProcessList( RPW::Core::Socket& sock, const std::vector<std::string>& tokens, unsigned short permission )
{
	if ( NULL != m_pProxy )
	{
		if ( tokens.size() == 1 )
		{
			PublishIpList( sock, permission );
		}
		else
		{
			if ( 0 == std::string( "clear" ).find( tokens[1] ) )
			{
				m_pProxy->ClearIpList( permission );
				sock.send( std::string( "list cleared" ) );
			}
			else
			{
				std::ostringstream oss;
				unsigned long ip = RPW::Core::IpEndpoint::getAddressULong( tokens[1].c_str() );
				if ( INADDR_NONE == ip )
				{
					oss << "invalid ip address : ";
				}
				else if ( m_pProxy->SocketIsRoot( sock ) )
				{
					m_pProxy->AddToIpList( ip, permission );
					oss << "ip address added : ";
				}
				else
				{
					oss << "only root users can add : ";
				}
				oss << tokens[1];
				sock.send( oss.str() );
			}
		}
	}
	else
	{
		sock.send( std::string( "ZMenu not initialized" ) );
	}
}
void RPW::TW::ZMenu::ProcessQuery( RPW::Core::Socket &sock, const std::string& command, const char* const delim, bool header, const char* const prefix, int prefix_len )
{
	std::ostringstream oss;
	oss << "sql = " << command;
	oss << prefix;
	oss << std::setw( 70 ) << std::setfill( '-' ) << '-';

	RPW::SQLite::SqliteDatabase* pDb = &m_pProxy->store().db();

	RPW::SQLite::SqliteStatement* s = NULL;
	try
	{
		s = &pDb->getPreparedStatement( command );
	}
	catch ( const RPW::SQLite::SqliteException& e )
	{
		oss << prefix;
		oss << e.what();
	}

	if ( NULL != s )
	{
		bool done = false;
		s->reset().step( done );

		if ( done )
		{
			oss << prefix;
			oss << "no table results";
		}
		else if ( header )
		{
			std::ostringstream oss1;
			std::ostringstream oss2;
			int c = s->get_column_count();
			for ( int i = 0; i < c; ++i )
			{
				if ( i > 0 )
				{
					oss1 << delim;
					oss2 << std::setw( ::strlen( delim ) ) << std::setfill( ' ' ) <<  ' ';
				}
				std::string cn = s->get_column_name( i );
				oss1 << cn;
				oss2 << std::setw( cn.length() ) << std::setfill( '-' ) << '-';
			}
			oss << prefix;
			oss << oss1.str();
			oss << prefix;
			oss << oss2.str();
		}

		while ( !done )
		{
			int c = s->get_column_count();

			oss << prefix;
			for ( int i = 0; i < c; ++i )
			{
				if ( i > 0 ) oss << delim;
				oss << s->get_column_text( i );
			}
			s->step( done );
		}
	}

	sock.send( oss.str() );
}

void RPW::TW::ZMenu::PublishIpList( RPW::Core::Socket &sock, unsigned short permission  )
{
	std::ostringstream oss;

	oss << "ip addresses";
	oss << prefix;
	oss << std::setw( 70 ) << std::setfill( '-' ) << '-' << std::setfill( ' ' );

	std::map<unsigned long, unsigned short> list = m_pProxy->IpList();
	for ( std::map<unsigned long, unsigned short>::const_iterator it = list.begin(); it != list.end(); ++it )
	{
		if ( permission > TwProxy::BLACK || it->second == permission )
		{
			oss << prefix;
			switch ( it->second )
			{
				case TwProxy::ROOT  : oss << std::setw(6) << "root ";  break;
				case TwProxy::WHITE : oss << std::setw(6) << "white "; break;
				case TwProxy::GRAY  : oss << std::setw(6) << "gray ";  break;
				case TwProxy::BLACK : oss << std::setw(6) << "black "; break;
				default             : oss << std::setw(6) << " ";   break;
			}
			oss << RPW::Core::IpEndpoint::getAddressString( it->first );
		}
	}

	sock.send( oss.str() );
}

bool RPW::TW::ZMenu::IsInitialized( void ) const
{
	return NULL != m_pProxy;
}

bool RPW::TW::ZMenu::Initialize( TwProxy* proxy )
{
	Cleanup();
	m_pProxy = proxy;
	return true;
}

void RPW::TW::ZMenu::Cleanup( void )
{
	m_pProxy = NULL;
}

bool RPW::TW::ZMenu::InMenu( void )
{
	return m_inMenu;
}

void RPW::TW::ZMenu::UpdateLastLine( const char* const data, int len )
{
	std::string s( data, len );
	std::string::size_type i = s.find_last_of( '\r' );

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

std::ostringstream& RPW::TW::ZMenu::Buffer()
{
	return buffer;
}

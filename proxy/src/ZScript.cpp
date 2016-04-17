#include "StdAfx.h"
#include "ZScript.h"
#include "TwProxy.h"
#include "StringEx.h"

extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

#include "luabind/luabind.hpp"

#include <iomanip>
#include <sstream>

unsigned long ZScript::Number = 0;

ZScript::ZScript( SOCKET sock, const char* const path, const char* const name, unsigned long maxTriggerAgeMs /*= 1000*/, unsigned long cleanTriggerAgeMs /*= 60000*/ )
	: m_path( path )
	, m_name( name )
	, m_originatingSocket( sock )
	, m_number( ++Number )
	, m_pThread( NULL )
	, m_triggerEvent( NULL, FALSE, FALSE, NULL )
	, m_abortEvent( NULL, TRUE, FALSE, NULL )
	, m_maxTriggerAgeMs( maxTriggerAgeMs )
	, m_isRunning( false )
{
}

ZScript::ZScript( const ZScript& copy )
	: m_pProxy( copy.m_pProxy )
	, m_path( copy.m_path )
	, m_name( copy.m_name )
	, m_originatingSocket( copy.m_originatingSocket )
	, m_number( copy.m_number )
	, m_pThread( NULL )
	, m_triggerEvent( NULL, FALSE, FALSE, NULL )
	, m_abortEvent( NULL, TRUE, FALSE, NULL )
	, m_maxTriggerAgeMs( copy.m_maxTriggerAgeMs )
	, m_isRunning( false )
{
}

ZScript::~ZScript( void )
{
	Cleanup();
}

std::string ZScript::ScriptFile( void )
{
	std::ostringstream oss;
	oss << m_path << '\\' << m_name;
	return oss.str();
}

void ZScript::Start( TwProxy& proxy )
{
	Cleanup();

	m_pProxy = &proxy;
	m_pThread = new RPW::Core::SimpleThread<ZScript>( *this, &ZScript::run );
	m_pThread->start();
}

bool ZScript::IsRunning( void ) const
{
	return NULL != m_pProxy && m_isRunning;
}

void ZScript::Cleanup( void )
{
	if ( NULL != m_pThread )
	{
		delete m_pThread;
		m_pThread = NULL;
	}

	if ( NULL != m_pProxy )
	{
		m_pProxy = NULL;
	}
}

ZScript& ZScript::operator=( const ZScript & rhs )
{
	if ( this != &rhs )
	{
		Cleanup();
		m_pProxy = rhs.m_pProxy;
		m_path = rhs.m_path;
		m_name = rhs.m_name;
		m_originatingSocket = rhs.m_originatingSocket;
		m_number = rhs.m_number;
		m_maxTriggerAgeMs = rhs.m_maxTriggerAgeMs;
		m_isRunning = false;
	}

	return *this;
}

bool ZScript::operator<( const ZScript& rhs ) const
{
	return m_number < rhs.m_number;
}

unsigned long ZScript::GetNumber( void ) const
{
	return m_number;
}

std::string ZScript::GetName( void ) const
{
	return m_name;
}

std::string ZScript::GetPath( void ) const
{
	return m_path;
}

std::string ZScript::ScriptInfo( void ) const
{
	std::ostringstream oss;
	oss << std::right << std::setw( 5 ) << m_number;
	oss << ' ';
	oss << std::right << std::setw( 6 ) << static_cast<unsigned long>( m_originatingSocket );
	oss << std::left << ' ' << m_name;
	return oss.str();
}

bool ZScript::createTrigger( unsigned short index, const std::string& pattern, ZTriggerType type, bool raw )
{
	std::cout << __FUNCTION__ << "(" << index << ", '" << pattern << "')" << std::endl;

	deleteTrigger( index );

	bool success = false;

	if ( index > 0 && pattern.length() > 0 )
	{
		RPW::Core::CriticalSection::Scope scope( m_cs );
		std::pair<TriggerMap::iterator, bool> p = m_triggerMap.insert( TriggerPair( index, ZTrigger( index, type, pattern ) ) );
		success = p.second; // successfully inserted the trigger into the map
	}

	return success;
}

bool ZScript::createTrigger( unsigned short index, const std::string& pattern )
{
	return createTrigger( index, pattern, ZTrigger::NORMAL, false );
}

bool ZScript::createRawTrigger( unsigned short index, const std::string& pattern )
{
	return createTrigger( index, pattern, ZTrigger::NORMAL, true );
}

bool ZScript::enableTrigger( unsigned short index )
{
	return enableTrigger( index, true );
}

bool ZScript::disableTrigger( unsigned short index )
{
	return enableTrigger( index, false );
}

bool ZScript::deleteTrigger( unsigned short index )
{
	std::cout << __FUNCTION__ << "(" << index << ")" << std::endl;
	assert( index > 0 );

	std::set<unsigned short>::iterator sit = m_triggerSet.find( index );
	if ( sit != m_triggerSet.end() )
	{
		m_triggerSet.erase( sit );
	}

	TriggerMap::iterator it = m_triggerMap.find( index );

	bool success = it != m_triggerMap.end();

	if ( it != m_triggerMap.end() )
	{
		try
		{
			m_triggerMap.erase( it );
		}
		catch ( std::out_of_range e )
		{
			success = false;
		}
	}

	return success;
}

bool ZScript::setTriggerReply( unsigned short index, const std::string& reply )
{
	std::cout << __FUNCTION__ << "(" << index << ", '" << reply << "')" << std::endl;
	assert( index > 0 );

	TriggerMap::iterator it = m_triggerMap.find( index );

	if ( it != m_triggerMap.end() )
	{
		it->second.ReplyMacro = reply;
		it->second.AutoReply = true;
	}

	return it != m_triggerMap.end();
}

bool ZScript::clearTriggerReply( unsigned short index )
{
	std::cout << __FUNCTION__ << "(" << index << ")" << std::endl;
	assert( index > 0 );

	TriggerMap::iterator it = m_triggerMap.find( index );

	if ( it != m_triggerMap.end() )
	{
		it->second.AutoReply = false;
	}

	return it != m_triggerMap.end();
}

long ZScript::waitForTrigger( unsigned short index, unsigned long timeoutMs )
{
	std::cout << __FUNCTION__ << "(" << index << ", " << timeoutMs << ")" << std::endl;

	// this method will return the index of the trigger waited for or one of the following error
	// codes:
	// ABORT - the script was aborted externally
	// TIMEOUT - the wait for trigger timed out
	// BADINDEX - the index supplied does not match any trigger, or it was out of range
	// NOP - there is nothing to wait for because the trigger is not active
	// FAIL - WaitForMultipleObjects returned WAIT_FAIL
	// note:  passing zero index will wait for *any* trigger

	if ( index > 0 )
	{
		TriggerMap::iterator it = m_triggerMap.find( index );
		if ( it == m_triggerMap.end() )
		{
			// the indexed trigger was not found in the map
			return ZScript::BADINDEX;
		}
		else if ( !it->second.Active )
		{
			// the indexed trigger was found in the map, but it is not active
			return ZScript::NOP;
		}
	}

	// setup the event handles to wait for
	std::vector<HANDLE> ha;
	ha.push_back( m_abortEvent );
	ha.push_back( m_triggerEvent );

	bool done = false;
	long result = ZScript::ABORT;

	while ( !done )
	{
		DWORD dw = ::WaitForMultipleObjects( ha.size(), &ha[0], FALSE, timeoutMs );

		switch ( dw )
		{
			case WAIT_OBJECT_0:
				// the script was aborted by a call to ZScript::abort()
				result = ZScript::ABORT;
				done = true;
				break;

			case WAIT_OBJECT_0 + 1:
			{
				// a trigger event was raised by ZScript::evaluate()
				RPW::Core::CriticalSection::Scope scope( m_cs );

				// empty out the queue of trigger events and process them
				while ( !m_triggerQueue.empty() )
				{
					ZTrigger t( m_triggerQueue.front() );
					m_triggerQueue.pop_front();

					// if the popped trigger has an index of 0, it is an abort trigger, so abort
					if ( 0 == t.Index )
					{
						result = ZScript::ABORT;
						done = true;
						break;
					}

					// if the popped trigger is active and matches the index we're looking for
					// then check if it has expired and break out.
					if ( t.Active && ( t.Index == index || index == 0 ) )
					{
						bool expired = ::GetTickCount64() - t.Tick > m_maxTriggerAgeMs;
						if ( !expired )
						{
							result = t.Index;
							m_lastTrigger = t;
							done = true;
							break;
						}
					}
				}
			}
			break;

			case WAIT_FAILED:
				result = ZScript::FAIL;
				done = true;
				break;

			case WAIT_TIMEOUT:
				result = ZScript::TIMEOUT;
				done = true;
				break;

			default:
				break;
		}
	}

	std::cout << "... " << __FUNCTION__ << "(" << index << ", " << timeoutMs << ") = " << result << std::endl;

	return result;
}

long ZScript::waitForTrigger( unsigned short index )
{
	return waitForTrigger( index, INFINITE );
}

long ZScript::waitForAnyTrigger( unsigned long timeoutMs )
{
	return waitForTrigger( 0, timeoutMs );
}

long ZScript::waitForAnyTrigger( void )
{
	return waitForTrigger( 0, INFINITE );
}

bool ZScript::send( const std::string& text )
{
	bool success = false;

	try
	{
		if ( NULL != m_pProxy )
		{
			m_pProxy->Rhs().send( text );
			success = true;
		}
	}
	catch ( const RPW::Core::SocketException& e )
	{
		std::cout << "ZScript::send " << e.what();
	}

	return success;
}

bool ZScript::write( const std::string& text )
{
	bool success = false;

	try
	{
		if ( NULL != m_pProxy )
		{
			RPW::Core::Socket* pSock = m_pProxy->Lhs( m_originatingSocket );
			if ( NULL != pSock )
			{
				pSock->send( text );
				success = true;
			}
		}
	}
	catch ( const RPW::Core::SocketException& e )
	{
		std::cout << "ZScript::write " << e.what();
	}

	return success;
}

void ZScript::evaluate( const std::string& rawGameText )
{
	RPW::Core::CriticalSection::Scope scope( m_cs );

	std::string::size_type left = 0;
	std::string::size_type right = rawGameText.find_first_of( "\r\n", left );

	while ( right != std::string::npos )
	{
		evaluateLine( rawGameText.substr( left, right - left ) );
		m_triggerSet.clear();
		m_currentLine.str( "" );
		left = right + 1;
		right = rawGameText.find_first_of( "\r\n", left );
	}

	if ( left < rawGameText.length() )
	{
		evaluateLine( rawGameText.substr( left ) );
	}
}

void ZScript::cleanQueue( void )
{
	RPW::Core::CriticalSection::Scope scope( m_cs );

	while ( !m_triggerQueue.empty() )
	{
		if ( ::GetTickCount64() - m_triggerQueue.front().Tick > m_maxTriggerAgeMs )
		{
			m_triggerQueue.pop_front();
		}
		else
		{
			break;
		}
	}
}

void ZScript::abort( void )
{
	m_abortEvent.set();
	RPW::Core::CriticalSection::Scope scope( m_cs );
	m_triggerQueue.push_front( ZTrigger() );
	m_triggerEvent.set();
}

std::string ZScript::lastLine( void ) const
{
	return m_lastTrigger.LastTriggerLineStripped;
}

std::string ZScript::lastLineRaw( void ) const
{
	return m_lastTrigger.LastTriggerLineRaw;
}

bool ZScript::enableTrigger( unsigned short index, bool enabled )
{
	std::cout << __FUNCTION__ << "(" << index << ", " << ( enabled ? "true" : "false" ) << ")" << std::endl;
	assert( index > 0 );

	TriggerMap::iterator it = m_triggerMap.find( index );

	if ( it != m_triggerMap.end() )
	{
		it->second.Active = enabled;
	}

	return it != m_triggerMap.end();
}

void ZScript::evaluateLine( const std::string& rawGameText )
{
	RPW::Core::CriticalSection::Scope scope( m_cs );

	m_currentLine << rawGameText;

	if ( rawGameText.length() > 0 )
	{
		std::string stripped = RPW::Core::StringEx::stripAnsi( m_currentLine.str() );
		std::cout << "evaluateLine('" << stripped << "')" << std::endl;

		for ( TriggerMap::iterator it = m_triggerMap.begin(); it != m_triggerMap.end(); ++it )
		{
			// don't set a trigger twice for the same line
			if ( m_triggerSet.find( it->first ) == m_triggerSet.end() && it->second.match( m_currentLine.str(), stripped ) )
			{
				// clean out any expired triggers
				cleanQueue();

				// insert this trigger into set for this line
				m_triggerSet.insert( it->first );

				// update the trigger's status
				it->second.LastTriggerLineRaw = m_currentLine.str();
				it->second.LastTriggerLineStripped = stripped;
				it->second.stampIt();

				if ( it->second.AutoReply )
				{
					m_pProxy->Rhs().send( it->second.ReplyMacro );
				}
				m_triggerQueue.push_back( ZTrigger( it->second ) );
				m_triggerEvent.set();
			}
		}
	}
}

int panic( lua_State* L )
{
	std::string err( lua_tostring( L, -1 ) );
	throw std::runtime_error( err.c_str() );
	return 0;
}

void ZScript::run( void )
{
	m_isRunning = true;
	m_triggerMap.clear();
	m_triggerSet.clear();
	m_triggerQueue.clear();

	lua_State* L = lua_open();

	luabind::open( L );

	luabind::module( L )[
	   luabind::class_<ZScript>( "Zyrain" )
	   .def( "createTrigger", ( bool( ZScript::* )( unsigned short, const std::string& ) )&ZScript::createTrigger )
	   .def( "createRawTrigger", ( bool( ZScript::* )( unsigned short, const std::string& ) )&ZScript::createRawTrigger )
	   .def( "enableTrigger", ( bool( ZScript::* )( unsigned short ) )&ZScript::enableTrigger )
	   .def( "disableTrigger", ( bool( ZScript::* )( unsigned short ) )&ZScript::disableTrigger )
	   .def( "deleteTrigger", ( bool( ZScript::* )( unsigned short ) )&ZScript::deleteTrigger )
	   .def( "setTriggerReply", ( bool( ZScript::* )( unsigned short, const std::string& ) )&ZScript::setTriggerReply )
	   .def( "clearTriggerReply", ( bool( ZScript::* )( unsigned short ) )&ZScript::clearTriggerReply )
	   .def( "waitForTrigger", ( long( ZScript::* )( unsigned short, unsigned long ) )&ZScript::waitForTrigger )
	   .def( "waitForTrigger", ( long( ZScript::* )( unsigned short ) )&ZScript::waitForTrigger )
	   .def( "waitForAnyTrigger", ( long( ZScript::* )( unsigned long ) )&ZScript::waitForAnyTrigger )
	   .def( "waitForAnyTrigger", ( long( ZScript::* )( void ) )&ZScript::waitForAnyTrigger )
	   .def( "send", ( bool( ZScript::* )( const std::string& ) )&ZScript::send )
	   .def( "write", ( bool( ZScript::* )( const std::string& ) )&ZScript::write )
	   .property( "line", &ZScript::lastLine )
	   .property( "rawLine", &ZScript::lastLineRaw )
	   .property( "tick", &ZScript::getTick )
	   .property( "maxTriggerAgeMs", &ZScript::getMaxTriggerAgeMs, &ZScript::setMaxTriggerAgeMs )
	];

	/// @todo http://lua-users.org/lists/lua-l/2005-11/msg00400.html
	//luaL_openlibs( L );
	lua_pushcfunction( L, luaopen_base );    lua_pushstring( L, "" );               lua_call( L, 1, 0 );  // Opens the basic library.
	lua_pushcfunction( L, luaopen_package ); lua_pushstring( L, LUA_LOADLIBNAME );  lua_call( L, 1, 0 );  // Opens the package library.
	lua_pushcfunction( L, luaopen_string );  lua_pushstring( L, LUA_STRLIBNAME );   lua_call( L, 1, 0 );  // Opens the string lib.
	lua_pushcfunction( L, luaopen_table );   lua_pushstring( L, LUA_TABLIBNAME );   lua_call( L, 1, 0 );  // Opens the table library.
	lua_pushcfunction( L, luaopen_math );    lua_pushstring( L, LUA_MATHLIBNAME );  lua_call( L, 1, 0 );  // Opens the math lib.
	lua_pushcfunction( L, luaopen_io );      lua_pushstring( L, LUA_IOLIBNAME );    lua_call( L, 1, 0 );  // Opens the I/O library.
	lua_pushcfunction( L, luaopen_os );      lua_pushstring( L, LUA_OSLIBNAME );    lua_call( L, 1, 0 );  // Opens the OS library.
	lua_pushcfunction( L, luaopen_debug );   lua_pushstring( L, LUA_DBLIBNAME );    lua_call( L, 1, 0 );  // Opens the debug library.

	lua_atpanic( L, panic );

	try
	{
		luabind::globals( L )["z"] = this;

		int n = luaL_loadfile( L, ScriptFile().c_str() );

		switch ( n )
		{
			case 0 :
			{
				switch ( lua_pcall( L, 0, 0, 0 ) )
				{
					case 0 : // successfully ran the script
					{
						// Read a global from the lua script
						//std::string lastLine = luabind::object_cast<std::string>(
						//                          luabind::globals( L )["lastLine"]
						//                       );
						//std::cout << "lastLine = '" << lastLine << "'" << std::endl;
					}
					break;

					case LUA_ERRRUN :
					case LUA_ERRMEM :
					case LUA_ERRERR :
						lua_error( L );

					default :
						throw std::runtime_error( "lua_pcall - unknown error" );
				}
			}
			break;

			case LUA_ERRFILE :
			case LUA_ERRSYNTAX :
			case LUA_ERRMEM :
				lua_error( L );

			default :
				throw std::runtime_error( "luaL_loadfile - unknown error" );
		}

	}
	catch ( const std::exception& e )
	{
		std::cerr << e.what() << std::endl;
		write( e.what() );
	}

	m_isRunning = false;
	try
	{
		lua_close( L );
	}
	catch ( const std::exception& e )
	{
		std::cerr << e.what() << std::endl;
		write( e.what() );
	}
}

#include "StdAfx.h"
#include "TwProxy.h"

#include "ForwardTable.h"

#include <string>
#include <iostream>

TwProxy::TwProxy( const RPW::Core::IpEndpoint& ep
                  , RPW::TW::SqliteStore& dataStore
                  , RPW::Core::EventHandle& exitHandle
                  , char hotkey /*= '~'*/
                  , DWORD keepaliveTimeout /*= 10*/ )
	: RPW::SocketProxy( ep )
	, m_pDataStore( &dataStore )
	, m_pExitHandle( &exitHandle )
	, m_hotkey( hotkey )
{
	m_parser.Inject( *m_pDataStore );
	m_parser.Initialize();

	RPW::Core::ForwardTable ft;
	Rhs().create( AF_INET, SOCK_STREAM, IPPROTO_TCP );
	Rhs().bind( RPW::Core::IpEndpoint( ft.getLocalIpForDestination( ep.getAddress() ), 0 ) );
	Rhs().SetupKeepAlive( keepaliveTimeout );
}

TwProxy::~TwProxy( void )
{
	AbortAllScripts();
	m_parser.Cleanup();
}

bool TwProxy::AcceptedSocketIsAllowed( RPW::Core::Socket& sock )
{
	bool allowed = false;

	RPW::Core::IpEndpoint ep;
	sock.getPeerName( ep );

	unsigned long ip = ep.getAddress();

	std::cout << "connection attempt from " << ep.getAddressString();

	if ( LOCAL_UP_ULONG == ip )
	{
		// localhost
		allowed = true;
		std::cout << " LOCALHOST" << std::endl;
	}
	else
	{
		RPW::Core::CriticalSection::Scope scope( m_csIpData );
		std::map<unsigned long, unsigned short>::iterator it = m_ipList.find( ip );
		if ( it == m_ipList.end() || it->second == TwProxy::GRAY )
		{
			// unknown user
			std::cout << " GRAY LISTED" << std::endl;
			sock.send( std::string( "your ip is on the gray list.\r\n" ) );
			sock.send( std::string( "your request to connect has been noted.  please try again later.\r\n" ) );
			sock.send( std::string( "disconnecting...\r\n" ) );
			AddToIpList( ip, TwProxy::GRAY );
			/// @todo send message to Rhs() that an unknown user attempted to connect
		}
		else if ( it->second >= TwProxy::BLACK )
		{
			// black listed
			std::cout << " BLACK LISTED" << std::endl;
			sock.send( std::string( "your ip is on the black list; goodbye...\r\n" ) );
		}
		else if ( it->second <= TwProxy::WHITE )
		{
			// white listed
			allowed = true;
			std::cout << " WHITE LISTED" << std::endl;
		}
	}

	return allowed;
}

bool TwProxy::SocketIsRoot( RPW::Core::Socket& sock )
{
	bool isRoot = false;

	RPW::Core::IpEndpoint ep;
	sock.getPeerName( ep );
	unsigned long ip = ep.getAddress();

	if ( LOCAL_UP_ULONG == ip )
	{
		// localhost
		isRoot = true;
	}
	else
	{
		RPW::Core::CriticalSection::Scope scope( m_csIpData );
		std::map<unsigned long, unsigned short>::iterator it = m_ipList.find( ip );
		if ( it != m_ipList.end() && it->second == TwProxy::ROOT )
		{
			// root listed
			isRoot = true;
		}
	}

	return isRoot;
}

void TwProxy::RhsOnClose( void )
{
	RPW::SocketProxy::RhsOnClose();
	m_pExitHandle->set();
}

void TwProxy::RhsOnReceive( const char* const data, int len )
{
	if ( len > 0 && data[0] == -1 )
	{
		std::cout << "rhs telnet sequence:";
		for ( int i = 0; i < len; i++ )
		{
			std::cout << " " << std::hex << ( static_cast<short>( data[i] ) & 0xff );
		}
		std::cout << std::endl;
	}

	m_parser.Parse( data, len );
	SendToScripts( data, len );
	RPW::SocketProxy::RhsOnReceive( data, len );
}

void TwProxy::RhsOnSocketError( unsigned long lastError )
{
	RPW::SocketProxy::RhsOnSocketError( lastError );
	Rhs().destroy();
}

void TwProxy::RhsOnKeepAlive( void )
{
	static const unsigned char k[] = { 0xFF, 0xF1 }; // IAC NOP

	try
	{
		Rhs().send( k, 2 );
		std::cout << "RhsOnKeepAlive" << std::endl;
	}
	catch ( const RPW::Core::SocketException& e )
	{
		std::cout << "RhsOnKeepAlive " << e.longErrorText() << std::endl;
	}
}

int TwProxy::LhsSend( LhsSocket& sock, const char* const data, int len )
{
	if ( IsInMenu( sock ) )
	{
		m_mapZMenus[ sock.getSocket()].Buffer() << std::string( data, len );
		return len;
	}
	else
	{
		m_mapZMenus[ sock.getSocket()].UpdateLastLine( data, len );
		return RPW::SocketProxy::LhsSend( sock, data, len );
	}
}

void TwProxy::LhsOnReceive( LhsSocket& sock, const char* const data, int len )
{
	assert( len >= 0 );

	if ( len > 0 && SocketIsRoot( sock ) )
	{
		if ( data[0] == -1 )
		{
			if ( len > 0 && data[0] == -1 )
			{
				std::cout << "lhs telnet sequence:";
				for ( int i = 0; i < len; i++ )
				{
					std::cout << " " << std::hex << ( static_cast<short>( data[i] ) & 0xff );
				}
				std::cout << std::endl;
			}

			// this is a telnet negotiation, so just send it to the base method
			RPW::SocketProxy::LhsOnReceive( sock, data, len );
		}
		else if ( IsInMenu( sock ) )
		{
			// IsInMenu() has the side effect of making sure that the menu parser
			// map is populated, which is necessary for the rest of the logic
			// in this method to work.

			RPW::TW::ZMenu* pzm = &m_mapZMenus[ sock.getSocket()];

			int i = 0;
			for ( i = 0; i < len && pzm->InMenu(); ++i )
			{
				// parse one character at a time
				pzm->Process( sock, *( data + i ) );
			}

			if ( i < len )
			{
				// we exited the menu before all the data was handled
				// so recurse through and handle the unparsed data
				LhsOnReceive( sock, data + i, len - i );
			}
		}
		else
		{
			// find the hotkey character in the data
			const char* p = static_cast<const char*>( ::memchr( data, m_hotkey, len ) );

			if ( NULL == p )
			{
				// hotkey was not found, so just call the base method
				RPW::SocketProxy::LhsOnReceive( sock, data, len );
			}
			else if ( data == p )
			{
				// the hotkey was the first character in the data string
				m_mapZMenus[ sock.getSocket()].Process( sock );

				if ( len > 1 )
				{
					// recurse through and process the menu keystrokes
					LhsOnReceive( sock, data + 1, len - 1 );
				}
			}
			else if ( data + len - 1 == p )
			{
				// the hotkey was the last character in the data string
				// so call the base method on all of the data except the hotkey
				RPW::SocketProxy::LhsOnReceive( sock, data, len - 1 );

				m_mapZMenus[ sock.getSocket()].Process( sock );
			}
			else
			{
				// call base method on the portion of the string before
				RPW::SocketProxy::LhsOnReceive( sock, data, p - data );

				m_mapZMenus[ sock.getSocket()].Process( sock );

				// recurse through and process the menu keystrokes
				LhsOnReceive( sock, p + 1, len - ( p - data ) - 1 );
			}
		}
	}
}

void TwProxy::LhsOnCloseAll( void )
{
	Rhs().destroy();
}

bool TwProxy::IsInMenu( LhsSocket& sock )
{
	std::map<SOCKET, RPW::TW::ZMenu>::iterator it = m_mapZMenus.find( sock.getSocket() );

	if ( it == m_mapZMenus.end() )
	{
		// add a menu parser object to the map
		std::pair< std::map<SOCKET, RPW::TW::ZMenu>::iterator, bool> p =
		   m_mapZMenus.insert( std::pair<SOCKET, RPW::TW::ZMenu>( sock.getSocket(), RPW::TW::ZMenu() ) );

		assert( p.second ); // why wouldn't it perform the insert?
		if ( p.second )
		{
			// the menu parser object was inserted
			p.first->second.Initialize( this ); // start the parser
		}
	}

	return it != m_mapZMenus.end() && it->second.InMenu();
}

void TwProxy::AddToIpList( unsigned long ip, unsigned short permission )
{
	RPW::Core::CriticalSection::Scope scope( m_csIpData );
	m_ipList[ip] = permission;
}

bool TwProxy::RemoveFromIpList( unsigned long ip, unsigned short permission )
{
	bool success = false;

	RPW::Core::CriticalSection::Scope scope( m_csIpData );

	std::map<unsigned long, unsigned short>::iterator it = m_ipList.find( ip );
	if ( it != m_ipList.end() && it->second == permission )
	{
		m_ipList.erase( it );
		success = true;
	}

	return success;
}

void TwProxy::ClearIpList( unsigned short permission )
{
	RPW::Core::CriticalSection::Scope scope( m_csIpData );

	std::map<unsigned long, unsigned short>::iterator it = m_ipList.begin();

	while ( it != m_ipList.end() )
	{
		if ( it->second == permission )
		{
			// the iterator gets invalidated by erase but the ++ unary operator
			// creates a 'magical' copy in the background which is what gets passed
			// into the erase() method and goes out of scope leaving us with
			// an iterator that is still valid and incremented.  yay.
			m_ipList.erase( it++ );
		}
		else
		{
			++it;
		}
	}
}

const std::map<unsigned long, unsigned short>& TwProxy::IpList( void )
{
	return m_ipList;
}

std::set<unsigned long> TwProxy::LhsIpList( void )
{
	std::set<unsigned long> list;

	RPW::Core::CriticalSection::Scope scope( m_csLhsSockets );
	for ( std::set<LhsSocket>::iterator it = m_lhsSockets.begin(); it != m_lhsSockets.end(); ++it )
	{
		unsigned long ip = it->getIp();
		if ( it->isValid() && list.find( ip ) != list.end() )
		{
			list.insert( ip );
		}
	}

	return list;
}

bool TwProxy::KickIp( unsigned long ip )
{
	std::set<RPW::Core::Socket*> list = LhsByPeer( ip );

	for ( std::set<RPW::Core::Socket*>::iterator it = list.begin(); it != list.end(); ++it )
	{
		(*it)->destroy();
	}

	return list.size() > 0;
}

bool TwProxy::RunScript( ZScript& script )
{
	bool success = false;

	PruneScripts();

	std::pair<TwProxy::ScriptMapT::iterator, bool> p = m_mapScripts.insert( TwProxy::ScriptPairT( script.GetNumber(), script ) );

	if ( p.second )
	{
		// successfully inserted the script into the map
		p.first->second.Start( *this );
		success = true;
	}

	return success;
}

bool TwProxy::KillScript( unsigned long number, __out std::string& name )
{
	bool success = false;
	TwProxy::ScriptMapT::iterator it = m_mapScripts.find( number );

	if ( it != m_mapScripts.end() )
	{
		// found the script in the set
		name = it->second.GetName();
		it->second.abort();
		success = true;
	}

	return success;
}

void TwProxy::PruneScripts( void )
{
	// do some script map maintenance.  remove any scripts that are not running.
	TwProxy::ScriptMapT::iterator it = m_mapScripts.begin();
	while ( it != m_mapScripts.end() )
	{
		if ( !it->second.IsRunning() )
		{
			std::cout << "removing dormant script " << it->second.GetName() << " from the map" << std::endl;
			// the iterator gets invalidated by erase but the ++ unary operator
			// creates a 'magical' copy in the background which is what gets passed
			// into the erase() method and goes out of scope leaving us with
			// an iterator that is still valid and incremented.  yay.
			m_mapScripts.erase( it++ );
		}
		else
		{
			++it;
		}
	}
}

void TwProxy::AbortAllScripts( void )
{
	for ( TwProxy::ScriptMapT::iterator it = m_mapScripts.begin(); it != m_mapScripts.end(); ++it )
	{
		if ( it->second.IsRunning() )
		{
			std::cout << "aborting script " << it->second.GetName() << " from the map" << std::endl;
			it->second.abort();
		}
	}
}

void TwProxy::SendToScripts( const char* const data, int len )
{
	std::string d( data, len );
	for ( TwProxy::ScriptMapT::iterator it = m_mapScripts.begin(); it != m_mapScripts.end(); ++it )
	{
		if ( it->second.IsRunning() )
		{
			it->second.evaluate( d );
		}
	}
}

const TwProxy::ScriptMapT& TwProxy::ScriptMap( void )
{
	return m_mapScripts;
}

RPW::TW::SqliteStore& TwProxy::store( void )
{
	return *m_pDataStore;
}

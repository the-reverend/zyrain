#pragma once

#include "SocketProxy.h"
#include "IpEndpoint.h"
#include "EventHandle.h"
#include "SqliteStore.h"
#include "LemonParser.h"
#include "ZMenu.h"
#include "ZScript.h"
#include "CriticalSection.h"

#include <map>
#include <set>

class TwProxy : public RPW::SocketProxy
{
public:
	static const unsigned short ROOT = 0;
	static const unsigned short WHITE = 1;
	static const unsigned short GRAY = 2;
	static const unsigned short BLACK = 3;
	static const unsigned long LOCAL_UP_ULONG = 2130706433; // 127.0.0.1

	typedef std::map<unsigned long, ZScript> ScriptMapT;
	typedef std::pair<unsigned long, ZScript> ScriptPairT;

	TwProxy( const RPW::Core::IpEndpoint& ep
	         , RPW::TW::SqliteStore& dataStore
	         , RPW::Core::EventHandle& exitHandle
	         , char hotkey = '~'
             , DWORD keepaliveTimeout = 10 );
	virtual ~TwProxy( void );

	virtual bool AcceptedSocketIsAllowed( RPW::Core::Socket& sock );
	bool SocketIsRoot( RPW::Core::Socket& sock );

	virtual void RhsOnClose( void );
	virtual void RhsOnReceive( const char* const data, int len );
	virtual void RhsOnSocketError( unsigned long lastError );
	virtual void RhsOnKeepAlive( void );

	virtual int LhsSend( LhsSocket& sock, const char* const data, int len );
	virtual void LhsOnReceive( LhsSocket& sock, const char* const data, int len );
	virtual void LhsOnCloseAll( void );

	bool IsInMenu( LhsSocket& sock );

	void AddToIpList( unsigned long ip, unsigned short permission );
	bool RemoveFromIpList( unsigned long ip, unsigned short permission );
	void ClearIpList( unsigned short permission );
	const std::map<unsigned long, unsigned short>& IpList( void );

	std::set<unsigned long> LhsIpList( void );
	bool KickIp( unsigned long ip );

	bool RunScript( ZScript& script );
	bool KillScript( unsigned long number, __out std::string& name );
	const ScriptMapT& ScriptMap( void );

	void PruneScripts( void );
	void AbortAllScripts( void );
	void SendToScripts( const char* const data, int len );

	RPW::TW::SqliteStore& store( void );

protected:
	RPW::TW::LemonParser m_parser;
	RPW::TW::SqliteStore* m_pDataStore;
	RPW::TW::IParser* m_pMenuParser;
	RPW::Core::EventHandle* m_pExitHandle;

	char m_hotkey;
	std::map<SOCKET, RPW::TW::ZMenu> m_mapZMenus;
	RPW::Core::CriticalSection m_csIpData;
	std::map<unsigned long, unsigned short> m_ipList;
	std::map<unsigned long, std::string> m_ipTags;

	std::map<unsigned long, ZScript> m_mapScripts;
};

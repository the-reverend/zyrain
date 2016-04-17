#pragma once

#include "Socket.h"
#include "ZTrigger.h"
#include "CriticalSection.h"
#include "EventHandle.h"
#include "ErrorCodeException.h"
#include "SimpleThread.h"

#include <assert.h>
#include <iostream>
#include <string>
#include <map>
#include <set>
#include <sstream>
#include <deque>
#include <stdexcept>

class TwProxy;

class ZScript
{
public:
	static const long TIMEOUT = 0L;
	static const long ABORT = -1L;
	static const long BADINDEX = -2L;
	static const long NOP = -3L;
	static const long FAIL = -4L;

	static unsigned long Number;

	ZScript( SOCKET sock, const char* const path, const char* const name, unsigned long maxTriggerAgeMs = 1000, unsigned long cleanTriggerAgeMs = 60000 );
	ZScript( const ZScript& copy );
	~ZScript( void );
	std::string ScriptFile( void );
	void Start( TwProxy& proxy );
	bool IsRunning( void ) const;
	void Cleanup( void );
	ZScript& operator=( const ZScript& rhs );
	bool operator<( const ZScript& rhs ) const;
	unsigned long GetNumber( void ) const;
	std::string GetName( void ) const;
	std::string GetPath( void ) const;
	std::string ScriptInfo( void ) const;

	bool createTrigger( unsigned short index, const std::string& pattern, ZTriggerType type, bool raw );
	bool createTrigger( unsigned short index, const std::string& pattern );
	bool createRawTrigger( unsigned short index, const std::string& pattern );
	bool enableTrigger( unsigned short index );
	bool disableTrigger( unsigned short index );
	bool deleteTrigger( unsigned short index );
	bool setTriggerReply( unsigned short index, const std::string& reply );
	bool clearTriggerReply( unsigned short index );

	long waitForTrigger( unsigned short index, unsigned long timeoutMs );
	long waitForTrigger( unsigned short index );
	long waitForAnyTrigger( unsigned long timeoutMs );
	long waitForAnyTrigger( void );

	bool send( const std::string& text );
	bool write( const std::string& text );

	void evaluate( const std::string& rawGameText );

	void cleanQueue( void );
	void abort( void );

	std::string lastLineRaw( void ) const;
	std::string lastLine( void ) const;
	unsigned long getMaxTriggerAgeMs( void ) { return m_maxTriggerAgeMs; }
	void setMaxTriggerAgeMs( unsigned long ms ) { m_maxTriggerAgeMs = ms; }
	unsigned long getTick( void ) { return ::GetTickCount(); }

protected:
	bool enableTrigger( unsigned short index, bool enabled );
	void evaluateLine( const std::string& rawGameText );
	void run( void );

	TwProxy* m_pProxy;
	std::string m_path;
	std::string m_name;
	SOCKET m_originatingSocket; // for reference only
	unsigned long m_number;

	typedef std::map<unsigned short, ZTrigger> TriggerMap;
	typedef std::pair<unsigned short, ZTrigger> TriggerPair;

	RPW::Core::SimpleThread<ZScript>* m_pThread;

	TriggerMap m_triggerMap; ///< keep track of all defined triggers
	std::ostringstream m_currentLine; ///< keep track of the current line - the socket may append to the current line causing it to be re-evaluated for trigger matches
	std::set<unsigned short> m_triggerSet; ///< keep track of triggers that were set on the current line
	std::deque<ZTrigger> m_triggerQueue; ///< list of triggers that have happened
	ZTrigger m_lastTrigger;
	RPW::Core::CriticalSection m_cs;
	RPW::Core::EventHandle m_triggerEvent;
	RPW::Core::EventHandle m_abortEvent;
	unsigned long m_maxTriggerAgeMs;
	bool m_isRunning;
private:
	ZScript( void ) {};
};


#pragma once

#include "deelx.h"

#include <windows.h>
#include <string>

enum ZTriggerType { ZT_Normal = 0, ZT_Regex = 1 };
class ZTrigger
{
public:
	static const ZTriggerType NORMAL = ZT_Normal;
	static const ZTriggerType REGEX = ZT_Regex;

	ZTrigger( unsigned long index = 0,
	          ZTriggerType type = ZT_Normal,
	          const std::string& pattern = "",
	          bool raw = false,
	          bool autoReply = false,
	          const std::string& replyMacro = "",
	          ULONGLONG tick = 0 );
	ZTrigger( const ZTrigger& trigger );
	virtual ~ZTrigger( void );

	ZTrigger& operator=( const ZTrigger& rhs );
	bool operator<( const ZTrigger& rhs );
	void stampIt( void );
	bool match( const std::string& raw, const std::string& stripped ) const;

	unsigned long Index;
	ZTriggerType Type;
	std::string Pattern;
	CRegexpT<char> Regex;
	bool Raw;
	bool Active;
	bool AutoReply;
	std::string ReplyMacro;
	std::string LastTriggerLineRaw;
	std::string LastTriggerLineStripped;
	ULONGLONG Tick;

protected:
	bool match( const std::string& text ) const;
};

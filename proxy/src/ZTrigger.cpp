#include "StdAfx.h"
#include "ZTrigger.h"

ZTrigger::ZTrigger( unsigned long index /*= 0*/
                    , ZTriggerType type /*= normal*/
                    , const std::string& pattern /*= ""*/
                    , bool raw /*=false*/
                    , bool autoReply /*= false*/
                    , const std::string& replyMacro /*= ""*/
                    , ULONGLONG tick /*= 0*/ )
	: Index( index )
	, Type( type )
	, Pattern( pattern )
	, Regex( type == ZTrigger::REGEX ? pattern.c_str() : NULL )
	, Raw( raw )
	, Active( false )
	, AutoReply( autoReply )
	, ReplyMacro( replyMacro )
	, Tick( tick )
{
}

ZTrigger::ZTrigger( const ZTrigger& trigger )
	: Index( trigger.Index )
	, Type( trigger.Type )
	, Pattern( trigger.Pattern )
	, Regex( trigger.Type == ZTrigger::REGEX ? trigger.Pattern.c_str() : NULL )
	, Raw( trigger.Raw )
	, Active( trigger.Active )
	, AutoReply( trigger.AutoReply )
	, ReplyMacro( trigger.ReplyMacro )
	, LastTriggerLineRaw( trigger.LastTriggerLineRaw )
	, LastTriggerLineStripped( trigger.LastTriggerLineStripped )
	, Tick( trigger.Tick )
{
}

ZTrigger::~ZTrigger( void )
{
}

ZTrigger& ZTrigger::operator=( const ZTrigger& rhs )
{
	if ( this != &rhs )
	{
		this->Index = rhs.Index;
		this->Type = rhs.Type;
		this->Pattern = rhs.Pattern;
		this->Raw = rhs.Raw;
		this->Active = rhs.Active;
		this->AutoReply = rhs.AutoReply;
		this->ReplyMacro = rhs.ReplyMacro;
		this->LastTriggerLineRaw = rhs.LastTriggerLineRaw;
		this->LastTriggerLineStripped = rhs.LastTriggerLineStripped;
		this->Tick = rhs.Tick;
	}

	return *this;
}

bool ZTrigger::operator<( const ZTrigger& rhs )
{
	return this < &rhs;
}

void ZTrigger::stampIt( void )
{
	Tick = ::GetTickCount64();
}

bool ZTrigger::match( const std::string& raw, const std::string& stripped ) const
{
	return match( Raw ? raw : stripped );
}

bool ZTrigger::match( const std::string& text ) const
{
	bool matched = false;

	if ( text.length() > 0 )
	{
		switch ( Type )
		{
			case ZTrigger::NORMAL: matched = text.find( Pattern ) != std::string::npos; break;
			case ZTrigger::REGEX: matched = Regex.Match( text.c_str() ).IsMatched() != 0; break;
			default: break;
		}
	}

	return matched;
}

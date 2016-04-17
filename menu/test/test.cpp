/// @file test.cpp
/// @author RPW

#include "testharness.h"
#include "XmlReportPrinter.h"

#include "MenuParser.h"

#include <windows.h>
#include <fstream>
#include <sstream>
#include <string>

using namespace easyunit;

class MenuManager1 : public RPW::TW::IMenuManager
{
public:
	// IMenuManager Interface
	virtual void cd( void ) { oss << "{cd}"; }
	virtual void cd( std::string& folder ) { oss << "{cd '" << folder << "'}"; }
	virtual void ls( void ) { oss << "{ls}"; }
	virtual void ls( std::string& huh ) { oss << "{ls '" << huh << "'}"; }
	virtual void run( void ) { oss << "{run}"; }
	virtual void run( std::string& script ) { oss << "{run '" << script << "'}"; }
	virtual void kill( void ) { oss << "{kill}"; }
	virtual void kill( std::string& script ) { oss << "{kill '" << script << "'}"; }
	virtual void ban( void ) { oss << "{ban}"; }
	virtual void ban( std::string& ip ) { oss << "{ban '" << ip << "'}"; }
	virtual void unban( void ) { oss << "{unban}"; }
	virtual void unban( std::string& ip ) { oss << "{unban '" << ip << "'}"; }
	virtual void allow( void ) { oss << "{allow}"; }
	virtual void allow( std::string& ip ) { oss << "{allow '" << ip << "'}"; }
	virtual void quit( void ) { oss << "{quit}"; }

	virtual void accept( void ) { oss << "{accept}"; }
	virtual void error( std::string& huh ) { oss << "{error '" << huh << "'}"; }
	virtual void error( void ) { oss << "{error}"; }
	virtual void fail( void ) { oss << "{fail}"; }

	std::ostringstream oss;
};

TEST( MenuParser, cd )
{
	MenuManager1 m1;

	RPW::TW::MenuParser p;
	p.Inject( m1 );
	p.Initialize();
	p.Parse( "cd 123\rcd\r" );
	p.Parse( "cd 456\rc" );
	p.Parse( "d\rCD abc\rCD\r" );

	p.Cleanup();

	ASSERT_EQUALS_V(
	   "{cd '123'}{cd}{accept}"
	   "{cd '456'}{accept}"
	   "{cd}{cd 'abc'}{cd}{accept}"
	   "{accept}{accept}"
	   , m1.oss.str() );

	return;
}

TEST( MenuParser, ls )
{
	MenuManager1 m1;

	RPW::TW::MenuParser p;
	p.Inject( m1 );
	p.Initialize();
	p.Parse( "ls\r" );
	p.Parse( "ls 123\rl" );
	p.Parse( "s\rLS abc\rLS\r" );

	p.Cleanup();

	ASSERT_EQUALS_V(
	   "{ls}{accept}"
	   "{ls '123'}{accept}"
	   "{ls}{ls 'abc'}{ls}{accept}"
	   "{accept}{accept}"
	   , m1.oss.str() );

	return;
}

TEST( MenuParser, run )
{
	MenuManager1 m1;

	RPW::TW::MenuParser p;
	p.Inject( m1 );
	p.Initialize();
	p.Parse( "run\r" );
	p.Parse( "run 123\rr" );
	p.Parse( "un\rRUN abc\rRUN\r" );

	p.Cleanup();

	ASSERT_EQUALS_V(
	   "{run}{accept}"
	   "{run '123'}{accept}"
	   "{run}{run 'abc'}{run}{accept}"
	   "{accept}{accept}"
	   , m1.oss.str() );

	return;
}

TEST( MenuParser, kill )
{
	MenuManager1 m1;

	RPW::TW::MenuParser p;
	p.Inject( m1 );
	p.Initialize();
	p.Parse( "kill\r" );
	p.Parse( "kill 123\rki" );
	p.Parse( "ll\rKILL abc\rKILL\r" );

	p.Cleanup();

	ASSERT_EQUALS_V(
	   "{kill}{accept}"
	   "{kill '123'}{accept}"
	   "{kill}{kill 'abc'}{kill}{accept}"
	   "{accept}{accept}"
	   , m1.oss.str() );

	return;
}

TEST( MenuParser, ban )
{
	MenuManager1 m1;

	RPW::TW::MenuParser p;
	p.Inject( m1 );
	p.Initialize();
	p.Parse( "ban\r" );
	p.Parse( "ban 123\rba" );
	p.Parse( "n\rBAN abc\rBAN\r" );

	p.Cleanup();

	ASSERT_EQUALS_V(
	   "{ban}{accept}"
	   "{ban '123'}{accept}"
	   "{ban}{ban 'abc'}{ban}{accept}"
	   "{accept}{accept}"
	   , m1.oss.str() );

	return;
}

TEST( MenuParser, unban )
{
	MenuManager1 m1;

	RPW::TW::MenuParser p;
	p.Inject( m1 );
	p.Initialize();
	p.Parse( "unban\r" );
	p.Parse( "unban 123\run" );
	p.Parse( "ban\rUNBAN abc\rUNBAN\r" );

	p.Cleanup();

	ASSERT_EQUALS_V(
	   "{unban}{accept}"
	   "{unban '123'}{accept}"
	   "{unban}{unban 'abc'}{unban}{accept}"
	   "{accept}{accept}"
	   , m1.oss.str() );

	return;
}

TEST( MenuParser, allow )
{
	MenuManager1 m1;

	RPW::TW::MenuParser p;
	p.Inject( m1 );
	p.Initialize();
	p.Parse( "allow\r" );
	p.Parse( "allow 123\rall" );
	p.Parse( "ow\rALLOW abc\rALLOW\r" );

	p.Cleanup();

	ASSERT_EQUALS_V(
	   "{allow}{accept}"
	   "{allow '123'}{accept}"
	   "{allow}{allow 'abc'}{allow}{accept}"
	   "{accept}{accept}"
	   , m1.oss.str() );

	return;
}

TEST( MenuParser, quit )
{
	MenuManager1 m1;

	RPW::TW::MenuParser p;
	p.Inject( m1 );
	p.Initialize();
	p.Parse( "allow\r" );
	p.Parse( "\r" );
	p.Parse( "\r\r\r" );

	p.Cleanup();

	ASSERT_EQUALS_V(
	   "{allow}{accept}"
	   "{quit}{accept}"
	   "{quit}{quit}{quit}{accept}"
	   "{accept}{accept}"
	   , m1.oss.str() );

	return;
}

TEST( MenuParser, syntax_error )
{
	MenuManager1 m1;

	RPW::TW::MenuParser p;
	p.Inject( m1 );
	p.Initialize();
	p.Parse( "ls\rcd\rrun\rban\runban\rkill\rallow\r" );
	p.Parse( "holy cow\r\n" );
	p.Parse( "\r" );
	p.Parse( "\r\nbatman!\r" );

	p.Cleanup();

	ASSERT_EQUALS_V(
	   "{ls}{cd}{run}{ban}{unban}{kill}{allow}{accept}"
	   "{error 'holy cow'}{accept}"
	   "{quit}{accept}"
	   "{quit}{error 'batman!'}{accept}"
	   "{accept}{accept}"
	   , m1.oss.str() );

	return;
}

TEST( MenuParser, backspace1 )
{
	MenuManager1 m1;

	RPW::TW::MenuParser p;
	p.Inject( m1 );
	p.Initialize();
	p.Parse( "runs\x08\r" );

	p.Cleanup();

	ASSERT_EQUALS_V(
	   "{run}{accept}"
	   "{accept}{accept}"
	   , m1.oss.str() );

	return;
}

TEST( MenuParser, backspace2 )
{
	MenuManager1 m1;

	RPW::TW::MenuParser p;
	p.Inject( m1 );
	p.Initialize();
	p.Parse( "run this\r" );
	p.Parse( "run this\x08\x08\x08\x08that\r" );

	p.Cleanup();

	ASSERT_EQUALS_V(
	   "{run 'this'}{accept}"
	   "{run 'that'}{accept}"
	   "{accept}{accept}"
	   , m1.oss.str() );

	return;
}

TEST( MenuParser, backspace3 )
{
	MenuManager1 m1;

	RPW::TW::MenuParser p;
	p.Inject( m1 );
	p.Initialize();
	p.Parse( "run abc\r" );
	p.Parse( "run def\x08\x08\x08\x08\x08\x08\x08ls ghi\r" );
	p.Parse( "ls jkl mno\x08\x08\x08pqr\x08\x08\x08\x08\x08\x08\x08\x08\x08\x08kill stu\r" );

	p.Cleanup();

	ASSERT_EQUALS_V(
	   "{run 'abc'}{accept}"
	   "{ls 'ghi'}{accept}"
	   "{kill 'stu'}{accept}"
	   "{accept}{accept}"
	   , m1.oss.str() );

	return;
}

TEST( MenuParser, backspace4 )
{
	MenuManager1 m1;

	RPW::TW::MenuParser p;
	p.Inject( m1 );
	p.Initialize();
	p.Parse( "run abc\r" );
	p.Parse( "run def\x08\x08\x08\x08\x08\x08\x08\x08\x08\x08ls ghi\r" );
	p.Parse( "\x08\x08\x08\x08\x08\x08\x08\x08\x08\x08kill stu\r" );

	p.Cleanup();

	ASSERT_EQUALS_V(
	   "{run 'abc'}{accept}"
	   "{ls 'ghi'}{accept}"
	   "{kill 'stu'}{accept}"
	   "{accept}{accept}"
	   , m1.oss.str() );

	return;
}

TEST( MenuParser, backspace5 )
{
	MenuManager1 m1;

	RPW::TW::MenuParser p;
	p.Inject( m1 );
	p.Initialize();
	p.Parse( "run abc\r" );
	p.Parse( "run def\x08\x08\x08\x08\x08\x08\x08\x08\x08\x08\r" );
	p.Parse( "\x08\x08\x08\x08\x08\x08\x08\x08\x08\x08\r" );

	p.Cleanup();

	ASSERT_EQUALS_V(
	   "{run 'abc'}{accept}"
	   "{quit}{accept}"
	   "{quit}{accept}"
	   "{accept}{accept}"
	   , m1.oss.str() );

	return;
}

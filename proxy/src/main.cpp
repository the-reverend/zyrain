/// @file main.cpp
/// @author rev

#include "StdAfx.h"

#define NOGDI // avoid #define ERROR - conflicts with glog
#include <glog/logging.h>

#include "WsaData.h"
#include "EventHandle.h"
#include "CommandLineProcessor.h"
#include "Utilities.h"
#include "rpw.tw.proxy_manifest.h"

//#include "ThreadSafeSqliteDatabaseFactory.h"
#include "SqliteStore.h"
#include "LemonParser.h"

#include "TwProxy.h"

#include <string>
#include <iostream>
#include <stdexcept>

class CommandLineParser
{
public:
	CommandLineParser()
		: remoteIp( "192.168.2.199" )
		, database( ":memory:" )
		, journal( "DELETE" )
		, remotePort( 5023 )
		, localPort( 5023 )
		, synchronous( "OFF" )
		, keepAlive( 10 )
		, help( false )
	{
		parser.RegisterConsumer( *this );
		parser.RegisterCallback( "rip", &CommandLineParser::setRemoteIpAddress );
		parser.RegisterCallback( "rp", &CommandLineParser::setRemotePort );
		parser.RegisterCallback( "lp", &CommandLineParser::setLocalPort );
		parser.RegisterCallback( "db", &CommandLineParser::setDatabase );
		parser.RegisterCallback( "sync", &CommandLineParser::setSynchronous );
		parser.RegisterCallback( "journal", &CommandLineParser::setJournal );
		parser.RegisterCallback( "keepalive", &CommandLineParser::setKeepAlive );
		parser.RegisterCallback( "help", &CommandLineParser::setHelp );
		parser.RegisterCallback( "h", &CommandLineParser::setHelp );
		parser.RegisterCallback( "?", &CommandLineParser::setHelp );
	}

	void setRemoteIpAddress( const std::string& argument )
	{
		remoteIp = argument.c_str();
	}

	void setRemotePort( const std::string& argument )
	{
		unsigned short val;
		RPW::Core::Utilities::FromStringValue( argument, val, true );

		remotePort = val;
	}

	void setLocalPort( const std::string& argument )
	{
		unsigned short val;
		RPW::Core::Utilities::FromStringValue( argument, val, true );

		localPort = val;
	}

	void setDatabase( const std::string& argument )
	{
		database = argument;
	}

	void setSynchronous( const std::string& argument )
	{
		synchronous = argument;
	}

	void setJournal( const std::string& argument )
	{
		journal = argument;
	}

	void setHelp( const std::string& argument = "" )
	{
		help = true;
	}

	void setKeepAlive( const std::string& argument )
	{
		unsigned short val;
		RPW::Core::Utilities::FromStringValue( argument, val, true );

		keepAlive = val;
	}

	std::string remoteIp;
	std::string database;
	std::string journal;
	unsigned short remotePort;
	unsigned short localPort;
	std::string synchronous;
	unsigned short keepAlive;
	bool help;
	RPW::Core::CommandLineProcessor<CommandLineParser> parser;
};

int main( int argc, char* argv[] )
{
	try
	{
		CommandLineParser clp;
		clp.parser.Process( argc, argv, 1 );

		google::InitGoogleLogging( argv[0] );

		LOG( INFO ) << "starting up : " << argv[0];

		if ( 0 == clp.localPort
		      || 0 == clp.remotePort
		      || 0 == clp.remoteIp.length()
		      || clp.help )
		{
			std::string exe( argv[0] );
			std::cout << "== usage:" << std::endl;
			std::cout << "== " << exe.substr( exe.find_last_of( '\\' ) + 1 ) << " [params]" << std::endl;
			std::cout << "==  -rip [remoteip] -- default \"192.168.2.199\"" << std::endl;
			std::cout << "==  -rp [remoteport] -- default \"5023\"" << std::endl;
			std::cout << "==  -lp [localport] -- default \"5023\"" << std::endl;
			std::cout << "==  -db [database] -- default \":memory:\"" << std::endl;
			std::cout << "==  -sync [0|OFF|1|NORMAL|2|FULL] -- default \"OFF\"" << std::endl;
			std::cout << "==  -journal [DELETE|TRUNCATE|PERSIST|MEMORY|OFF] -- default \"DELETE\"" << std::endl;
			std::cout << "==  -keepalive [seconds] -- default \"10\"" << std::endl;
			throw std::runtime_error( "missing command line argument(s)" );
		}

		LOG( INFO ) << "localPort = " << clp.localPort;
		LOG( INFO ) << "remotePort = " << clp.remotePort;
		LOG( INFO ) << "remoteIp = " << clp.remoteIp;
		LOG( INFO ) << "database = " << clp.database;
		LOG( INFO ) << "sync = " << clp.synchronous;
		LOG( INFO ) << "journal = " << clp.journal;

#pragma region sqlite sanity check
		int sqlitever = sqlite3_libversion_number();
		std::string sqlitesourceid( sqlite3_sourceid() );
		std::string sqlitelibversion( sqlite3_libversion() );

		if ( sqlitever == SQLITE_VERSION_NUMBER )
			LOG( INFO ) << "sqlite3_libversion_number = " << sqlitever << " == " << SQLITE_VERSION_NUMBER;
		else
			LOG( ERROR ) << "sqlite3_libversion_number = " << sqlitever << " != " << SQLITE_VERSION_NUMBER;

		if ( sqlitesourceid == std::string( SQLITE_SOURCE_ID ) )
			LOG( INFO ) << "sqlite3_sourceid = " << sqlitesourceid << " == " << SQLITE_SOURCE_ID;
		else
			LOG( ERROR ) << "sqlite3_sourceid = " << sqlitesourceid << " != " << SQLITE_SOURCE_ID;

		if ( sqlitelibversion == std::string( SQLITE_VERSION ) )
			LOG( INFO ) << "sqlite3_libversion = " << sqlitelibversion << " == " << SQLITE_VERSION;
		else
			LOG( ERROR ) << "sqlite3_libversion = " << sqlitelibversion << " != " << SQLITE_VERSION;
#pragma endregion

		RPW::Core::EventHandle hExit( 0, TRUE, 0, 0 );
		RPW::Core::WsaData wsa;

//		ThreadSafeSqliteDatabaseFactory::init( clp.database.c_str() );
		RPW::TW::SqliteStore store;
		store.open( clp.database.c_str(), VERSION_1_CREATION_SQL );

		if ( clp.synchronous.length() > 0 )
		{
			std::ostringstream oss;
			oss << "PRAGMA synchronous=" << clp.synchronous;
			store.db().getPreparedStatement( oss.str() ).reset().step_all().finalize();
		}

		if ( clp.journal.length() > 0 )
		{
			std::ostringstream oss;
			oss << "PRAGMA journal_mode=" << clp.journal;
			store.db().getPreparedStatement( oss.str() ).reset().step_all().finalize();
		}

		int age = 0;
		std::ostringstream oss;
		oss << "select julianday('now') - julianday('" << RPW_TW_MANIFEST_DATE << "')";
		store.db().getPreparedStatement( oss.str() )
			.reset().step().get_column_int( 0, age ).finalize();

		if ( age <= 90 )
		{
			TwProxy proxy( RPW::Core::IpEndpoint( clp.remoteIp.c_str(), clp.remotePort )
			               , store
			               , hExit
			               , '~'
			               , clp.keepAlive );

			RPW::Core::IpEndpoint( INADDR_ANY, clp.localPort );
			try
			{
				// setup the proxy listener
				if ( proxy.create( AF_INET, SOCK_STREAM, IPPROTO_TCP )
				      && proxy.bind( RPW::Core::IpEndpoint( INADDR_ANY, clp.localPort ) )
				      && proxy.listen() )
				{
					proxy.Start();
				}
				else
				{
					// fail if the listener socket isn't working
					proxy.destroy();
					hExit.set();
					LOG( ERROR ) << "failed to create, bind, or listen on port " << clp.localPort;
				}
			}
			catch ( const RPW::Core::SocketException& e )
			{
				proxy.destroy();
				hExit.set();
				LOG( ERROR ) << "failed to create, bind, or listen on port " << clp.localPort;
				LOG( ERROR ) << e.formatErrorMessage( proxy );
				LOG( ERROR ) << e.what();
			}

			hExit.waitFor();

			proxy.destroy();
			proxy.Stop();
			proxy.Rhs().destroy();
			proxy.Rhs().Stop();
			proxy.LhsCloseAll();
			proxy.LhsStopAll();
		}

		store.close();
	}
	catch ( const std::exception& e )
	{
		LOG( ERROR ) << e.what();
		return 1;
	}
	catch ( ... )
	{
		LOG( ERROR ) << "unknown system exception";
		return 2;
	}

	return 0;
}


/// @file TwData.cpp
/// @author RPW

#include "StdAfx.h"
#include "SqliteStore.h"

#include "Utilities.h"
#include "MD5.h"

#include <assert.h>
#include <iostream>

RPW::TW::SqliteStore::SqliteStore()
	: m_iSchemaVersion( 0 )
	, m_iWhosOnlineStamp()
	, m_bInCimMode( false )
{
	assert( sqlite3_libversion_number() == SQLITE_VERSION_NUMBER );
	assert( strcmp( sqlite3_sourceid(), SQLITE_SOURCE_ID ) == 0 );
	assert( strcmp( sqlite3_libversion(), SQLITE_VERSION ) == 0 );
}

RPW::TW::SqliteStore::~SqliteStore( void )
{
	close();
}

void RPW::TW::SqliteStore::open( const char* const dbName, const char* const creationSql )
{
	close();

	std::string emptyHash;
	std::string referenceHash;

	// begin temporary scope to calculate what the schema hash should be
	{
		RPW::SQLite::SqliteDatabase db;
		int rc = db.open( ":memory:" );

		if ( SQLITE_OK == rc )
		{
			emptyHash = getSchemaHash( db );
			if ( NULL != creationSql )
			{
				runDelimitedSql( db, creationSql );
				referenceHash = getSchemaHash( db );
			}
			else
			{
				referenceHash = emptyHash;
			}
		}
		else
		{
			throw RPW::SQLite::SqliteException( rc, db.errmsg() );
		}

		if ( emptyHash == referenceHash )
		{
			throw std::runtime_error( "unexpected schema hash" );
		}
	}

	// open the database on disk
	int rc = m_db.open( dbName );

	if ( SQLITE_OK == rc )
	{
		// get the current schema hash
		std::string hash = getSchemaHash();

		// if the hash matches the empty hash, the database is empty and needs a schema
		if ( emptyHash == hash )
		{
			// create the schema
			runDelimitedSql( creationSql );
			// recalculate the schema hash
			hash = getSchemaHash();
		}

		// if the hash matches the reference hash, then the existing schema is compatible
		if ( referenceHash == hash )
		{
			m_iSchemaVersion = CURRENT_SCHEMA_VERSION;
		}
		else
		{
			std::ostringstream oss;
			oss << "invalid schema hash " << hash;
			throw std::runtime_error( oss.str() );
		}
	}
	else
	{
		throw RPW::SQLite::SqliteException( rc, m_db.errmsg() );
	}

	if ( 0 == m_iSchemaVersion )
	{
		throw std::range_error( "invalid schema version" );
	}
}

void RPW::TW::SqliteStore::close()
{
	m_iSchemaVersion = 0;
	m_db.close();
}

void RPW::TW::SqliteStore::runDelimitedSql( const std::string& sql )
{
	/// @todo use m_db.executeStatements( sql );
	runDelimitedSql( m_db, sql );
}

std::vector<std::string> RPW::TW::SqliteStore::existingTables()
{
	//CREATE TABLE sqlite_master (
	//  type TEXT,
	//  name TEXT,
	//  tbl_name TEXT,
	//  rootpage INTEGER,
	//  sql TEXT
	//);

	RPW::SQLite::SqliteStatement* t = &m_db.getPreparedStatement( SQL_SELECT_TABLE_NAMES );

	int cols = t->enter().reset().get_column_count();

	std::vector<std::string> tables;
	bool done = false;
	while ( !done )
	{
		t->step( done );

		if ( !done )
		{
			for ( int i = 0; i < cols; i++ )
			{
				std::string text;
				t->get_column_text( i, text );
				tables.push_back( text );
			}
		}
	}

	t->leave();

	return tables;
}

std::string RPW::TW::SqliteStore::getSchema()
{
	return getSchema( m_db );
}

std::string RPW::TW::SqliteStore::getSchemaHash()
{
	return getSchemaHash( m_db );
}

std::vector<std::string> RPW::TW::SqliteStore::getSingleColumnResults( RPW::SQLite::SqliteStatement& t )
{
	std::vector<std::string> vec;

	bool done = false;
	while ( !done )
	{
		t.step( done );

		if ( !done )
		{
			std::string text;
			t.get_column_text( 0, text );
			vec.push_back( t.get_column_text( 0 ) );
		}
	}

	return vec;
}

unsigned long RPW::TW::SqliteStore::getInfoCount( const std::string& key )
{
	RPW::SQLite::SqliteStatement* t = &m_db.getPreparedStatement( SQL_COUNT_INFO_BY_KEY );
	t->reset().bind_text( 1, key );

	std::vector<std::string> vec = getSingleColumnResults( *t );

	unsigned long count = 0;
	if ( vec.size() == 1 )
	{
		RPW::Core::Utilities::FromStringValue( vec[0], count );
	}
	else if ( vec.size() > 1 )
	{
		throw std::runtime_error( "unexpected result" );
	}

	return count;
}

void RPW::TW::SqliteStore::updateInfo( const std::string& key, unsigned long value )
{
	m_db.getPreparedStatement( SQL_REPLACE_INFO )
	.reset().bind_text( 1, key ).bind_int64( 2, value ).step_all();
}

void RPW::TW::SqliteStore::updateInfo( const std::string& key, int value )
{
	m_db.getPreparedStatement( SQL_REPLACE_INFO )
	.reset().bind_text( 1, key ).bind_int( 2, value ).step_all();
}

void RPW::TW::SqliteStore::updateInfo( const std::string& key, const std::string& value )
{
	m_db.getPreparedStatement( SQL_REPLACE_INFO )
	.reset().bind_text( 1, key ).bind_text( 2, value ).step_all();
}

int RPW::TW::SqliteStore::getInfoInt( const std::string& key )
{
	return m_db.getPreparedStatement( SQL_SELECT_INFO_VALUE_BY_KEY ).reset()
	       .bind_text( 1, key.c_str(), static_cast<int>( key.length() ) ).step().get_column_int( 0 );
}

std::string RPW::TW::SqliteStore::getInfoString( const std::string& key )
{
	return m_db.getPreparedStatement( SQL_SELECT_INFO_VALUE_BY_KEY ).reset()
	       .bind_text( 1, key.c_str(), static_cast<int>( key.length() ) ).step().get_column_text( 0 );
}

unsigned long RPW::TW::SqliteStore::getEntityInfoCount( unsigned long eId, const std::string& key )
{
	RPW::SQLite::SqliteStatement* t = &m_db.getPreparedStatement( SQL_COUNT_INFO_BY_KEY_AND_ACTOR );
	t->reset().bind_text( 1, key ).bind_int( 2, eId );

	std::vector<std::string> vec = getSingleColumnResults( *t );

	unsigned long count = 0;
	if ( vec.size() == 1 )
	{
		RPW::Core::Utilities::FromStringValue( vec[0], count );
	}
	else if ( vec.size() > 1 )
	{
		throw std::runtime_error( "unexpected result" );
	}

	return count;
}

void RPW::TW::SqliteStore::updateEntityInfo( unsigned long eId, const std::string& key, unsigned long value )
{
	if ( getEntityInfoCount( eId, key ) == 1 )
	{
		m_db.getPreparedStatement( SQL_UPDATE_INFO_2 )
		.reset().bind_int64( 1, value ).bind_text( 2, key ).bind_int( 3, eId ).step_all();
	}
	else
	{
		m_db.getPreparedStatement( SQL_INSERT_INFO )
		.reset().bind_int( 1, eId ).bind_text( 2, key ).bind_int64( 3, value ).step_all();
	}
}

/// @brief get the UTC time with milliseconds from the sqlite3 engine
int RPW::TW::SqliteStore::getCurrentDateTimeStamp()
{
	return m_db.getPreparedStatement( SQL_SELECT_TIME ).reset()
	       .step().get_column_int( 0 );
}

void RPW::TW::SqliteStore::runDelimitedSql( RPW::SQLite::SqliteDatabase& db, const std::string& sql )
{
	std::istringstream iss( sql );
	std::string field;
	RPW::SQLite::SqliteStatement t( db );
	while ( std::getline( iss, field, ';' ) )
	{
		if ( field.length() > 0 )
		{
			try
			{
				t.prepare( field.c_str() ).step_all();
			}
			catch ( const RPW::SQLite::SqliteException& e )
			{
				std::cout << e.getNumber() << " " << field << std::endl;
				if ( e.getNumber() != SQLITE_MISUSE )
				{
					throw;
				}
			}
		}
	}
}

std::string RPW::TW::SqliteStore::getSchema( RPW::SQLite::SqliteDatabase& db )
{
	//CREATE TABLE sqlite_master (
	//  type TEXT,
	//  name TEXT,
	//  tbl_name TEXT,
	//  rootpage INTEGER,
	//  sql TEXT
	//);

	std::string schema;
	db.getPreparedStatement( SQL_SELECT_SCHEMA )
	.enter().reset().step().get_column_text( 0, schema ).leave();

	return schema;
}

std::string RPW::TW::SqliteStore::getSchemaHash( RPW::SQLite::SqliteDatabase& db )
{
	RPW::Core::MD5 md5;
	return md5.getHashFromString( getSchema( db ) );
}

#pragma once

#include "IDataActor.h"

#include "SqliteDatabase.h"
#include "SqliteStatement.h"
#include "SqliteException.h"

#include <vector>
#include <sstream>
#include <string>
#include <algorithm>

#include "statements.h"

#define EMPTY_SCHEMA_HASH "d41d8cd98f00b204e9800998ecf8427e"
#define CURRENT_SCHEMA_VERSION 1
#include "VERSION_1_CREATION_SQL.h"
//#define VERSION_1_CREATION_SQL \
//"PRAGMA legacy_file_format=false;" \
//"BEGIN TRANSACTION;" \
//"CREATE TABLE info(info_id integer primary key, infoKey, infoValue, infoStamp integer DEFAULT (cast(strftime('%s', 'now') as integer)), guid_id DEFAULT 0, UNIQUE(infoKey, guid_id));" \
//"INSERT INTO info (infoKey, infoValue) VALUES('version', 1);" \
//"CREATE TABLE entities(entity_id integer primary key, entityName, entityType_id, corpNum, entityStamp integer DEFAULT (cast(strftime('%s', 'now') as integer)), guid_id DEFAULT 0, UNIQUE(entityName, corpNum, entityType_id, guid_id));" \
//"CREATE TABLE entityInfo(entityInfo_id integer primary key, entity_id, entityInfoKey, entityInfoValue, entityInfoStamp integer DEFAULT (cast(strftime('%s', 'now') as integer)), guid_id DEFAULT 0, UNIQUE(entity_id, entityInfoKey, guid_id));" \
//"CREATE TABLE entityTypes(entityType_id integer primary key, entityTypeName, entityTypeStamp integer DEFAULT (cast(strftime('%s', 'now') as integer)), guid_id DEFAULT 0, UNIQUE(entityTypeName, guid_id));" \
//"INSERT INTO entityTypes (entityType_id, entityTypeName) VALUES(0, 'unknown');" \
//"INSERT INTO entityTypes (entityType_id, entityTypeName) VALUES(1, 'ambassador');" \
//"INSERT INTO entityTypes (entityType_id, entityTypeName) VALUES(2, 'player');" \
//"INSERT INTO entityTypes (entityType_id, entityTypeName) VALUES(3, 'corporation');" \
//"CREATE TABLE figHits(figHit_id integer primary key, sector, entity_id, figHitStamp integer DEFAULT (cast(strftime('%s', 'now') as integer)), guid_id DEFAULT 0, UNIQUE(sector, entity_id, guid_id));" \
//"CREATE TABLE figs(fig_id integer primary key, sector, amount, entity_id, figStamp integer DEFAULT (cast(strftime('%s', 'now') as integer)), guid_id DEFAULT 0, UNIQUE(sector, guid_id));" \
//"CREATE TABLE guids(guid_id integer primary key, guid, UNIQUE(guid));" \
//"INSERT INTO guids(guid_id, guid) VALUES(0, 0);" \
//"CREATE TABLE haggle(haggle_id integer primary key, port_id, product, exp, tradeType, iteration, offer, counter, psyprobe, haggleStamp integer DEFAULT (cast(strftime('%s', 'now') as integer)), guid_id DEFAULT 0, UNIQUE(port_id, product, haggleStamp, guid_id));" \
//"CREATE TABLE limpets(limpet_id integer primary key, sector, amt, entity_id, limpetStamp integer DEFAULT (cast(strftime('%s', 'now') as integer)), guid_id DEFAULT 0, UNIQUE(sector, guid_id));" \
//"CREATE TABLE mines(mine_id integer primary key, sector, amt, entity_id, mineStamp integer DEFAULT (cast(strftime('%s', 'now') as integer)), guid_id DEFAULT 0, UNIQUE(sector, guid_id));" \
//"CREATE TABLE portTypes(portType_id integer primary key, portTypeName, guid_id DEFAULT 0, UNIQUE(portTypeName, guid_id));" \
//"INSERT INTO portTypes(portType_id, portTypeName) VALUES(0, 'Special');" \
//"INSERT INTO portTypes(portType_id, portTypeName) VALUES(1, 'BBS');" \
//"INSERT INTO portTypes(portType_id, portTypeName) VALUES(2, 'BSB');" \
//"INSERT INTO portTypes(portType_id, portTypeName) VALUES(3, 'SBB');" \
//"INSERT INTO portTypes(portType_id, portTypeName) VALUES(4, 'SSB');" \
//"INSERT INTO portTypes(portType_id, portTypeName) VALUES(5, 'SBS');" \
//"INSERT INTO portTypes(portType_id, portTypeName) VALUES(6, 'BSS');" \
//"INSERT INTO portTypes(portType_id, portTypeName) VALUES(7, 'SSS');" \
//"INSERT INTO portTypes(portType_id, portTypeName) VALUES(8, 'BBB');" \
//"INSERT INTO portTypes(portType_id, portTypeName) VALUES(9, 'Stardock');" \
//"CREATE TABLE ports(port_id integer primary key, sector, portName, portType_id, ore, oreMax, oreMcic, org, orgMax, orgMcic, equ, equMax, equMcic, days, entity_id, portStamp integer DEFAULT (cast(strftime('%s', 'now') as integer)), guid_id DEFAULT 0, UNIQUE(sector, portName, portType_id, guid_id));" \
//"CREATE TABLE sectors(sector_id integer primary key, num, port_id, beacon, space, sectorStamp integer DEFAULT (cast(strftime('%s', 'now') as integer)), guid_id DEFAULT 0, UNIQUE (num, guid_id));" \
//"CREATE TABLE warps(warp_id integer primary key, src, dst, warpStamp integer DEFAULT (cast(strftime('%s', 'now') as integer)), guid_id DEFAULT 0, UNIQUE(src, dst, guid_id));" \
//"CREATE TABLE whosOnline(whosOnline_id integer primary key, entity_id, isOnline, whosOnlineStamp1 integer, whosOnlineStamp2 integer, guid_id DEFAULT 0);" \
//"COMMIT;"

namespace RPW
{
	namespace TW
	{

		class SqliteStore : public RPW::TW::IDataActor
		{
		public:
			SqliteStore();
			virtual ~SqliteStore();

			void open( const char* const dbName, const char* const creationSql = NULL );
			void close();

			void runDelimitedSql( const std::string& sql );
			std::vector<std::string> getSingleColumnResults( RPW::SQLite::SqliteStatement& t );
			std::vector<std::string> existingTables();
			std::string getSchema();
			std::string getSchemaHash();

			unsigned long getInfoCount( const std::string& key );
			void updateInfo( const std::string& key, unsigned long value );
			void updateInfo( const std::string& key, int value );
			void updateInfo( const std::string& key, const std::string& value );

			int getInfoInt( const std::string& key );
			std::string getInfoString( const std::string& key );

			unsigned long getEntityInfoCount( unsigned long eId, const std::string& key );
			void updateEntityInfo( unsigned long eId, const std::string& key, unsigned long value );

			int getCurrentDateTimeStamp();

			unsigned long getNumPlayersOnline()
			{
				int c = 0;
				m_db.getPreparedStatement( SQL_COUNT_ONLINE_BY_TYPE )
				.enter().reset().bind_text( 0, "player" ).step().get_column_int( 0, c ).leave();
				return c;
			}

			unsigned long getNumCorpsOnline()
			{
				int c = 0;
				m_db.getPreparedStatement( SQL_COUNT_ONLINE_BY_TYPE )
				.enter().reset().step().bind_text( 0, "corporation" ).get_column_int( 0, c ).leave();
				return c;
			}

			RPW::SQLite::SqliteDatabase& db() { return m_db; }

		protected:
			static void runDelimitedSql( RPW::SQLite::SqliteDatabase& db, const std::string& sql );
			static std::string getSchema( RPW::SQLite::SqliteDatabase& db );
			static std::string getSchemaHash( RPW::SQLite::SqliteDatabase& db );

			RPW::SQLite::SqliteDatabase m_db;
			int m_iSchemaVersion;
			int m_iWhosOnlineStamp;
			bool m_bInCimMode;

		public: // IDataActor interface
			virtual void HandlePlayTradewarsMenu() { }
			virtual void HandleShowTodaysLog() { }
			virtual void HandleHelloName( const std::string& name ) { }
			virtual void HandleVersion( unsigned long major, unsigned long minor ) { }
			virtual void HandleMbbs() { }
			virtual void HandleGold() { }
			virtual void HandleRegisteredTo( const std::string& name )
			{
				updateInfo( "registered to", name );
			}

			virtual void HandleCourtesyOf( const std::string& name )
			{
				updateInfo( "courtesy of", name );
			}

			virtual void HandleNewCharacter() { }
			virtual void HandleEnterPassword() { }
			virtual void HandleRepeatPassword() { }
			virtual void HandleBeginWhosOnline()
			{
				// store the current datetime stamp for future use
				m_iWhosOnlineStamp = getCurrentDateTimeStamp();
			}

			void HandleWhosOnline( int eId )
			{
				// check if we think the player/corp is already online
				//   if true then update the end stamp on the latest record
				//   else add a new record showing him online

				// get the number of matching rows in the whosOnline table
				int rows = 0;
				m_db.getPreparedStatement( SQL_COUNT_ONLINE_BY_ACTOR )
				.enter().reset().bind_int( 1, eId ).step().get_column_int( 0, rows ).leave();

				if ( rows == 0 ) // no rows found in the whosOnline table
				{
					// insert a row showing this player online
					m_db.getPreparedStatement( SQL_INSERT_ONLINE_ENTRY )
					.enter().reset().bind_int( 1, eId ).bind_int( 2, m_iWhosOnlineStamp ).step_all().leave();
				}
				else
				{
					// find out if we think this player is online already
					int online = 0;
					int online_id = 0;
					m_db.getPreparedStatement( SQL_SELECT_ONLINE_BY_ID )
					.enter().reset().bind_int( 1, eId ).step().get_column_int( 0, online ).get_column_int( 1, online_id ).leave();

					if ( online == 0 )
					{
						// according to our data, we think this player is offline, so insert a row showing this player online
						m_db.getPreparedStatement( SQL_INSERT_ONLINE_ENTRY )
						.enter().reset().bind_int( 1, eId ).bind_int( 2, m_iWhosOnlineStamp ).step_all().leave();
					}
					else
					{
						// this player is already online, so update the stamp2
						m_db.getPreparedStatement( SQL_UPDATE_ONLINE_STAMP_BY_ID )
						.enter().reset().bind_int( 1, m_iWhosOnlineStamp ).bind_int( 2, online_id ).step_all().leave();
					}
				}
			}

			virtual void HandleWhosOnline( long rank, const std::string& name, unsigned long corp )
			{
				// step 1 entity discovery for player or ambassador
				int c1 = 0;
				m_db.getPreparedStatement( SQL_COUNT_ACTORS_BY_NAME )
				.enter().reset().bind_text( 1, name ).bind_text( 2, rank == 23 ? "ambassador" : "player" ).step().get_column_int( 0, c1 ).leave();

				if ( c1 == 0 ) // no matching entities were found
				{
					// insert a new entity
					m_db.getPreparedStatement( SQL_INSERT_ACTOR )
					.enter().reset().bind_text( 1, name ).bind_text( 2, rank == 23 ? "ambassador" : "player" ).bind_int( 3, corp ).bind_int( 4, m_iWhosOnlineStamp ).step_all().leave();
				}

				// step 2 update online status for the player or ambassador
				int eId = 0;
				m_db.getPreparedStatement( SQL_SELECT_ACTOR_BY_NAME_AND_TYPE )
				.enter().reset().bind_text( 1, name ).bind_text( 2, rank == 23 ? "ambassador" : "player" ).step().get_column_int( 0, eId ).leave();
				HandleWhosOnline( eId );

				// step 5 update player entity info with corp, rank, and exp/aln if necessary

				// update the entity info 'rank' entry
				int c2 = 0;
				m_db.getPreparedStatement( SQL_COUNT_INFO_BY_KEY_AND_ACTOR )
				.enter().reset().bind_text( 1, "rank" ).bind_int( 2, eId ).step().get_column_int( 0, c2 ).leave();

				if ( c2 == 0 )
				{
					m_db.getPreparedStatement( SQL_INSERT_INFO )
					.enter().reset().bind_int( 1, eId ).bind_text( 2, "rank" ).bind_int( 3, rank ).step_all().leave();
				}
				else
				{
					m_db.getPreparedStatement( SQL_UPDATE_INFO_1 )
					.enter().reset().bind_int( 1, rank ).bind_int( 2, m_iWhosOnlineStamp ).bind_int( 3, eId ).step_all().leave();
				}

				if ( 0 != corp )
				{
					// step 3 entity discovery for the corporation
					int c2 = 0;
					m_db.getPreparedStatement( SQL_COUNT_CORPS_BY_NUM )
					.enter().reset().bind_int( 1, corp ).step().get_column_int( 0, c2 ).leave();

					if ( c2 == 0 ) // no matching entities were found
					{
						// insert a new entity
						m_db.getPreparedStatement( SQL_INSERT_ACTOR )
						.enter().reset().bind_text( 1, "" ).bind_text( 2, "corporation" ).bind_int( 3, corp ).bind_int( 4, m_iWhosOnlineStamp ).step_all().leave();
					}

					// step 4 update online status for the corp
					eId = 0;
					m_db.getPreparedStatement( SQL_SELECT_CORP_BY_CORP_NUM )
					.enter().reset().bind_int( 1, corp ).step().get_column_int( 0, eId ).leave();
					HandleWhosOnline( eId );
				}
			}

			virtual void HandleEndWhosOnline()
			{
				// foreach entity
				//   if isOnline==1 && end stamp is old
				//     then add a new record showing him offline

				bool done = false;

				// get a list of rowids for the last known online state of every entity
				// and filter out the rows that were updated with the latest datetime in stamp2.
				// this should be a list of entities that are offline now
				RPW::SQLite::SqliteStatement* offlineList = &m_db.getPreparedStatement( SQL_SELECT_OFFLINE_ACTORS );
				offlineList->enter().reset().bind_int( 1, m_iWhosOnlineStamp ).step( done );

				// iterate through the results
				while ( !done )
				{
					int online_id = offlineList->get_column_int( 0 );
					int eId = offlineList->get_column_int( 1 );
					int online = offlineList->get_column_int( 2 );

					if ( online == 1 )
					{
						// player was marked as online but he is offline now, so insert an offline entry
						m_db.getPreparedStatement( SQL_INSERT_ONLINE_ENTRY )
						.enter().reset().bind_int( 1, eId ).bind_int( 2, m_iWhosOnlineStamp ).step_all().leave();
					}
					else
					{
						// player was already marked offline, so update the stamp2 field for that row
						m_db.getPreparedStatement( SQL_UPDATE_ONLINE_STAMP_BY_ID )
						.enter().reset().bind_int( 1, m_iWhosOnlineStamp ).bind_int( 2, online_id ).step_all().leave();
					}

					offlineList->step( done );
				}

				offlineList->leave();
			}

			virtual void HandleCommandPrompt() { }
			virtual void HandleComputerPrompt() { }
			virtual void HandleCorporatePrompt() { }
			virtual void UpdateTimeRemain( unsigned long seconds ) { }
			virtual void BeginQuickStats( unsigned long sector )
			{
				m_db.beginTransaction();
				UpdateCurrentSector( sector );
			}

			virtual void UpdateCurrentSector( unsigned long sector )
			{
				updateInfo( "sector", sector );
			}

			virtual void HandleCommandChar( char letter ) { }
			virtual void HandleComputerChar( char letter ) { }
			virtual void HandleCorporateChar( char letter ) { }
			virtual void HandleCommandDigits( unsigned long sector ) { }
			virtual void UpdateCurrentTurns( unsigned long turns )
			{
				updateInfo( "turns", turns );
			}

			virtual void UpdateCurrentCredits( unsigned long credits )
			{
				updateInfo( "credits", credits );
			}

			virtual void UpdateCurrentFighters( unsigned long fighters )
			{
				updateInfo( "fighters", fighters );
			}

			virtual void UpdateCurrentShields( unsigned long shields )
			{
				updateInfo( "shields", shields );
			}

			virtual void UpdateCurrentHolds( unsigned long holds )
			{
				updateInfo( "holds", holds );
			}

			virtual void UpdateCurrentFuelOre( unsigned long fuelOre )
			{
				updateInfo( "fuel ore", fuelOre );
			}

			virtual void UpdateCurrentOrganics( unsigned long organics )
			{
				updateInfo( "organics", organics );
			}

			virtual void UpdateCurrentEquipment( unsigned long equipment )
			{
				updateInfo( "equipment", equipment );
			}

			virtual void UpdateCurrentColonists( unsigned long colonists )
			{
				updateInfo( "colonists", colonists );
			}

			virtual void UpdateCurrentPhotonTorpedos( unsigned long photonTorpedos )
			{
				updateInfo( "photon torpedos", photonTorpedos );
			}

			virtual void UpdateCurrentArmidMines( unsigned long armidMines )
			{
				updateInfo( "armid mines", armidMines );
			}

			virtual void UpdateCurrentLimpetMines( unsigned long limpetMines )
			{
				updateInfo( "limpet mines", limpetMines );
			}

			virtual void UpdateCurrentGenesisTorpedos( unsigned long genesisTorpedos )
			{
				updateInfo( "genesis torpedos", genesisTorpedos );
			}

			virtual void UpdateCurrentTranswarpDrive( unsigned long transwarpDrive )
			{
				updateInfo( "transwarp drive", transwarpDrive );
			}

			virtual void UpdateCurrentCloakingDevices( unsigned long cloakingDevices )
			{
				updateInfo( "cloaking devices", cloakingDevices );
			}

			virtual void UpdateCurrentBeacons( unsigned long beacons )
			{
				updateInfo( "beacons", beacons );
			}

			virtual void UpdateCurrentAtomicDetonators( unsigned long atomicDetonators )
			{
				updateInfo( "atomic detonators", atomicDetonators );
			}

			virtual void UpdateCurrentCorbomiteDevices( unsigned long corbomiteDevices )
			{
				updateInfo( "corbomite devices", corbomiteDevices );
			}

			virtual void UpdateCurrentEtherProbes( unsigned long etherProbes )
			{
				updateInfo( "ether probes", etherProbes );
			}

			virtual void UpdateCurrentMineDisrupters( unsigned long mineDisrupters )
			{
				updateInfo( "mine disrupters", mineDisrupters );
			}

			virtual void UpdateCurrentPsychicProbe( unsigned long psychicProbe )
			{
				updateInfo( "psychic probe", psychicProbe );
			}

			virtual void UpdateCurrentPlanetScanner( unsigned long planetScanner )
			{
				updateInfo( "planet scanner", planetScanner );
			}

			virtual void UpdateCurrentLongRangeScanner( unsigned long longRangeScanner )
			{
				updateInfo( "long range scanner", longRangeScanner );
			}

			virtual void UpdateCurrentAlignment( long alignment )
			{
				updateInfo( "alignment", alignment );
			}

			virtual void UpdateCurrentExperience( unsigned long experience )
			{
				updateInfo( "experience", experience );
			}

			virtual void UpdateCurrentCorporationNumber( unsigned long corporationNumber )
			{
				updateInfo( "corporation number", corporationNumber );
			}

			virtual void UpdateCurrentShipNumber( unsigned long shipNumber )
			{
				updateInfo( "ship number", shipNumber );
			}

			virtual void UpdateCurrentShipBriefName( const std::string& shipBriefName )
			{
				updateInfo( "ship brief name", shipBriefName );
			}

			virtual void EndQuickStats( const std::string& shipBriefName )
			{
				UpdateCurrentShipBriefName( shipBriefName );
				m_db.commit();
			}

			virtual void HandleEnterCim()
			{
				if ( !m_bInCimMode )
				{
					m_db.beginTransaction();
					m_bInCimMode = true;
				}
			}

			virtual void HandleExitCim()
			{
				if ( m_bInCimMode )
				{
					m_db.commit();
					m_bInCimMode = false;
				}
			}

			virtual void HandleCoursePlot( std::vector<unsigned long>& course )
			{
				unsigned long fr = 0;
				for ( std::vector<unsigned long>::iterator it = course.begin(); it != course.end(); ++it )
				{
					if ( it == course.begin() )
					{
						fr = *it;
					}
					else
					{
						unsigned long to = *it;

						try
						{
							m_db.getPreparedStatement( SQL_INSERT_NEW_WARP )
							.enter().forgiveErrorCode( SQLITE_CONSTRAINT ).reset().bind_int( 1, fr ).bind_int( 2, to ).step_all().leave();
						}
						catch ( RPW::SQLite::SqliteException e )
						{
							if ( e.getNumber() != SQLITE_CONSTRAINT )
							{
								throw e;
							}
						}
						fr = to;
					}
				}
			}

			virtual void HandleStartWarpCim() { }
			virtual void HandleWarpCimEntry( std::vector<unsigned long>& warpSpec )
			{
				unsigned long fr = 0;
				for ( std::vector<unsigned long>::iterator it = warpSpec.begin(); it != warpSpec.end(); ++it )
				{
					if ( it == warpSpec.begin() )
					{
						fr = *it;
					}
					else
					{
						unsigned long to = *it;

						try
						{
							m_db.getPreparedStatement( SQL_INSERT_NEW_WARP )
							.enter().forgiveErrorCode( SQLITE_CONSTRAINT ).reset().bind_int( 1, fr ).bind_int( 2, to ).step_all().leave();
						}
						catch ( RPW::SQLite::SqliteException e )
						{
							if ( e.getNumber() != SQLITE_CONSTRAINT )
							{
								throw e;
							}
						}
					}
				}
			}

			virtual void HandleStartPortCim() { }
			virtual void HandlePortCimEntry( unsigned long sector, long fuelOreQuan, unsigned long fuelOrePerc, long organicsQuan, unsigned long organicsPerc, long equipmentQuan, unsigned long equipmentPerc ) { }
			virtual void HandleBeginFigList() { }
			virtual void HandleEndFigList() { }
			virtual void HandleAbortFigList() { }
			virtual void RecordSectorFigs( unsigned long sector, unsigned long count, unsigned int ownership, unsigned int type ) { }

		};

	}
}

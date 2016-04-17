/// @file test.cpp
/// @author RPW

#include "testharness.h"
#include "XmlReportPrinter.h"

#include "SqliteStore.h"
#include "MD5.h"
#include "LemonParser.h"

#include <windows.h>
#include <fstream>
#include <sstream>
#include <string>

using namespace easyunit;

TEST( MD5, GetHashFromString )
{
	RPW::Core::MD5 md5;
	std::string hash = md5.getHashFromString( "hello" );

	ASSERT_EQUALS_V( "5d41402abc4b2a76b9719d911017c592", hash );
}

TEST( TwData, SchemaHash )
{
	RPW::TW::SqliteStore d;
	d.open( ":memory:", VERSION_1_CREATION_SQL );
	std::string hash = d.getSchemaHash();

	int ver = d.getInfoInt( "version" );
	ASSERT_EQUALS_V( 1, ver );
}

TEST( TwData, QuickStats )
{
	RPW::TW::SqliteStore d;
	d.open( ":memory:", VERSION_1_CREATION_SQL );

	d.UpdateCurrentSector( 1 );
	int sector = d.getInfoInt( "sector" );
	ASSERT_EQUALS_V( 1, sector );

	d.UpdateCurrentTurns( 2 );
	int turns = d.getInfoInt( "turns" );
	ASSERT_EQUALS_V( 2, turns );

	d.UpdateCurrentCredits( 3 );
	int credits = d.getInfoInt( "credits" );
	ASSERT_EQUALS_V( 3, credits );

	d.UpdateCurrentFighters( 4 );
	int fighters = d.getInfoInt( "fighters" );
	ASSERT_EQUALS_V( 4, fighters );

	d.UpdateCurrentShields( 5 );
	int shields = d.getInfoInt( "shields" );
	ASSERT_EQUALS_V( 5, shields );

	d.UpdateCurrentHolds( 6 );
	int holds = d.getInfoInt( "holds" );
	ASSERT_EQUALS_V( 6, holds );

	d.UpdateCurrentFuelOre( 7 );
	int fuelOre = d.getInfoInt( "fuel ore" );
	ASSERT_EQUALS_V( 7, fuelOre );

	d.UpdateCurrentOrganics( 8 );
	int organics = d.getInfoInt( "organics" );
	ASSERT_EQUALS_V( 8, organics );

	d.UpdateCurrentEquipment( 9 );
	int equipment = d.getInfoInt( "equipment" );
	ASSERT_EQUALS_V( 9, equipment );

	d.UpdateCurrentColonists( 10 );
	int colonists = d.getInfoInt( "colonists" );
	ASSERT_EQUALS_V( 10, colonists );

	d.UpdateCurrentPhotonTorpedos( 11 );
	int photonTorpedos = d.getInfoInt( "photon torpedos" );
	ASSERT_EQUALS_V( 11, photonTorpedos );

	d.UpdateCurrentArmidMines( 12 );
	int armidMines = d.getInfoInt( "armid mines" );
	ASSERT_EQUALS_V( 12, armidMines );

	d.UpdateCurrentLimpetMines( 13 );
	int limpetMines = d.getInfoInt( "limpet mines" );
	ASSERT_EQUALS_V( 13, limpetMines );

	d.UpdateCurrentGenesisTorpedos( 14 );
	int genesisTorpedos = d.getInfoInt( "genesis torpedos" );
	ASSERT_EQUALS_V( 14, genesisTorpedos );

	d.UpdateCurrentTranswarpDrive( 15 );
	int transwarpDrive = d.getInfoInt( "transwarp drive" );
	ASSERT_EQUALS_V( 15, transwarpDrive );

	d.UpdateCurrentCloakingDevices( 16 );
	int cloakingDevices = d.getInfoInt( "cloaking devices" );
	ASSERT_EQUALS_V( 16, cloakingDevices );

	d.UpdateCurrentBeacons( 17 );
	int beacons = d.getInfoInt( "beacons" );
	ASSERT_EQUALS_V( 17, beacons );

	d.UpdateCurrentAtomicDetonators( 18 );
	int atomicDetonators = d.getInfoInt( "atomic detonators" );
	ASSERT_EQUALS_V( 18, atomicDetonators );

	d.UpdateCurrentCorbomiteDevices( 19 );
	int corbomiteDevices = d.getInfoInt( "corbomite devices" );
	ASSERT_EQUALS_V( 19, corbomiteDevices );

	d.UpdateCurrentEtherProbes( 20 );
	int etherProbes = d.getInfoInt( "ether probes" );
	ASSERT_EQUALS_V( 20, etherProbes );

	d.UpdateCurrentMineDisrupters( 21 );
	int mineDisrupters = d.getInfoInt( "mine disrupters" );
	ASSERT_EQUALS_V( 21, mineDisrupters );

	d.UpdateCurrentPsychicProbe( 22 );
	int psychicProbe = d.getInfoInt( "psychic probe" );
	ASSERT_EQUALS_V( 22, psychicProbe );

	d.UpdateCurrentPlanetScanner( 23 );
	int planetScanner = d.getInfoInt( "planet scanner" );
	ASSERT_EQUALS_V( 23, planetScanner );

	d.UpdateCurrentLongRangeScanner( 24 );
	int longRangeScanner = d.getInfoInt( "long range scanner" );
	ASSERT_EQUALS_V( 24, longRangeScanner );

	d.UpdateCurrentAlignment( -25 );
	int alignment = d.getInfoInt( "alignment" );
	ASSERT_EQUALS_V( -25, alignment );

	d.UpdateCurrentAlignment( 25 );
	alignment = d.getInfoInt( "alignment" );
	ASSERT_EQUALS_V( 25, alignment );

	d.UpdateCurrentExperience( 26 );
	int experience = d.getInfoInt( "experience" );
	ASSERT_EQUALS_V( 26, experience );

	d.UpdateCurrentCorporationNumber( 27 );
	int corporationNumber = d.getInfoInt( "corporation number" );
	ASSERT_EQUALS_V( 27, corporationNumber );

	d.UpdateCurrentShipNumber( 28 );
	int shipNumber = d.getInfoInt( "ship number" );
	ASSERT_EQUALS_V( 28, shipNumber );

	d.UpdateCurrentShipBriefName( "TKai" );
	std::string shipBriefName = d.getInfoString( "ship brief name" );
	ASSERT_EQUALS_V( std::string( "TKai" ), shipBriefName );
}

TEST( TwData, WhosOnline )
{
	RPW::TW::SqliteStore d;
	d.open( ":memory:", VERSION_1_CREATION_SQL );

	d.HandleBeginWhosOnline();
	d.HandleWhosOnline( 0, std::string( "rev" ), 1 );
	d.HandleWhosOnline( 0, std::string( "joe" ), 0 );
	d.HandleEndWhosOnline();

	int c =  d.getNumPlayersOnline();
	ASSERT_EQUALS_V( 2, c );
	c = d.getNumCorpsOnline();
	ASSERT_EQUALS_V( 1, c );

	Sleep( 100 );

	d.HandleBeginWhosOnline();
	d.HandleWhosOnline( 0, std::string( "rev" ), 1 );
	d.HandleWhosOnline( 0, std::string( "joe" ), 0 );
	d.HandleEndWhosOnline();

	c =  d.getNumPlayersOnline();
	ASSERT_EQUALS_V( 2, c );
	c = d.getNumCorpsOnline();
	ASSERT_EQUALS_V( 1, c );

	Sleep( 100 );

	d.HandleBeginWhosOnline();
	d.HandleWhosOnline( 0, std::string( "rev" ), 1 );
	d.HandleEndWhosOnline();

	c =  d.getNumPlayersOnline();
	ASSERT_EQUALS_V( 1, c );
	c = d.getNumCorpsOnline();
	ASSERT_EQUALS_V( 1, c );

	Sleep( 100 );

	d.HandleBeginWhosOnline();
	d.HandleEndWhosOnline();

	c =  d.getNumPlayersOnline();
	ASSERT_EQUALS_V( 0, c );
	c = d.getNumCorpsOnline();
	ASSERT_EQUALS_V( 0, c );
}

TEST( TwData, Ztm )
{
	RPW::TW::SqliteStore d;
	d.open( ":memory:", VERSION_1_CREATION_SQL );
	//std::string dt = d.getCurrentDateTimeStamp();
	//std::string::size_type i = 0;
	//while (( i = dt.find_first_of( ":." ) ) != std::string::npos )
	//{
	// dt.replace( i, 1, "_" );
	//}
	//dt += ".db3";
	//d.close();
	//d.open( dt.c_str() );

	RPW::TW::LemonParser p;
	p.Inject( d );
	p.Initialize();

	std::ifstream ifs( ".\\data\\sct.txt", std::ios::binary );
	ifs.seekg( std::fstream::end );
	unsigned long len = ifs.tellg();
	ifs.seekg( std::fstream::beg );
	std::stringstream ss;
	if ( ifs )
	{
		ss << ifs.rdbuf();
		ifs.close();

		while ( !ss.eof() )
		{
			std::string line;

			std::getline( ss, line, '\r' );
			line += '\r';
//   fprintf(stderr,line.c_str());
			p.Parse( line.c_str(), line.length() );
		}
	}

	ifs.close();
	ifs.clear();

	std::string a = d.db().getPreparedStatement( "select group_concat(dst) from warps where src=7" )
	                .reset().step().get_column_text( 0 );
	ASSERT_EQUALS_V( "1,2,6,8,1727,6355", a );

	std::string b = d.db().getPreparedStatement( "select group_concat(dst) from warps where src=6995" )
	                .reset().step().get_column_text( 0 );
	ASSERT_EQUALS_V( "501,3393", b );

	ifs.open( ".\\data\\ztm.txt", std::ios::binary );
	ifs.seekg( std::fstream::end );
	len = ifs.tellg();
	ifs.seekg( std::fstream::beg );
	ss.clear();
	if ( ifs )
	{
		ss << ifs.rdbuf();
		ifs.close();

		while ( !ss.eof() )
		{
			std::string line;

			std::getline( ss, line, '\r' );
			line += '\r';
//   fprintf(stderr,line.c_str());
			p.Parse( line.c_str(), line.length() );
		}
	}

	p.Cleanup();

	std::string c = d.db().getPreparedStatement( "select group_concat(dst) from warps where src=7" )
	                .reset().step().get_column_text( 0 );
	ASSERT_EQUALS_V( "1,2,6,8,1727,6355", c );

	std::string e = d.db().getPreparedStatement( "select group_concat(dst) from warps where src=6867" )
	                .reset().step().get_column_text( 0 );
	ASSERT_EQUALS_V( "416,1657,2910,3637,5662,5663", e );

	std::string f = d.db().getPreparedStatement( "select group_concat(dst) from warps where src=6937" )
	                .reset().step().get_column_text( 0 );
	ASSERT_EQUALS_V( "177,6039", f );
}

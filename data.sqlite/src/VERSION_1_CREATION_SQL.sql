
PRAGMA legacy_file_format = false;
BEGIN TRANSACTION;
CREATE TABLE IF NOT EXISTS ZG (
	"guid_" integer primary key, 
	"guid" integer, 
	UNIQUE( "guid" ) );

INSERT INTO ZG ( "guid_", "guid" ) VALUES( 0, 0 );

CREATE TABLE IF NOT EXISTS ZT (
	"type_" integer primary key, 
	"typeName" text, 
	"type$" integer DEFAULT ( cast( strftime( '%s', 'now' ) as integer ) ), 
	"guid_" integer DEFAULT 1, 
	UNIQUE( "typeName", "guid_" ) );

INSERT INTO ZT ( "type_", "typeName", "guid_" ) VALUES( 0, 'unknown', 0 );
INSERT INTO ZT ( "typeName", "guid_" ) VALUES( 'global', 0 );
INSERT INTO ZT ( "typeName", "guid_" ) VALUES( 'stardock', 0 );
INSERT INTO ZT ( "typeName", "guid_" ) VALUES( 'special', 0 );
INSERT INTO ZT ( "typeName", "guid_" ) VALUES( 'BBS', 0 );
INSERT INTO ZT ( "typeName", "guid_" ) VALUES( 'BSB', 0 );
INSERT INTO ZT ( "typeName", "guid_" ) VALUES( 'SBB', 0 );
INSERT INTO ZT ( "typeName", "guid_" ) VALUES( 'SSB', 0 );
INSERT INTO ZT ( "typeName", "guid_" ) VALUES( 'SBS', 0 );
INSERT INTO ZT ( "typeName", "guid_" ) VALUES( 'BSS', 0 );
INSERT INTO ZT ( "typeName", "guid_" ) VALUES( 'SSS', 0 );
INSERT INTO ZT ( "typeName", "guid_" ) VALUES( 'BBB', 0 );
INSERT INTO ZT ( "typeName", "guid_" ) VALUES( 'planet', 0 );
INSERT INTO ZT ( "typeName", "guid_" ) VALUES( 'ship', 0 );
INSERT INTO ZT ( "typeName", "guid_" ) VALUES( 'ambassador', 0 );
INSERT INTO ZT ( "typeName", "guid_" ) VALUES( 'player', 0 );
INSERT INTO ZT ( "typeName", "guid_" ) VALUES( 'corporation', 0 );
INSERT INTO ZT ( "typeName", "guid_" ) VALUES( 'alien', 0 );

CREATE TABLE IF NOT EXISTS ZI (
	"infoKey",
	"infoValue",
	"actor_" integer DEFAULT 0,
	"info$" integer DEFAULT ( cast( strftime( '%s', 'now' ) as integer ) ),
	"guid_" integer DEFAULT 1,
	UNIQUE( "infoKey", "actor_", "guid_" ) );

INSERT INTO ZI ( "infoKey", "infoValue", "actor_", "guid_" ) VALUES( 'schema', 1, 0, 0 );

CREATE TABLE IF NOT EXISTS ZA (
	"actor_" integer primary key,
	"actorName" text,
	"type_" integer,
	"corpNum" integer,
	"actorNum" integer,
	"actor$" integer DEFAULT ( cast( strftime( '%s', 'now' ) as integer ) ), 
	"guid_" integer DEFAULT 1, 
	UNIQUE( "actorName", "type_", "corpNum", "actorNum", "guid_" ) );

INSERT INTO ZA ( "actor_", "actorName", "type_", "corpNum", "actorNum", "guid_" ) VALUES( 0, "", 0, 0, 0, 0 );
	
CREATE TABLE IF NOT EXISTS ZR (
	"sector_" integer, 
	"actor_" integer, 
	"report$" integer DEFAULT ( cast( strftime( '%s', 'now' ) as integer ) ), 
	"guid_" integer DEFAULT 1 );

CREATE TABLE IF NOT EXISTS ZF (
	"sector_" integer, 
	"figCount" integer, 
	"actor_" integer, 
	"fig$" integer DEFAULT ( cast( strftime( '%s', 'now' ) as integer ) ), 
	"guid_" integer DEFAULT 1, 
	UNIQUE( "sector_", "guid_" ) );

CREATE TABLE IF NOT EXISTS ZH (
	"port_" integer, 
	"productNum" integer, 
	"experience" integer, 
	"planetNego" integer,
	"iteration" integer, 
	"offer" integer, 
	"counterOffer" integer, 
	"psychicPercent" integer, 
	"haggle$" integer DEFAULT ( cast( strftime( '%s', 'now' ) as integer ) ), 
	"guid_" integer DEFAULT 1 );

CREATE TABLE IF NOT EXISTS ZL (
	"sector_" integer, 
	"limpetCount" integer, 
	"actor_" integer, 
	"limpet$" integer DEFAULT ( cast( strftime( '%s', 'now' ) as integer ) ), 
	"guid_" integer DEFAULT 1, 
	UNIQUE( "sector_", "guid_" ) );

CREATE TABLE IF NOT EXISTS ZM (
	"sector_" integer, 
	"mineCount" integer, 
	"actor_" integer, 
	"mine$" integer DEFAULT ( cast( strftime( '%s', 'now' ) as integer ) ), 
	"guid_" integer DEFAULT 1, 
	UNIQUE( "sector_", "guid_" ) );

CREATE TABLE IF NOT EXISTS ZP (
	"port_" integer primary key, 
	"sector_" integer, 
	"portName" text, 
	"type_" integer, 
	"ore" integer, 
	"oreMax" integer, 
	"oreMcic" integer, 
	"org" integer, 
	"orgMax" integer, 
	"orgMcic" integer, 
	"equ" integer, 
	"equMax" integer, 
	"equMcic" integer, 
	"days" integer, 
	"actor_" integer, 
	"port$" integer DEFAULT ( cast( strftime( '%s', 'now' ) as integer ) ), 
	"guid_" integer DEFAULT 1, 
	UNIQUE( "sector_", "guid_" ) );

CREATE TABLE IF NOT EXISTS ZS (
	"sector_" integer, 
	"port_" integer, 
	"beacon" text, 
	"space" text, 
	"sector$" integer DEFAULT ( cast( strftime( '%s', 'now' ) as integer ) ), 
	"guid_" integer DEFAULT 1, 
	UNIQUE( "sector_", "guid_" ) );

CREATE TABLE IF NOT EXISTS ZW (
	"sector_" integer, 
	"warp" integer, 
	"warp$" integer DEFAULT ( cast( strftime( '%s', 'now' ) as integer ) ), 
	"guid_" integer DEFAULT 1, 
	UNIQUE( "sector_", "warp", "guid_" ) );

CREATE TABLE IF NOT EXISTS ZO (
	"online_" integer primary key,
	"actor_" integer, 
	"isOnline" integer, 
	"online1$" integer DEFAULT ( cast( strftime( '%s', 'now' ) as integer ) ), 
	"online2$" integer DEFAULT ( cast( strftime( '%s', 'now' ) as integer ) ), 
	"guid_" integer DEFAULT 1 );

COMMIT;

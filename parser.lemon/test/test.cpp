/// @file test.cpp
/// @author RPW

#include "testharness.h"
#include "XmlReportPrinter.h"

#include "LemonParser.h"

#include <windows.h>
#include <fstream>
#include <sstream>
#include <string>

using namespace easyunit;

class NopActor : public RPW::TW::IDataActor
{
public:
	virtual void HandlePlayTradewarsMenu() { }
	virtual void HandleShowTodaysLog() { }
	virtual void HandleHelloName( const std::string& name ) { }
	virtual void HandleVersion( unsigned long major, unsigned long minor ) { }
	virtual void HandleMbbs() { }
	virtual void HandleGold() { }
	virtual void HandleRegisteredTo( const std::string& name ) { }
	virtual void HandleCourtesyOf( const std::string& name ) { }
	virtual void HandleNewCharacter() { }
	virtual void HandleEnterPassword() { }
	virtual void HandleRepeatPassword() { }
	virtual void HandleBeginWhosOnline() { }
	virtual void HandleWhosOnline( long rank, const std::string& name, unsigned long corp ) { }
	virtual void HandleEndWhosOnline() { }
	virtual void HandleCommandPrompt() { }
	virtual void HandleComputerPrompt() { }
	virtual void HandleCorporatePrompt() { }
	virtual void HandleCommandChar( char letter ) { }
	virtual void HandleComputerChar( char letter ) { }
	virtual void HandleCorporateChar( char letter ) { }
	virtual void HandleCommandDigits( unsigned long sector ) { }
	virtual void UpdateTimeRemain( unsigned long seconds ) { }
	virtual void BeginQuickStats( unsigned long sector ) { }
	virtual void UpdateCurrentSector( unsigned long sector ) { }
	virtual void UpdateCurrentTurns( unsigned long turns ) { }
	virtual void UpdateCurrentCredits( unsigned long credits ) { }
	virtual void UpdateCurrentFighters( unsigned long fighters ) { }
	virtual void UpdateCurrentShields( unsigned long shields ) { }
	virtual void UpdateCurrentHolds( unsigned long holds ) { }
	virtual void UpdateCurrentFuelOre( unsigned long fuelOre ) { }
	virtual void UpdateCurrentOrganics( unsigned long organics ) { }
	virtual void UpdateCurrentEquipment( unsigned long equipment ) { }
	virtual void UpdateCurrentColonists( unsigned long colonists ) { }
	virtual void UpdateCurrentPhotonTorpedos( unsigned long photonTorpedos ) { }
	virtual void UpdateCurrentArmidMines( unsigned long armidMines ) { }
	virtual void UpdateCurrentLimpetMines( unsigned long limpetMines ) { }
	virtual void UpdateCurrentGenesisTorpedos( unsigned long genesisTorpedos ) { }
	virtual void UpdateCurrentTranswarpDrive( unsigned long transwarpDrive ) { }
	virtual void UpdateCurrentCloakingDevices( unsigned long cloakingDevices ) { }
	virtual void UpdateCurrentBeacons( unsigned long beacons ) { }
	virtual void UpdateCurrentAtomicDetonators( unsigned long atomicDetonators ) { }
	virtual void UpdateCurrentCorbomiteDevices( unsigned long corbomiteDevices ) { }
	virtual void UpdateCurrentEtherProbes( unsigned long etherProbes ) { }
	virtual void UpdateCurrentMineDisrupters( unsigned long mineDisrupters ) { }
	virtual void UpdateCurrentPsychicProbe( unsigned long psychicProbe ) { }
	virtual void UpdateCurrentPlanetScanner( unsigned long planetScanner ) { }
	virtual void UpdateCurrentLongRangeScanner( unsigned long longRangeScanner ) { }
	virtual void UpdateCurrentAlignment( long alignment ) { }
	virtual void UpdateCurrentExperience( unsigned long experience ) { }
	virtual void UpdateCurrentCorporationNumber( unsigned long corporationNumber ) { }
	virtual void UpdateCurrentShipNumber( unsigned long shipNumber ) { }
	virtual void UpdateCurrentShipBriefName( const std::string& shipBriefName ) { }
	virtual void EndQuickStats( const std::string& shipBriefName ) { }
	virtual void HandleEnterCim() { }
	virtual void HandleExitCim() { }
	virtual void HandleCoursePlot( std::vector<unsigned long>& course ) { }
	virtual void HandleStartWarpCim() { }
	virtual void HandleWarpCimEntry( std::vector<unsigned long>& warpSpec ) { }
	virtual void HandleStartPortCim() { }
	virtual void HandlePortCimEntry( unsigned long sector, long fuelOreQuan, unsigned long fuelOrePerc, long organicsQuan, unsigned long organicsPerc, long equipmentQuan, unsigned long equipmentPerc ) { }
	virtual void HandleBeginFigList() { }
	virtual void HandleEndFigList() { }
	virtual void HandleAbortFigList() { }
	virtual void RecordSectorFigs( unsigned long sector, unsigned long count, unsigned int ownership, unsigned int type ) { }
};

TEST( LemonParser, NopActor )
{
	NopActor n;
}

class MainPromptsActor : public NopActor
{
public:
	void HandleCommandPrompt()
	{
		oss << "{command prompt}";
	}

	void HandleComputerPrompt()
	{
		oss << "{computer prompt}";
	}

	void HandleCorporatePrompt()
	{
		oss << "{corporate prompt}";
	}

	void UpdateTimeRemain( unsigned long seconds )
	{
		oss << "{time remain = " << seconds << "}";
	}

	void UpdateCurrentSector( unsigned long sector )
	{
		oss << "{current sector = " << sector << "}";
	}

	void HandleCommandChar( char letter )
	{
		oss << "{command char = " << letter << "}";
	}

	void HandleComputerChar( char letter )
	{
		oss << "{computer char = " << letter << "}";
	}

	void HandleCorporateChar( char letter )
	{
		oss << "{corporate char = " << letter << "}";
	}

	void HandleCommandDigits( unsigned long sector )
	{
		oss << "{moving to sector " << sector << "}";
	}

	std::ostringstream oss;
};

TEST( LemonParser, MainPromptsActor )
{
	MainPromptsActor actor;
	RPW::TW::LemonParser p;
	p.Inject( actor );
	p.Initialize();

	p.Parse( "\n\x1b[35mCommand [\x1b[1;33mTL\x1b[0;33m=\x1b[1m00:00:00\x1b[0;35m]\x1b[1;37m:\x1b[0;35m[\x1b[1;36m1\x1b[0;35m] (\x1b[1;33m?=Help\x1b[0;35m)? : \r" );
	p.Parse( "\n\x1b[35mCommand [\x1b[1;33mTL\x1b[0;33m=\x1b[1m00:00:00\x1b[0;35m]\x1b[1;37m:\x1b[0;35m[\x1b[1;36m1\x1b[0;35m] (\x1b[1;33m?=Help\x1b[0;35m)? : 1234\r" );
	p.Parse( "\n\x1b[35mCommand [\x1b[1;33mTL\x1b[0;33m=\x1b[1m00:00:00\x1b[0;35m]\x1b[1;37m:\x1b[0;35m[\x1b[1;36m1\x1b[0;35m] (\x1b[1;33m?=Help\x1b[0;35m)? : 1234\x08 \x08\r" );
	p.Parse( "\n\x1b[35mComputer command [\x1b[1;33mTL\x1b[0;33m=\x1b[1m00:00:00\x1b[0;35m]\x1b[1;37m:\x1b[0;35m[\x1b[1;36m1\x1b[0;35m] (\x1b[1;33m?=Help\x1b[0;35m)? L\r" );
	p.Parse( "\n\x1b[35mCorporate command [\x1b[1;33mTL\x1b[0;33m=\x1b[1m00:00:00\x1b[0;35m]\x1b[1;37m:\x1b[0;35m[\x1b[1;36m5666\x1b[0;35m] (\x1b[1;33m?=Help\x1b[0;35m)? M\r" );
	p.Cleanup();

	ASSERT_EQUALS_V( "{command prompt}{time remain = 0}{current sector = 1}{command prompt}{time remain = 0}{current sector = 1}{moving to sector 1234}{command prompt}{time remain = 0}{current sector = 1}{moving to sector 123}{computer prompt}{time remain = 0}{current sector = 1}{computer char = L}{corporate prompt}{time remain = 0}{current sector = 5666}{corporate char = M}", actor.oss.str() );
}

class QuickStatsActor : public NopActor
{
public:
	virtual void UpdateCurrentSector( unsigned long sector ) { oss << " SECT " << sector; }
	virtual void UpdateCurrentTurns( unsigned long turns ) { oss << " TURNS " << turns; }
	virtual void UpdateCurrentCredits( unsigned long credits ) { oss << " CREDS " << credits; }
	virtual void UpdateCurrentFighters( unsigned long fighters ) { oss << " FIGS " << fighters; }
	virtual void UpdateCurrentShields( unsigned long shields ) { oss << " SHLDS " << shields; }
	virtual void UpdateCurrentHolds( unsigned long holds ) { oss << " HOLDS " << holds; }
	virtual void UpdateCurrentFuelOre( unsigned long fuelOre ) { oss << " FUEL " << fuelOre; }
	virtual void UpdateCurrentOrganics( unsigned long organics ) { oss << " ORG " << organics; }
	virtual void UpdateCurrentEquipment( unsigned long equipment ) { oss << " EQU " << equipment; }
	virtual void UpdateCurrentColonists( unsigned long colos ) { oss << " COLOS " << colos; }
	virtual void UpdateCurrentPhotonTorpedos( unsigned long photons ) { oss << " PHOT " << photons; }
	virtual void UpdateCurrentArmidMines( unsigned long armids ) { oss << " ARMD " << armids; }
	virtual void UpdateCurrentLimpetMines( unsigned long limpets ) { oss << " LMPT " << limpets; }
	virtual void UpdateCurrentGenesisTorpedos( unsigned long genesisTorps ) { oss << " GTRP " << genesisTorps; }
	virtual void UpdateCurrentTranswarpDrive( unsigned long twarp ) { oss << " TWARP " << twarp; }
	virtual void UpdateCurrentCloakingDevices( unsigned long cloaks ) { oss << " CLKS " << cloaks; }
	virtual void UpdateCurrentBeacons( unsigned long beacons ) { oss << " BEAC " << beacons; }
	virtual void UpdateCurrentAtomicDetonators( unsigned long atomicDetonators ) { oss << " ADET " << atomicDetonators; }
	virtual void UpdateCurrentCorbomiteDevices( unsigned long corbomite ) { oss << " CORB " << corbomite; }
	virtual void UpdateCurrentEtherProbes( unsigned long eprobes ) { oss << " EPRB " << eprobes; }
	virtual void UpdateCurrentMineDisrupters( unsigned long mineDisrupters ) { oss << " MDIS " << mineDisrupters; }
	virtual void UpdateCurrentPsychicProbe( unsigned long psychicProbe ) { oss << " PSYP " << psychicProbe; }
	virtual void UpdateCurrentPlanetScanner( unsigned long planetScanner ) { oss << " PLSC " << planetScanner; }
	virtual void UpdateCurrentLongRangeScanner( unsigned long longRangeScanner ) { oss << " LRS " << longRangeScanner; }
	virtual void UpdateCurrentAlignment( long alignment ) { oss << " ALN " << alignment; }
	virtual void UpdateCurrentExperience( unsigned long experience ) { oss << " EXP " << experience; }
	virtual void UpdateCurrentCorporationNumber( unsigned long corp ) { oss << " CORP " << corp; }
	virtual void UpdateCurrentShipNumber( unsigned long ship ) { oss << " SHIP " << ship; }
	virtual void UpdateCurrentShipBriefName( const std::string& brief ) { oss << " BRIEF " << brief; }

	std::ostringstream oss;
};

TEST( LemonParser, QuickStatsActor )
{
	std::string qs = "\x1b[30;47m Sect 5666³Turns 1,773³Creds 10,000³Figs 500³Shlds 2,000³Hlds 20³Ore 1³Org 2³Equ 3 \r\x1b[0m\n Col 4³Phot 99³Armd 98³Lmpt 97³GTorp 96³TWarp No³Clks 94³Beacns 93³AtmDt 92³Crbo 91    \r\n\x1b[30;47m EPrb 90³MDis 89³PsPrb No³PlScn No³LRS None³Aln -55,111³Exp 4,000,000³Corp 1³Ship 2 MerCru     \r\x1b[0m\n";

	QuickStatsActor actor;
	RPW::TW::LemonParser p;
	p.Inject( actor );
	p.Initialize();

	p.Parse( qs.c_str() );
	p.Cleanup();

	ASSERT_EQUALS_V( " SECT 5666 TURNS 1773 CREDS 10000 FIGS 500 SHLDS 2000 HOLDS 20 FUEL 1 ORG 2 EQU 3 COLOS 4 PHOT 99 ARMD 98 LMPT 97 GTRP 96 TWARP 0 CLKS 94 BEAC 93 ADET 92 CORB 91 EPRB 90 MDIS 89 PSYP 0 PLSC 0 LRS 0 ALN -55111 EXP 4000000 CORP 1 SHIP 2 BRIEF MerCru", actor.oss.str() );
}

class CoursePlotActor : public NopActor
{
public:
	virtual void HandleCoursePlot( std::vector<unsigned long>& course )
	{
		std::vector<unsigned long> list;
		list.swap( course );

		oss << "PLOT ";
		for ( unsigned long i = 0; i < list.size(); i++ )
		{
			oss << list[i] << " ";
		}
	}

	std::ostringstream oss;
};

TEST( LemonParser, CoursePlotActor )
{
	{
		CoursePlotActor actor;
		RPW::TW::LemonParser p;
		p.Inject( actor );
		p.Initialize();

		p.Parse( "\x1b[0m\n: \r\nFM > 1\r\n  TO > 11\r\n1 > 3 > (1929) > (794) > (3639) > (2167) > (4831) > (2815) > (1389) > (6255) >\r\n (11)\r\n\r\n" );
		p.Parse( ": \r\nFM > 1\r\n  TO > 1389\r\n1 > 3 > (1929) > (794) > (3639) > (2167) > (4831) > (2815) > (1389)\r\n\r\n\r\n: ENDINTERROG\r\n\r\n" );
		p.Cleanup();

		ASSERT_EQUALS_V( "PLOT 1 3 1929 794 3639 2167 4831 2815 1389 6255 11 PLOT 1 3 1929 794 3639 2167 4831 2815 1389 ", actor.oss.str() );
	}

	{
		CoursePlotActor actor;
		RPW::TW::LemonParser p;
		p.Inject( actor );
		p.Initialize();

		p.Parse( "\x1b[32mThe shortest path \x1b[35m(12 hops, 36 turns)\x1b[32m from sector \x1b[1;33m1\x1b[0;32m to sector \x1b[1;33m11\x1b[0;32m is\x1b[1;33m:\r\x1b[0m\n\x1b[33m1\x1b[1m >\x1b[31m (5)\x1b[33m >\x1b[31m (2512)\x1b[33m >\x1b[31m (1185)\x1b[33m >\x1b[31m (4198)\x1b[33m >\x1b[31m (5226)\x1b[33m >\x1b[31m (1966)\x1b[33m >\x1b[31m (6570)\x1b[33m >\x1b[31m (931)\x1b[33m >\r\x1b[0m\n\x1b[1;31m (3799)\x1b[33m >\x1b[31m (1626)\x1b[33m >\x1b[31m (5491)\x1b[33m >\x1b[31m (11)\r\x1b[0m\n\r\n" );
		p.Cleanup();

		ASSERT_EQUALS_V( "PLOT 1 5 2512 1185 4198 5226 1966 6570 931 3799 1626 5491 11 ", actor.oss.str() );
	}
}

class CimDataActor : public NopActor
{
public:
	virtual void HandleEnterCim()
	{
		oss << "{cim}";
	}
	virtual void HandleExitCim()
	{
		oss << "{exit}";
	}
	virtual void HandleStartWarpCim()
	{
		oss << "{warpspec}";
	}
	virtual void HandleWarpCimEntry( std::vector<unsigned long>& warpSpec )
	{
		oss << "{";
		for ( unsigned long i = 0; i < warpSpec.size(); i++ )
		{
			if ( i > 0 ) oss << " ";
			oss << warpSpec[i];
		}
		oss << "}";
	}
	virtual void HandleStartPortCim()
	{
		oss << "{portspec}";
	}
	virtual void HandlePortCimEntry( unsigned long sector, long fuelOreQuan, unsigned long fuelOrePerc, long organicsQuan, unsigned long organicsPerc, long equipmentQuan, unsigned long equipmentPerc )
	{
		oss << "{" << sector << " " << fuelOreQuan << " " << fuelOrePerc << "% " << organicsQuan << " " << organicsPerc << "% " << equipmentQuan << " " << equipmentPerc << "%}";
	}

	std::ostringstream oss;
};

TEST( LemonParser, CimDataActor )
{
	CimDataActor actor;
	RPW::TW::LemonParser p;
	p.Inject( actor );
	p.Initialize();

	p.Parse( ": \r\n" );
	p.Parse( "    1     2     3     4     5     6     7\r\n" );
	p.Parse( "    2     1     3     7     8     9    10\r\n" );
	p.Parse( "    3     1     2     4\r\n" );
	p.Parse( "    4     1     3     5  1594\r\n" );
	p.Parse( "    5     1     4     6  1052  2512  3182\r\n" );
	p.Parse( "    6     1     5     7  4263  6564\r\n" );
	p.Parse( "    7     1     2     6     8\r\n" );
	p.Parse( "    8     2     7\r\n" );
	p.Parse( "    9     2    10   219  3872  4346\r\n" );
	p.Parse( "   10     2     9  3523\r\n" );
	p.Parse( "  219     9\r\n" );
	p.Parse( "\r\n" );
	p.Parse( ": \r\n" );
	p.Parse( "    2 - 2520 100%    850 100% - 2380 100%\r\n" );
	p.Parse( "    3   2030 100% - 2920 100%   2850 100%\r\n" );
	p.Parse( "    5 - 2160 100% - 2860 100%   2600 100%\r\n" );
	p.Parse( "    6 - 1440 100%   2850 100% - 1800 100%\r\n" );
	p.Parse( "    7   1820 100% - 1780 100%   1750 100%\r\n" );
	p.Parse( "    8   2460 100% -  920 100% - 1390 100%\r\n" );
	p.Parse( "    9 - 1680 100% - 2580 100% - 1790 100%\r\n" );
	p.Parse( "   10   2560 100%   1790 100% - 2930 100%\r\n" );
	p.Parse( "\r\n" );
	p.Parse( ": ENDINTERROG\r\n" );
	p.Cleanup();

//{cim}
//{warpspec}
//{1 2 3 4 5 6 7}
//{2 1 3 7 8 9 10}
//{3 1 2 4}
//{4 1 3 5 1594}
//{5 1 4 6 1052 2512 3182}
//{6 1 5 7 4263 6564}
//{7 1 2 6 8}
//{8 2 7}
//{9 2 10 219 3872 4346}
//{10 2 9 3523}
//{219 9}
//{cim}
//{portspec}
//{2 -2520 100% 850 100% -2380 100%}
//{3 2030 100% -2920 100% 2850 100%}
//{5 -2160 100% -2860 100% 2600 100%}
//{6 -1440 100% 2850 100% -1800 100%}
//{7 1820 100% -1780 100% 1750 100%}
//{8 2460 100% -920 100% -1390 100%}
//{9 -1680 100% -2580 100% -1790 100%}
//{10 2560 100% 1790 100% -2930 100%}
//{exit}

	ASSERT_EQUALS_V( "{cim}{warpspec}{1 2 3 4 5 6 7}{2 1 3 7 8 9 10}{3 1 2 4}{4 1 3 5 1594}{5 1 4 6 1052 2512 3182}{6 1 5 7 4263 6564}{7 1 2 6 8}{8 2 7}{9 2 10 219 3872 4346}{10 2 9 3523}{219 9}{cim}{portspec}{2 -2520 100% 850 100% -2380 100%}{3 2030 100% -2920 100% 2850 100%}{5 -2160 100% -2860 100% 2600 100%}{6 -1440 100% 2850 100% -1800 100%}{7 1820 100% -1780 100% 1750 100%}{8 2460 100% -920 100% -1390 100%}{9 -1680 100% -2580 100% -1790 100%}{10 2560 100% 1790 100% -2930 100%}{exit}", actor.oss.str() );
}

class FigListActor : public NopActor
{
public:
	virtual void HandleBeginFigList()
	{
		oss << "{figs}";
	}

	virtual void HandleEndFigList()
	{
		oss << "{end}";
	}

	virtual void HandleAbortFigList()
	{
		oss << "{abort}";
	}

	virtual void RecordSectorFigs( unsigned long sector, unsigned long count, unsigned int ownership, unsigned int type )
	{
		oss << "{" << sector << " " << count << " " << ownership << " " << type << "}";
	}

	std::ostringstream oss;
};

TEST( LemonParser, FigListActor )
{
	FigListActor actor;
	RPW::TW::LemonParser p;
	p.Inject( actor );
	p.Initialize();

	p.Parse( "\x1b[2J\r\n" );
	p.Parse( "\x1b[1;44m                 Deployed  Fighter  Scan                   \r" );
	p.Parse( "\x1b[0m\n" );
	p.Parse( "\r\n" );
	p.Parse( "\x1b[1;36m Sector    Fighters    Personal/Corp    Mode        Tolls  \r" );
	p.Parse( "\x1b[0m\n" );
	p.Parse( "\x1b[35m===========================================================\r" );
	p.Parse( "\x1b[0m\n" );
	p.Parse( "\x1b[1;36m    77\x1b[33m           1\x1b[0;35m         Corp   \x1b[33m   Defensive   \x1b[1m         N/A\r" );
	p.Parse( "\x1b[0m\n" );
	p.Parse( "\x1b[1;36m  6821\x1b[33m           1\x1b[0;35m         Corp   \x1b[33m   Defensive   \x1b[1m         N/A\r" );
	p.Parse( "\x1b[0m\n" );
	p.Parse( "\x1b[1;33m                50\x1b[36m Total\x1b[33m                                  0\x1b[36m Total\r" );
	p.Parse( "\x1b[0m\n" );
	p.Parse( "\r\n" );
	p.Parse( "\n\x1b[35mCommand [\x1b[1;33mTL\x1b[0;33m=\x1b[1m00:00:00\x1b[0;35m]\x1b[1;37m:\x1b[0;35m[\x1b[1;36m1\x1b[0;35m] (\x1b[1;33m?=Help\x1b[0;35m)? : \r" );
	p.Parse( "\x1b[2J\r\n" );
	p.Parse( "\x1b[1;44m                 Deployed  Fighter  Scan                   \r" );
	p.Parse( "\x1b[0m\n" );
	p.Parse( "\r\n" );
	p.Parse( "\x1b[1;36m Sector    Fighters    Personal/Corp    Mode        Tolls  \r" );
	p.Parse( "\x1b[0m\n" );
	p.Parse( "\x1b[35m===========================================================\r" );
	p.Parse( "\x1b[0m\n" );
	p.Parse( "\x1b[1;36m    77\x1b[33m           1\x1b[0;35m         Corp   \x1b[33m   Defensive   \x1b[1m         N/A\r" );
	p.Parse( "\x1b[0m\n" );
	p.Parse( "\x1b[1;36m  6821\x1b[33m           1\x1b[0;35m         Corp   \x1b[33m   Defensive   \x1b[1m         N/A\r" );
	p.Parse( "\x1b[0m\n" );
	p.Parse( "\r\n" );
	p.Parse( "\n\x1b[35mCommand [\x1b[1;33mTL\x1b[0;33m=\x1b[1m00:00:00\x1b[0;35m]\x1b[1;37m:\x1b[0;35m[\x1b[1;36m1\x1b[0;35m] (\x1b[1;33m?=Help\x1b[0;35m)? : \r" );
	p.Cleanup();

	ASSERT_EQUALS_V( "{figs}{77 1 1 0}{6821 1 1 0}{end}{figs}{77 1 1 0}{6821 1 1 0}{abort}", actor.oss.str() );
}

class LoginActor : public NopActor
{
public:
	virtual void HandlePlayTradewarsMenu()
	{
		oss << "{play tw menu}";
	}

	virtual void HandleShowTodaysLog()
	{
		oss << "{show todays log}";
	}

	virtual void HandleHelloName( const std::string& name )
	{
		oss << "{hello " << name << "}";
	}

	virtual void HandleVersion( unsigned long major, unsigned long minor )
	{
		oss << "{version " << major << "." << minor << "}";
	}

	virtual void HandleMbbs()
	{
		oss << "{mbbs}";
	}

	virtual void HandleGold()
	{
		oss << "{gold}";
	}

	virtual void HandleRegisteredTo( const std::string& name )
	{
		oss << "{registered to " << name << "}";
	}

	virtual void HandleCourtesyOf( const std::string& name )
	{
		oss << "{courtesy of " << name << "}";
	}

	virtual void HandleNewCharacter()
	{
		oss << "{new character}";
	}

	virtual void HandleEnterPassword()
	{
		oss << "{enter password}";
	}

	virtual void HandleRepeatPassword()
	{
		oss << "{repeat password}";
	}

	std::ostringstream oss;
};

TEST( LemonParser, LoginActor )
{
	{
		LoginActor actor;
		RPW::TW::LemonParser p;
		p.Inject( actor );
		p.Initialize();

		p.Parse( "   \x1b[36m==\x1b[1m-\x1b[0m- \x1b[1mTrade Wars 2002 \x1b[0m-\x1b[1;36m-\x1b[0;36m==\r" );
		p.Parse( "\x1b[0m\n" );
		p.Parse( "\r\n" );
		p.Parse( "   \x1b[1mT \x1b[36m- Play Trade Wars 2002\r" );
		p.Parse( "\x1b[0m\n" );
		p.Parse( "   \x1b[1mI \x1b[36m- Introduction & Help\r" );
		p.Parse( "\x1b[0m\n" );
		p.Parse( "   \x1b[1mS \x1b[36m- View Game Settings\r" );
		p.Parse( "\x1b[0m\n" );
		p.Parse( "   \x1b[1mH \x1b[36m- High scores\r" );
		p.Parse( "\x1b[0m\n" );
		p.Parse( "   \x1b[1mA \x1b[36m- View Announcement\r" );
		p.Parse( "\x1b[0m\n" );
		p.Parse( "   \x1b[1mX \x1b[36m- Exit\r" );
		p.Parse( "\x1b[0m\n" );
		p.Parse( "\r\n" );
		p.Parse( "\x1b[1;36mEnter your choice: \x1b[1;32m\r" );
		p.Parse( "\x1b[0m\n" );
		p.Parse( "\x1b[2J\x1b[32mShow today's log? (\x1b[1;33mY/N\x1b[0;32m) [\x1b[1;36mN\x1b[0;32m] \x1b[1;36mNo\r" );
		p.Parse( "\x1b[0m\n" );
		p.Parse( "\x1b[2J" );
		p.Parse( "\x1b[32mInitializing...\r" );
		p.Parse( "\x1b[0m\n" );
		p.Parse( "\r\n" );
		p.Parse( "\x1b[32m       Hello \x1b[1;36mrev\x1b[0;32m, welcome to:\r" );
		p.Parse( "\x1b[0m\n" );
		p.Parse( "\r\n" );
		p.Parse( "           \x1b[1;33mTrade Wars 2002\x1b[36m \x1b[31mver \x1b[36m3.13 MBBS Gold\r" );
		p.Parse( "\x1b[0m\n" );
		p.Parse( "\r\n" );
		p.Parse( "\x1b[32m(C) Copyright 1986 John Pritchett.  All rights reserved.\r" );
		p.Parse( "\x1b[0m\n" );
		p.Parse( "\x1b[32m    Brought to you by EIS\r" );
		p.Parse( "\x1b[0m\n" );
		p.Parse( "\x1b[32m    Written by Gary Martin, Mary Ann Martin and John Pritchett\r" );
		p.Parse( "\x1b[0m\n" );
		p.Parse( "\r\n" );
		p.Parse( "\x1b[35mFor Trade Wars support, visit \x1b[1;33mEIS \x1b[0;35mon the Net!\r" );
		p.Parse( "\x1b[0m\n" );
		p.Parse( "\r\n" );
		p.Parse( "           \x1b[35mWeb:   \x1b[33mhttp://www.tradewars.com\r" );
		p.Parse( "\x1b[0m\n" );
		p.Parse( "                  \x1b[33mhttp://www.eisonline.com\r" );
		p.Parse( "\x1b[0m\n" );
		p.Parse( "           \x1b[35mEmail: \x1b[33mjpritch@eisonline.com\r" );
		p.Parse( "\x1b[0m\n" );
		p.Parse( "\r\n" );
		p.Parse( "\r\n" );
		p.Parse( "\r\n" );
		p.Parse( "\x1b[32mThis copy of TW2002 is \x1b[31;47mRegistered\x1b[0m\x1b[32m to \x1b[1;36mthe reverend\r" );
		p.Parse( "\x1b[0m\n" );
		p.Parse( "\x1b[32mand is being brought to you courtesy of \x1b[1;33mthe reverend\r" );
		p.Parse( "\x1b[0m\n" );
		p.Parse( "\x1b[35m[Pause]\r" );
		p.Parse( "\x1b[0m\n" );
		p.Parse( "\r\n" );
		p.Parse( "\x1b[32mYou were not found in the player database.\r" );
		p.Parse( "\x1b[0m\n" );
		p.Parse( "\x1b[1;44mWould you like to start a new character in this game?  (Type \x1b[0m\x1b[1;33mY\x1b[37;44m or \x1b[0m\x1b[1;33mN\x1b[37;44m) \x1b[0m\x1b[1;36mYes\r" );
		p.Parse( "\x1b[0m\n" );
		p.Parse( "\r\n" );
		p.Parse( "\x1b[1;33mPlease enter a password for this game account.\r" );
		p.Parse( "\x1b[0m\n" );
		p.Parse( "\r\n" );
		p.Parse( "\x1b[35mPassword? ******\r" );
		p.Parse( "\x1b[0m\n" );
		p.Parse( "\x1b[1;33mRepeat password to verify.\r" );
		p.Parse( "\x1b[0m\n" );
		p.Parse( "\x1b[35mPassword? ******\r" );
		p.Parse( "\x1b[0m\n" );
		p.Parse( "\r\n" );
		p.Cleanup();

		ASSERT_EQUALS_V( "{play tw menu}{show todays log}{hello rev}{version 3.13}{mbbs}{gold}{registered to the reverend}{courtesy of the reverend}{new character}{enter password}{repeat password}{enter password}", actor.oss.str() );
	}
}

class WhosOnlineActor : public NopActor
{
public:
	virtual void HandleBeginWhosOnline()
	{
		oss << "{begin}";
	}

	virtual void HandleWhosOnline( long rank, const std::string& name, unsigned long corp )
	{
		oss << "{rank=" << rank << " name=\"" << name << "\" corp=" << corp << "}";
	}

	virtual void HandleEndWhosOnline()
	{
		oss << "{end}";
	}

	std::ostringstream oss;
};

TEST( LemonParser, WhosOnlineActor )
{
	WhosOnlineActor actor;
	RPW::TW::LemonParser p;
	p.Inject( actor );
	p.Initialize();

	p.Parse( "\x1b[1;44m                               Who's Playing                                 \r" );
	p.Parse( "\x1b[0m\n" );
	p.Parse( "\r\n" );
	p.Parse( "\x1b[32mCivilian \x1b[1;36mrevAnsi\x1b[0;34m\r" );
	p.Parse( "\x1b[0m\n" );
	p.Parse( "\x1b[32mCivilian \x1b[1;36mmaxAnsi\x1b[0;34m [\x1b[1;36m19\x1b[0;34m]\r" );
	p.Parse( "\x1b[0m\n" );
	p.Parse( "\r\n" );
	p.Parse( "                               Who's Playing                                 \r\n" );
	p.Parse( "\r\n" );
	p.Parse( "Civilian revNoAnsi\r\n" );
	p.Parse( "Civilian maxNoAnsi [3]\r\n" );
	p.Parse( "\r\n" );
	p.Cleanup();

	ASSERT_EQUALS_V( "{begin}{rank=0 name=\"revAnsi\" corp=0}{rank=0 name=\"maxAnsi\" corp=19}{end}{begin}{rank=0 name=\"revNoAnsi\" corp=0}{rank=0 name=\"maxNoAnsi\" corp=3}{end}", actor.oss.str() );
}

TEST( LemonParser, ReEntrant )
{
	WhosOnlineActor actor1;
	RPW::TW::LemonParser p1;
	p1.Inject( actor1 );
	p1.Initialize();

	FigListActor actor2;
	RPW::TW::LemonParser p2;
	p2.Inject( actor2 );
	p2.Initialize();

	FigListActor actor3;
	RPW::TW::LemonParser p3;
	p3.Inject( actor3 );
	p3.Initialize();

	p1.Parse( "\x1b[1;44m                               Who's Playing                                 \r" );
	p2.Parse( "\x1b[2J\r\n" );
	p3.Parse( "\x1b[2J\r\n" );
	p1.Parse( "\x1b[0m\n" );
	p2.Parse( "\x1b[1;44m                 Deployed  Fighter  Scan                   \r" );
	p3.Parse( "\x1b[1;44m                 Deployed  Fighter  Scan                   \r" );
	p1.Parse( "\r\n" );
	p2.Parse( "\x1b[0m\n" );
	p3.Parse( "\x1b[0m\n" );
	p2.Parse( "\r\n" );
	p1.Parse( "\x1b[32mCivilian \x1b[1;36mrevAnsi\x1b[0;34m\r" );
	p3.Parse( "\r\n" );
	p2.Parse( "\x1b[1;36m Sector    Fighters    Personal/Corp    Mode        Tolls  \r" );
	p3.Parse( "\x1b[1;36m Sector    Fighters    Personal/Corp    Mode        Tolls  \r" );
	p1.Parse( "\x1b[0m\n" );
	p2.Parse( "\x1b[0m\n" );
	p3.Parse( "\x1b[0m\n" );
	p2.Parse( "\x1b[35m===========================================================\r" );
	p1.Parse( "\x1b[32mCivilian \x1b[1;36mmaxAnsi\x1b[0;34m [\x1b[1;36m19\x1b[0;34m]\r" );
	p3.Parse( "\x1b[35m===========================================================\r" );
	p2.Parse( "\x1b[0m\n" );
	p3.Parse( "\x1b[0m\n" );
	p1.Parse( "\x1b[0m\n" );
	p2.Parse( "\x1b[1;36m    22\x1b[33m           2\x1b[0;35m         Corp   \x1b[33m   Defensive   \x1b[1m         N/A\r" );
	p3.Parse( "\x1b[1;36m    33\x1b[33m           3\x1b[0;35m         Corp   \x1b[33m   Defensive   \x1b[1m         N/A\r" );
	p2.Parse( "\x1b[0m\n" );
	p1.Parse( "\r\n" );
	p3.Parse( "\x1b[0m\n" );
	p2.Parse( "\x1b[1;36m  2222\x1b[33m           2\x1b[0;35m         Corp   \x1b[33m   Defensive   \x1b[1m         N/A\r" );
	p3.Parse( "\x1b[1;36m  3333\x1b[33m           3\x1b[0;35m         Corp   \x1b[33m   Defensive   \x1b[1m         N/A\r" );
	p1.Parse( "                               Who's Playing                                 \r\n" );
	p2.Parse( "\x1b[0m\n" );
	p3.Parse( "\x1b[0m\n" );
	p2.Parse( "\x1b[1;33m                 4\x1b[36m Total\x1b[33m                                  0\x1b[36m Total\r" );
	p1.Parse( "\r\n" );
	p3.Parse( "\x1b[1;33m                 6\x1b[36m Total\x1b[33m                                  0\x1b[36m Total\r" );
	p2.Parse( "\x1b[0m\n" );
	p3.Parse( "\x1b[0m\n" );
	p1.Parse( "Civilian revNoAnsi\r\n" );
	p2.Parse( "\r\n" );
	p3.Parse( "\r\n" );
	p2.Parse( "\n\x1b[35mCommand [\x1b[1;33mTL\x1b[0;33m=\x1b[1m00:00:00\x1b[0;35m]\x1b[1;37m:\x1b[0;35m[\x1b[1;36m1\x1b[0;35m] (\x1b[1;33m?=Help\x1b[0;35m)? : \r" );
	p1.Parse( "Civilian maxNoAnsi [3]\r\n" );
	p3.Parse( "\n\x1b[35mCommand [\x1b[1;33mTL\x1b[0;33m=\x1b[1m00:00:00\x1b[0;35m]\x1b[1;37m:\x1b[0;35m[\x1b[1;36m1\x1b[0;35m] (\x1b[1;33m?=Help\x1b[0;35m)? : \r" );
	p2.Parse( "\x1b[2J\r\n" );
	p3.Parse( "\x1b[2J\r\n" );
	p1.Parse( "\r\n" );
	p2.Parse( "\x1b[1;44m                 Deployed  Fighter  Scan                   \r" );
	p3.Parse( "\x1b[1;44m                 Deployed  Fighter  Scan                   \r" );
	p2.Parse( "\x1b[0m\n" );
	p3.Parse( "\x1b[0m\n" );
	p2.Parse( "\r\n" );
	p3.Parse( "\r\n" );
	p2.Parse( "\x1b[1;36m Sector    Fighters    Personal/Corp    Mode        Tolls  \r" );
	p3.Parse( "\x1b[1;36m Sector    Fighters    Personal/Corp    Mode        Tolls  \r" );
	p2.Parse( "\x1b[0m\n" );
	p3.Parse( "\x1b[0m\n" );
	p2.Parse( "\x1b[35m===========================================================\r" );
	p3.Parse( "\x1b[35m===========================================================\r" );
	p2.Parse( "\x1b[0m\n" );
	p3.Parse( "\x1b[0m\n" );
	p2.Parse( "\x1b[1;36m    44\x1b[33m           4\x1b[0;35m         Corp   \x1b[33m   Defensive   \x1b[1m         N/A\r" );
	p3.Parse( "\x1b[1;36m    66\x1b[33m           6\x1b[0;35m         Corp   \x1b[33m   Defensive   \x1b[1m         N/A\r" );
	p2.Parse( "\x1b[0m\n" );
	p3.Parse( "\x1b[0m\n" );
	p2.Parse( "\x1b[1;36m  4444\x1b[33m           4\x1b[0;35m         Corp   \x1b[33m   Defensive   \x1b[1m         N/A\r" );
	p3.Parse( "\x1b[1;36m  6666\x1b[33m           6\x1b[0;35m         Corp   \x1b[33m   Defensive   \x1b[1m         N/A\r" );
	p2.Parse( "\x1b[0m\n" );
	p3.Parse( "\x1b[0m\n" );
	p2.Parse( "\r\n" );
	p3.Parse( "\r\n" );
	p2.Parse( "\n\x1b[35mCommand [\x1b[1;33mTL\x1b[0;33m=\x1b[1m00:00:00\x1b[0;35m]\x1b[1;37m:\x1b[0;35m[\x1b[1;36m1\x1b[0;35m] (\x1b[1;33m?=Help\x1b[0;35m)? : \r" );
	p3.Parse( "\n\x1b[35mCommand [\x1b[1;33mTL\x1b[0;33m=\x1b[1m00:00:00\x1b[0;35m]\x1b[1;37m:\x1b[0;35m[\x1b[1;36m1\x1b[0;35m] (\x1b[1;33m?=Help\x1b[0;35m)? : \r" );

	p1.Cleanup();
	p2.Cleanup();
	p3.Cleanup();

	ASSERT_EQUALS_V( "{begin}{rank=0 name=\"revAnsi\" corp=0}{rank=0 name=\"maxAnsi\" corp=19}{end}{begin}{rank=0 name=\"revNoAnsi\" corp=0}{rank=0 name=\"maxNoAnsi\" corp=3}{end}", actor1.oss.str() );
	ASSERT_EQUALS_V( "{figs}{22 2 1 0}{2222 2 1 0}{end}{figs}{44 4 1 0}{4444 4 1 0}{abort}", actor2.oss.str() );
	ASSERT_EQUALS_V( "{figs}{33 3 1 0}{3333 3 1 0}{end}{figs}{66 6 1 0}{6666 6 1 0}{abort}", actor3.oss.str() );
}

class MetricsTestActor : public NopActor
{
public:
	MetricsTestActor()
		: lCommandPrompts( 0 )
		, lComputerPrompts( 0 )
		, lCorporatePrompts( 0 )
	{}
	void HandleCommandPrompt() { lCommandPrompts++; }
	void HandleComputerPrompt() { lComputerPrompts++; }
	void HandleCorporatePrompt() { lCorporatePrompts++; }
	void UpdateTimeRemain( unsigned long seconds ) { ossTimeRemainHistory << seconds << " "; }
	void UpdateCurrentSector( unsigned long sector ) { ossSectorHistory << sector << " "; }
	void HandleCommandChar( char letter ) { ossCommandCharHistory << letter; }
	void HandleComputerChar( char letter ) { ossComputerCharHistory << letter; }
	void HandleCorporateChar( char letter ) { ossCorporateCharHistory << letter; }
	void HandleCommandDigits( unsigned long sector ) { ossCommandDigitsHistory << sector << " "; }

	long lCommandPrompts;
	long lComputerPrompts;
	long lCorporatePrompts;

	std::ostringstream ossSectorHistory;
	std::ostringstream ossTimeRemainHistory;
	std::ostringstream ossCommandCharHistory;
	std::ostringstream ossComputerCharHistory;
	std::ostringstream ossCorporateCharHistory;
	std::ostringstream ossCommandDigitsHistory;
};

TEST( LemonParser, zoc_log_081025_00 )
{
	MetricsTestActor actor;
	RPW::TW::LemonParser p;
	p.Inject( actor );
	p.Initialize();

	std::ifstream ifs( ".\\data\\zoc_log_081025_00.txt", std::ios::binary );
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
	p.Cleanup();

	ASSERT_EQUALS_V( 24, actor.lCommandPrompts );
	ASSERT_EQUALS_V( 14, actor.lComputerPrompts );
	ASSERT_EQUALS_V( 4, actor.lCorporatePrompts );
	ASSERT_EQUALS_V( "1 1 1 1 1 1 1 5666 5666 5666 5666 5666 5666 5666 5666 5666 5666 5666 5666 5666 5666 5666 5666 5666 5666 5666 5666 5666 5666 5666 5666 5666 5666 5666 5666 5666 5666 5666 5666 5666 5666 5666 5666 ", actor.ossSectorHistory.str() );
	ASSERT_EQUALS_V( "0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 ", actor.ossTimeRemainHistory.str() );
	ASSERT_EQUALS_V( "CDSNCCTCTPNCPSSICQ", actor.ossCommandCharHistory.str() );
	ASSERT_EQUALS_V( "LQFQLQLQNQCJQ", actor.ossComputerCharHistory.str() );
	ASSERT_EQUALS_V( "MQDQ", actor.ossCorporateCharHistory.str() );
}


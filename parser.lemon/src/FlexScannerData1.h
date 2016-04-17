/// @file FlexScannerData1.h
/// @author rev

#pragma once

struct FlexScannerData1
{

	char yyBuffer[4096];
	char yyCurrentShipBrief[7];
	char yyHelloName[64];
	char yyRegisteredTo[64];
	char yyCourtesyOf[64];
	char yyPlayerName[64];

	char yyCommandChar;
	char yyComputerChar;
	char yyCorporateChar;

	unsigned long yyAnsiDigits;
	unsigned long yyCurrentSector;
	unsigned long yyTimeRemain;
	unsigned long yyCommandDigits;
	unsigned long yyCurrentTurns;
	unsigned long yyCurrentCreds;
	unsigned long yyCurrentFigs;
	unsigned long yyCurrentShields;
	unsigned long yyCurrentHolds;
	unsigned long yyCurrentFuelOre;
	unsigned long yyCurrentOrganics;
	unsigned long yyCurrentEquipment;
	unsigned long yyCurrentColos;
	unsigned long yyCurrentPhotons;
	unsigned long yyCurrentArmids;
	unsigned long yyCurrentLimpets;
	unsigned long yyCurrentGenesisTorps;
	unsigned long yyCurrentTwarp;
	unsigned long yyCurrentCloaks;
	unsigned long yyCurrentBeacons;
	unsigned long yyCurrentAtomicDetonators;
	unsigned long yyCurrentCorbomite;
	unsigned long yyCurrentEprobes;
	unsigned long yyCurrentMineDisrupters;
	unsigned long yyCurrentPsychicProbe;
	unsigned long yyCurrentPlanetScanner;
	unsigned long yyCurrentLongRangeScanner;
	unsigned long yyCurrentExperience;
	unsigned long yyCurrentCorp;
	unsigned long yyCurrentShipNum;
	unsigned long yyCoursePlotSector;
	unsigned long yyCimSector;
	unsigned long yyCimSectorOut;
	unsigned long yyCimProdPerc;
	unsigned long yyFigListSector;
	unsigned long yyFigListCount;
	unsigned long yyMajorVersion;
	unsigned long yyMinorVersion;
	unsigned long yyCrCount;
	unsigned long yyPlayerCorp;

	long yyCimProdQuan;
	long yyCurrentAlignment;
	long yyPlayerRank;
};

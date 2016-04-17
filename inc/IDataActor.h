/// @file IDataActor.h
/// @author rev

#pragma once

#include <vector>
#include <string>

namespace RPW
{
	namespace TW
	{
		class IDataActor
		{
		public:
			virtual ~IDataActor( void ) {}
			virtual void HandlePlayTradewarsMenu() = 0;
			virtual void HandleShowTodaysLog() = 0;
			virtual void HandleHelloName( const std::string& name ) = 0;
			virtual void HandleVersion( unsigned long major, unsigned long minor ) = 0;
			virtual void HandleMbbs() = 0;
			virtual void HandleGold() = 0;
			virtual void HandleRegisteredTo( const std::string& name ) = 0;
			virtual void HandleCourtesyOf( const std::string& name ) = 0;
			virtual void HandleNewCharacter() = 0;
			virtual void HandleEnterPassword() = 0;
			virtual void HandleRepeatPassword() = 0;
			virtual void HandleBeginWhosOnline() = 0;
			virtual void HandleWhosOnline( long rank, const std::string& name, unsigned long corp ) = 0;
			virtual void HandleEndWhosOnline() = 0;
			virtual void HandleCommandPrompt() = 0;
			virtual void HandleComputerPrompt() = 0;
			virtual void HandleCorporatePrompt() = 0;
			virtual void HandleCommandChar( char letter ) = 0;
			virtual void HandleComputerChar( char letter ) = 0;
			virtual void HandleCorporateChar( char letter ) = 0;
			virtual void HandleCommandDigits( unsigned long sector ) = 0;
			virtual void UpdateTimeRemain( unsigned long seconds ) = 0;
			virtual void BeginQuickStats( unsigned long sector ) = 0;
			virtual void UpdateCurrentSector( unsigned long sector ) = 0;
			virtual void UpdateCurrentTurns( unsigned long turns ) = 0;
			virtual void UpdateCurrentCredits( unsigned long credits ) = 0;
			virtual void UpdateCurrentFighters( unsigned long fighters ) = 0;
			virtual void UpdateCurrentShields( unsigned long shields ) = 0;
			virtual void UpdateCurrentHolds( unsigned long holds ) = 0;
			virtual void UpdateCurrentFuelOre( unsigned long fuelOre ) = 0;
			virtual void UpdateCurrentOrganics( unsigned long organics ) = 0;
			virtual void UpdateCurrentEquipment( unsigned long equipment ) = 0;
			virtual void UpdateCurrentColonists( unsigned long colonists ) = 0;
			virtual void UpdateCurrentPhotonTorpedos( unsigned long photonTorpedos ) = 0;
			virtual void UpdateCurrentArmidMines( unsigned long armidMines ) = 0;
			virtual void UpdateCurrentLimpetMines( unsigned long limpetMines ) = 0;
			virtual void UpdateCurrentGenesisTorpedos( unsigned long genesisTorpedos ) = 0;
			virtual void UpdateCurrentTranswarpDrive( unsigned long transwarpDrive ) = 0;
			virtual void UpdateCurrentCloakingDevices( unsigned long cloakingDevices ) = 0;
			virtual void UpdateCurrentBeacons( unsigned long beacons ) = 0;
			virtual void UpdateCurrentAtomicDetonators( unsigned long atomicDetonators ) = 0;
			virtual void UpdateCurrentCorbomiteDevices( unsigned long corbomiteDevices ) = 0;
			virtual void UpdateCurrentEtherProbes( unsigned long etherProbes ) = 0;
			virtual void UpdateCurrentMineDisrupters( unsigned long mineDisrupters ) = 0;
			virtual void UpdateCurrentPsychicProbe( unsigned long psychicProbe ) = 0;
			virtual void UpdateCurrentPlanetScanner( unsigned long planetScanner ) = 0;
			virtual void UpdateCurrentLongRangeScanner( unsigned long longRangeScanner ) = 0;
			virtual void UpdateCurrentAlignment( long alignment ) = 0;
			virtual void UpdateCurrentExperience( unsigned long experience ) = 0;
			virtual void UpdateCurrentCorporationNumber( unsigned long corporationNumber ) = 0;
			virtual void UpdateCurrentShipNumber( unsigned long shipNumber ) = 0;
			virtual void UpdateCurrentShipBriefName( const std::string& shipBriefName ) = 0;
			virtual void EndQuickStats( const std::string& shipBriefName ) = 0;
			virtual void HandleEnterCim() = 0;
			virtual void HandleExitCim() = 0;
			virtual void HandleCoursePlot( std::vector<unsigned long>& course ) = 0;
			virtual void HandleStartWarpCim() = 0;
			virtual void HandleWarpCimEntry( std::vector<unsigned long>& warpSpec ) = 0;
			virtual void HandleStartPortCim() = 0;
			virtual void HandlePortCimEntry( unsigned long sector, long fuelOreQuan, unsigned long fuelOrePerc, long organicsQuan, unsigned long organicsPerc, long equipmentQuan, unsigned long equipmentPerc ) = 0;
			virtual void HandleBeginFigList() = 0;
			virtual void HandleEndFigList() = 0;
			virtual void HandleAbortFigList() = 0;
			virtual void RecordSectorFigs( unsigned long sector, unsigned long count, unsigned int ownership, unsigned int type ) = 0;
		};
	}
}

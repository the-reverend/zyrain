%name TwParse

%extra_argument { RPW::TW::LemonParser* pParser }

%syntax_error {
		pParser->SyntaxError();
	}

%parse_failure {
		pParser->ParseFailure();
	}

%parse_accept {
		pParser->ParseAccept();
	}

%include {
/** @file parser.y
 *  @brief Lemon parser grammar
 *  @author rev
 *  @see http://www.hwaci.com/sw/lemon/
 */

#include "parser.h"
#include "LemonParser.h"

#define WIN32_LEAN_AND_MEAN
#include "windows.h"

#include <assert.h>
#include <vector>
#include <string>

#define NDEBUG

extern "C"
{
#include "FlexScannerData1.h"
}

void destroy1(std::vector<unsigned long>* v)
{
	if (v) v->clear();
	delete v;
}

void destroy1(std::string* s)
{
	delete s;
}

} /*include*/

/*--------------------------- types ---------------------------------*/
%token_type {unsigned long}
%type command_char { char }
%type computer_char { char }
%type corporate_char { char }
%type current_ship_brief { std::string* }
%type current_alignment { long }
%type course_plot { std::vector<unsigned long>* }
%type cim_ports_prod_quan { long }
%type warp_spec { std::vector<unsigned long>* }
%type hello_name { std::string* }
%type registered_to { std::string* }
%type courtesy_of { std::string* }
%type player_name { std::string* }
%type player_rank { long }

/*--------------------------- destructors ---------------------------*/
/* note: these destructors get called automatically when a type is 
 *       created by the parser, but unused, e.g. due to syntax error.
 *       it is the programmer's responsibility to call the destructors
 *       manually in all other cases, i.e. where access to the type
 *       is gained, whether the type is actually used or not.
 */

%destructor current_ship_brief { destroy1($$); }
%destructor course_plot { destroy1($$); }
%destructor hello_name { destroy1($$); }
%destructor registered_to { destroy1($$); }
%destructor courtesy_of { destroy1($$); }
%destructor player_name { destroy1($$); }

/*--------------------------- precedence ----------------------------*/

/*--------------------------- main grammar --------------------------
 * The grammar consists of many messages.
 */

main ::= zero_or_more_messages .

/*--------------------------- zero_or_more_messages -----------------
 * A set of messages can be empty, or can be followed by another message.
 */

zero_or_more_messages ::= .
zero_or_more_messages ::= zero_or_more_messages one_message .

/*--------------------------- messages ------------------------------*/
one_message ::= PLAY_TRADEWARS_MENU .           { pParser->Actor().HandlePlayTradewarsMenu();                              }
one_message ::= SHOW_TODAYS_LOG .               { pParser->Actor().HandleShowTodaysLog();                                  }
one_message ::= hello_name(A) .                 { pParser->Actor().HandleHelloName(*A); destroy1(A);                        }
one_message ::= major_version(A) minor_version(B) . { pParser->Actor().HandleVersion(A, B);                                }
one_message ::= TRADEWARS_VERSION_MBBS .        { pParser->Actor().HandleMbbs();                                           }
one_message ::= TRADEWARS_VERSION_GOLD .        { pParser->Actor().HandleGold();                                           }
one_message ::= registered_to(A) .              { pParser->Actor().HandleRegisteredTo(*A); destroy1(A);                     }
one_message ::= courtesy_of(A) .                { pParser->Actor().HandleCourtesyOf(*A); destroy1(A);                       }
one_message ::= NEW_CHARACTER .                 { pParser->Actor().HandleNewCharacter();                                   }
one_message ::= ENTER_PASSWORD .                { pParser->Actor().HandleEnterPassword();                                  }
one_message ::= REPEAT_PASSWORD .               { pParser->Actor().HandleRepeatPassword();                                 }
one_message ::= WHOS_PLAYING_BEG .              { pParser->Actor().HandleBeginWhosOnline();                               }
one_message ::= player_rank(A) player_name(B) player_corp(C) . { pParser->Actor().HandleWhosOnline(A, *B, C); destroy1(B); }
one_message ::= player_rank(A) player_name(B) . { pParser->Actor().HandleWhosOnline(A, *B, 0); destroy1(B);                }
one_message ::= WHOS_PLAYING_END .              { pParser->Actor().HandleEndWhosOnline();                                 }
one_message ::= COMMAND_PROMPT .                { pParser->Actor().HandleCommandPrompt();                                  }
one_message ::= COMPUTER_PROMPT .               { pParser->Actor().HandleComputerPrompt();                                 }
one_message ::= CORPORATE_PROMPT .              { pParser->Actor().HandleCorporatePrompt();                                }
one_message ::= command_char(A) .               { pParser->Actor().HandleCommandChar(A);                                   }
one_message ::= computer_char(A) .              { pParser->Actor().HandleComputerChar(A);                                  }
one_message ::= corporate_char(A) .             { pParser->Actor().HandleCorporateChar(A);                                 }
one_message ::= command_digits(A) .             { pParser->Actor().HandleCommandDigits(A);                                 }
one_message ::= time_remain(A) .                { pParser->Actor().UpdateTimeRemain(A);                        }
one_message ::= current_sector(A) .             { pParser->Actor().UpdateCurrentSector(A);                     }
one_message ::= quickstats_sector(A) .          { pParser->Actor().UpdateCurrentSector(A);                     }
one_message ::= current_turns(A) .              { pParser->Actor().UpdateCurrentTurns(A);                      }
one_message ::= current_credits(A) .            { pParser->Actor().UpdateCurrentCredits(A);                    }
one_message ::= current_fighters(A) .           { pParser->Actor().UpdateCurrentFighters(A);                   }
one_message ::= current_shields(A) .            { pParser->Actor().UpdateCurrentShields(A);                    }
one_message ::= current_holds(A) .              { pParser->Actor().UpdateCurrentHolds(A);                      }
one_message ::= current_fuel_ore(A) .           { pParser->Actor().UpdateCurrentFuelOre(A);                    }
one_message ::= current_organics(A) .           { pParser->Actor().UpdateCurrentOrganics(A);                   }
one_message ::= current_equipment(A) .          { pParser->Actor().UpdateCurrentEquipment(A);                  }
one_message ::= current_colos(A) .              { pParser->Actor().UpdateCurrentColonists(A);                  }
one_message ::= current_photons(A) .            { pParser->Actor().UpdateCurrentPhotonTorpedos(A);             }
one_message ::= current_armids(A) .             { pParser->Actor().UpdateCurrentArmidMines(A);                 }
one_message ::= current_limpets(A) .            { pParser->Actor().UpdateCurrentLimpetMines(A);                }
one_message ::= current_genesis_torps(A) .      { pParser->Actor().UpdateCurrentGenesisTorpedos(A);            }
one_message ::= current_twarp(A) .              { pParser->Actor().UpdateCurrentTranswarpDrive(A);             }
one_message ::= current_cloaks(A) .             { pParser->Actor().UpdateCurrentCloakingDevices(A);            }
one_message ::= current_beacons(A) .            { pParser->Actor().UpdateCurrentBeacons(A);                    }
one_message ::= current_atomic_detonators(A) .  { pParser->Actor().UpdateCurrentAtomicDetonators(A);           }
one_message ::= current_corbomite(A) .          { pParser->Actor().UpdateCurrentCorbomiteDevices(A);           }
one_message ::= current_eprobes(A) .            { pParser->Actor().UpdateCurrentEtherProbes(A);                }
one_message ::= current_mine_disruptors(A) .    { pParser->Actor().UpdateCurrentMineDisrupters(A);             }
one_message ::= current_psychic_probe(A) .      { pParser->Actor().UpdateCurrentPsychicProbe(A);               }
one_message ::= current_planet_scanner(A) .     { pParser->Actor().UpdateCurrentPlanetScanner(A);              }
one_message ::= current_long_range_scanner(A) . { pParser->Actor().UpdateCurrentLongRangeScanner(A);           }
one_message ::= current_alignment(A) .          { pParser->Actor().UpdateCurrentAlignment(A);                  }
one_message ::= current_experience(A) .         { pParser->Actor().UpdateCurrentExperience(A);                 }
one_message ::= current_corp(A) .               { pParser->Actor().UpdateCurrentCorporationNumber(A);          }
one_message ::= current_ship_num(A) .           { pParser->Actor().UpdateCurrentShipNumber(A);                 }
one_message ::= current_ship_brief(A) .         { pParser->Actor().UpdateCurrentShipBriefName(*A); destroy1(A); }
one_message ::= CIM_PROMPT .                    { pParser->Actor().HandleEnterCim();                        }
one_message ::= END_CIM .                       { pParser->Actor().HandleExitCim();                         }
one_message ::= course_plot(L) .                { pParser->Actor().HandleCoursePlot(*L); destroy1(L);        }
one_message ::= CIM_WARPS_BEG .                 { pParser->Actor().HandleStartWarpCim();                    }
one_message ::= warp_spec(L) .                  { pParser->Actor().HandleWarpCimEntry(*L); destroy1(L);      }
one_message ::= CIM_PORTS_BEG .                 { pParser->Actor().HandleStartPortCim();                    }
one_message ::= cim_ports_sector(A) cim_ports_prod_quan(B) cim_ports_prod_perc(C) cim_ports_prod_quan(D) cim_ports_prod_perc(E) cim_ports_prod_quan(F) cim_ports_prod_perc(G) .
                                                { pParser->Actor().HandlePortCimEntry(A, B, C, D, E, F, G); }
one_message ::= FIG_LIST_BEG .                  { pParser->Actor().HandleBeginFigList();                    }
one_message ::= fig_list_sector(A) fig_list_count(B) FIG_LIST_PERSONAL FIG_LIST_DEFENSIVE . { pParser->Actor().RecordSectorFigs(A, B, 0, 0); }
one_message ::= fig_list_sector(A) fig_list_count(B) FIG_LIST_PERSONAL FIG_LIST_OFFENSIVE . { pParser->Actor().RecordSectorFigs(A, B, 0, 1); }
one_message ::= fig_list_sector(A) fig_list_count(B) FIG_LIST_PERSONAL FIG_LIST_TOLL .      { pParser->Actor().RecordSectorFigs(A, B, 0, 2); }
one_message ::= fig_list_sector(A) fig_list_count(B) FIG_LIST_CORP FIG_LIST_DEFENSIVE .     { pParser->Actor().RecordSectorFigs(A, B, 1, 0); }
one_message ::= fig_list_sector(A) fig_list_count(B) FIG_LIST_CORP FIG_LIST_OFFENSIVE .     { pParser->Actor().RecordSectorFigs(A, B, 1, 1); }
one_message ::= fig_list_sector(A) fig_list_count(B) FIG_LIST_CORP FIG_LIST_TOLL .          { pParser->Actor().RecordSectorFigs(A, B, 1, 2); }
one_message ::= FIG_LIST_END .                  { pParser->Actor().HandleEndFigList();                      }
one_message ::= FIG_LIST_ABORT .                { pParser->Actor().HandleAbortFigList();                    }

/*----------------------------- pump messages -----------------------*/
one_message ::= PUMP . { /* must have something between brackets or lemon falls through */ }

/*----------------------------- course plot -------------------------*/
course_plot(V) ::= COURSE_PLOT_BEG COURSE_PLOT_ERROR . { V = new std::vector<unsigned long>; }
course_plot(V) ::= COURSE_PLOT_BEG course_plot_sector(A) .
	{
		V = new std::vector<unsigned long>;
		V->push_back(A);
	}
course_plot(V) ::= course_plot(L) COURSE_PLOT_GT course_plot_sector(A) .
	{
		V = L;
		V->push_back(A);
	}

/*----------------------------- cim warpspec ------------------------*/
warp_spec(V) ::= cim_warps_sector(A) .
	{
		V = new std::vector<unsigned long>;
		V->push_back(A);
	}
warp_spec(V) ::= warp_spec(L) CIM_WARPS_SPC cim_warps_out(A) .
	{
		V = L;
		V->push_back(A);
	}

/*----------------------------- terminals ---------------------------*/
time_remain(A)                ::= TIME_REMAIN .              { A = pParser->ScannerData().yyTimeRemain;                        }
current_sector(A)             ::= CURRENT_SECTOR .           { A = pParser->ScannerData().yyCurrentSector;                     }
quickstats_sector(A)          ::= QUICKSTATS_SECTOR .        { A = pParser->ScannerData().yyCurrentSector;                     }
command_char(A)               ::= COMMAND_CHAR .             { A = pParser->ScannerData().yyCommandChar;                       }
computer_char(A)              ::= COMPUTER_CHAR .            { A = pParser->ScannerData().yyComputerChar;                      }
corporate_char(A)             ::= CORPORATE_CHAR .           { A = pParser->ScannerData().yyCorporateChar;                     }
command_digits(A)             ::= COMMAND_DIGITS .           { A = pParser->ScannerData().yyCommandDigits;                     }
current_turns(A)              ::= CURRENT_TURNS .            { A = pParser->ScannerData().yyCurrentTurns;                      }
current_credits(A)            ::= CURRENT_CREDS .            { A = pParser->ScannerData().yyCurrentCreds;                      }
current_fighters(A)           ::= CURRENT_FIGS .             { A = pParser->ScannerData().yyCurrentFigs;                       }
current_shields(A)            ::= CURRENT_SHIELDS .          { A = pParser->ScannerData().yyCurrentShields;                    }
current_holds(A)              ::= CURRENT_HOLDS .            { A = pParser->ScannerData().yyCurrentHolds;                      }
current_fuel_ore(A)           ::= CURRENT_FUELORE .          { A = pParser->ScannerData().yyCurrentFuelOre;                    }
current_organics(A)           ::= CURRENT_ORGANICS .         { A = pParser->ScannerData().yyCurrentOrganics;                   }
current_equipment(A)          ::= CURRENT_EQUIPMENT .        { A = pParser->ScannerData().yyCurrentEquipment;                  }
current_colos(A)              ::= CURRENT_COLOS .            { A = pParser->ScannerData().yyCurrentColos;                      }
current_photons(A)            ::= CURRENT_PHOTONS .          { A = pParser->ScannerData().yyCurrentPhotons;                    }
current_armids(A)             ::= CURRENT_ARMIDS .           { A = pParser->ScannerData().yyCurrentArmids;                     }
current_limpets(A)            ::= CURRENT_LIMPETS .          { A = pParser->ScannerData().yyCurrentLimpets;                    }
current_genesis_torps(A)      ::= CURRENT_GENESISTORPS .     { A = pParser->ScannerData().yyCurrentGenesisTorps;               }
current_twarp(A)              ::= CURRENT_TWARP .            { A = pParser->ScannerData().yyCurrentTwarp;                      }
current_cloaks(A)             ::= CURRENT_CLOAKS .           { A = pParser->ScannerData().yyCurrentCloaks;                     }
current_beacons(A)            ::= CURRENT_BEACONS .          { A = pParser->ScannerData().yyCurrentBeacons;                    }
current_atomic_detonators(A)  ::= CURRENT_ATOMDETONATORS .   { A = pParser->ScannerData().yyCurrentAtomicDetonators;           }
current_corbomite(A)          ::= CURRENT_CORBOMITE .        { A = pParser->ScannerData().yyCurrentCorbomite;                  }
current_eprobes(A)            ::= CURRENT_EPROBES .          { A = pParser->ScannerData().yyCurrentEprobes;                    }
current_mine_disruptors(A)    ::= CURRENT_MINEDISRUPTERS .   { A = pParser->ScannerData().yyCurrentMineDisrupters;             }
current_psychic_probe(A)      ::= CURRENT_PSYCHICPROBE .     { A = pParser->ScannerData().yyCurrentPsychicProbe;               }
current_planet_scanner(A)     ::= CURRENT_PLANETSCANNER .    { A = pParser->ScannerData().yyCurrentPlanetScanner;              }
current_long_range_scanner(A) ::= CURRENT_LONGRANGESCANNER . { A = pParser->ScannerData().yyCurrentLongRangeScanner;           }
current_alignment(A)          ::= CURRENT_ALIGNMENT .        { A = pParser->ScannerData().yyCurrentAlignment;                  }
current_experience(A)         ::= CURRENT_EXPERIENCE .       { A = pParser->ScannerData().yyCurrentExperience;                 }
current_corp(A)               ::= CURRENT_CORP .             { A = pParser->ScannerData().yyCurrentCorp;                       }
current_ship_num(A)           ::= CURRENT_SHIP_NUM .         { A = pParser->ScannerData().yyCurrentShipNum;                    }
current_ship_brief(A)         ::= CURRENT_SHIP_BRIEF .       { A = new std::string(pParser->ScannerData().yyCurrentShipBrief); }
course_plot_sector(A)         ::= COURSE_PLOT_SEC .          { A = pParser->ScannerData().yyCoursePlotSector;                  }
cim_warps_sector(A)           ::= CIM_WARPS_SEC .            { A = pParser->ScannerData().yyCimSector;                         }
cim_warps_out(A)              ::= CIM_WARPS_OUT .            { A = pParser->ScannerData().yyCimSectorOut;                      }
cim_ports_sector(A)           ::= CIM_PORTS_SEC .            { A = pParser->ScannerData().yyCimSector;                         }
cim_ports_prod_quan(A)        ::= CIM_PORT_PROD_QUAN .       { A = pParser->ScannerData().yyCimProdQuan;                       }
cim_ports_prod_perc(A)        ::= CIM_PORT_PROD_PERC .       { A = pParser->ScannerData().yyCimProdPerc;                       }
fig_list_sector(A)            ::= FIG_LIST_SECTOR .          { A = pParser->ScannerData().yyFigListSector;                     }
fig_list_count(A)             ::= FIG_LIST_COUNT .           { A = pParser->ScannerData().yyFigListCount;                      }
hello_name(A)                 ::= HELLO_NAME .               { A = new std::string(pParser->ScannerData().yyHelloName);        }
major_version(A)              ::= TRADEWARS_VERSION_MAJOR  . { A = pParser->ScannerData().yyMajorVersion;                      }
minor_version(A)              ::= TRADEWARS_VERSION_MINOR  . { A = pParser->ScannerData().yyMinorVersion;                      }
player_rank(A)                ::= PLAYER_RANK .              { A = pParser->ScannerData().yyPlayerRank;                        }
player_name(A)                ::= PLAYER_NAME .              { A = new std::string(pParser->ScannerData().yyPlayerName);       }
player_corp(A)                ::= PLAYER_CORP .              { A = pParser->ScannerData().yyPlayerCorp;                        }
registered_to(A)              ::= REGISTERED_TO .            { A = new std::string(pParser->ScannerData().yyRegisteredTo);     }
courtesy_of(A)                ::= COURTESY_OF .              { A = new std::string(pParser->ScannerData().yyCourtesyOf);       }

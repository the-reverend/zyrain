%option reentrant
%option extra-type="struct FlexScannerData1*"
%option interactive
%option always-interactive
%option noyywrap
%option case-insensitive
%option stack
%option 8bit
%option nounistd
%{
/** @file lexer.l
 *  @brief Lexer source
 *  @author rev
 *  @see http://flex.sourceforge.net/
 */

#include "parser.h"
#include "FlexScannerData1.h"

#include <windows.h>

%}

/*================================================================ START CONDITIONS ----*/
%x scCommandPrompt
%x scComputerPrompt
%x scCorporatePrompt
%x scQuickStats
%x scQuickStatsBrief
%x scCimMode
%x scCimCoursePlot
%x scCoursePlot1
%x scCoursePlot2
%x scCimWarps
%x scCimExpl
%x scCimPorts
%x scFigList
%x scHelloName
%x scTradeWarsVersion
%x scRegisteredTo
%x scCourtesyOf
%x scWhosPlaying
%x scWhosPlaying1
%x scWhosPlaying2

/*=========================================================================== RULES ====*/
%%
 /*------------------------------------------------------------- Backspace Handling ----*/
<*>[^\x08]{2,}\x08 {
		int i;
		/* Copy yyget_text(yyscanner) because unput() trashes yyget_text(yyscanner) */
		memcpy_s(yyextra->yyBuffer, sizeof(yyextra->yyBuffer), yyget_text(yyscanner), yyget_leng(yyscanner));
		*(yyextra->yyBuffer + yyget_leng(yyscanner)) = 0;
		for (i = yyget_leng(yyscanner) - 3; i >= 0; i--)
		{
			/* unput each character except the backspace pair 
			 * so that they can be parsed again
			 */
			unput(yyextra->yyBuffer[i]);
		}
	}

 /*----------------------------------------------------- Trade Wars 2002 TISHX Menu ----*/
<INITIAL>(\x1b\[36m)?-[ ]Play[ ]Trade[ ]Wars[ ]2002 {
		return PLAY_TRADEWARS_MENU;
	}

 /*-------------------------------------------------- Trade Wars 2002 Game Settings ----*/
 /*---------------------------------------------------- Trade Wars 2002 High Scores ----*/
 /*-------------------------------------------------- Trade Wars 2002 Game Settings ----*/
 /*-------------------------------------------------- Trade Wars 2002 Game Settings ----*/
 /*-------------------------------------------------------------------- Star Screen ----*/
 /*------------------------------------------------------------------------ [Pause] ----*/
 /*----------------------------------- [Pause] - [Press Space or Enter to continue] ----*/
 /*---------------------------------------------------- Show today's log? (Y/N) [N] ----*/
<INITIAL>^(\x1b\[32m)?Show[ ]today's[ ]log\? {
		return SHOW_TODAYS_LOG;
	}

 /*--------------------------------------------- Include time/date stamp? (Y/N) [N] ----*/
 /*------------------------------------------------------------------ Daily Journal ----*/
 /*-------------------------------------------------------- Hello NAME, welcome to: ----*/
<INITIAL>^(\x1b\[32m)?[ ]{7}Hello[ ] {
		yy_push_state(scHelloName, yyscanner);
	}
<scHelloName>[^\x1b]+ {
		yy_pop_state(yyscanner);
		strcpy_s(yyextra->yyHelloName, sizeof(yyextra->yyHelloName), yyget_text(yyscanner));
		return HELLO_NAME;
	}
<scHelloName>\x1b\[1;36m[^\x1b]+ {
		yy_pop_state(yyscanner);
		strcpy_s(yyextra->yyHelloName, sizeof(yyextra->yyHelloName), yyget_text(yyscanner)+7);
		return HELLO_NAME;
	}
<scHelloName>\r {
		yy_pop_state(yyscanner);
	}

 /*--------------------------------------------- Trade Wars 2002 ver X.YY MBBS Gold ----*/
<INITIAL>^[ ]{11}(\x1b\[1;33m)?Trade[ ]Wars[ ]2002 {
		yy_push_state(scTradeWarsVersion, yyscanner);
	}
<scTradeWarsVersion>[0-9]/\.[0-9]+ {
		yyextra->yyMajorVersion = strtoul(yyget_text(yyscanner), NULL, 10);
		return TRADEWARS_VERSION_MAJOR;
	}
<scTradeWarsVersion>\.[0-9]+ {
		yyextra->yyMinorVersion = strtoul(yyget_text(yyscanner)+1, NULL, 10);
		return TRADEWARS_VERSION_MINOR;
	}
<scTradeWarsVersion>MBBS {
		return TRADEWARS_VERSION_MBBS;
	}
<scTradeWarsVersion>Gold {
		return TRADEWARS_VERSION_GOLD;
	}
<scTradeWarsVersion>\r {
		yy_pop_state(yyscanner);
	}
	
 /*----------------------------------- This copy of TW2002 is Registered to SOMEBBS ----*/
<INITIAL>^(\x1b\[32m)?This[ ]copy[ ]of[ ]TW2002[ ]is[ ] {
		yy_push_state(scRegisteredTo, yyscanner);
	}
<scRegisteredTo>\x1b\[1;36m[^\r]+/\r { /* ansi version */
 		yy_pop_state(yyscanner);
		strcpy_s(yyextra->yyRegisteredTo, sizeof(yyextra->yyRegisteredTo), yyget_text(yyscanner)+7);
		return REGISTERED_TO;
	}
<scRegisteredTo>Registered[ ]to[ ][^\r]+/\r { /* no-ansi version */
		yy_pop_state(yyscanner);
		strcpy_s(yyextra->yyRegisteredTo, sizeof(yyextra->yyRegisteredTo), yyget_text(yyscanner)+14);
		return REGISTERED_TO;
	}
<scRegisteredTo>\r {
		yy_pop_state(yyscanner);
	}
 /*---------------------------------- and is being brought to you courtesy of SYSOP ----*/
<INITIAL>^(\x1b\[32m)?and[ ]is[ ]being[ ]brought[ ]to[ ]you[ ]courtesy[ ]of[ ] {
		yy_push_state(scCourtesyOf, yyscanner);
	}
<scCourtesyOf>\x1b\[1;33m[^\r]*/\r {
		yy_pop_state(yyscanner);
		strcpy_s(yyextra->yyCourtesyOf, sizeof(yyextra->yyCourtesyOf), yyget_text(yyscanner)+7);
		return COURTESY_OF;
	}
<scCourtesyOf>[^\r]+/\r {
		yy_pop_state(yyscanner);
		strcpy_s(yyextra->yyCourtesyOf, sizeof(yyextra->yyCourtesyOf), yyget_text(yyscanner));
		return COURTESY_OF;
	}
<scCourtesyOf>\r {
		yy_pop_state(yyscanner);
	}

 /*-------------------------- Would you like to start a new character in this game? ----*/
<INITIAL>^(\x1b\[1;44m)?Would[ ]you[ ]like[ ]to[ ]start[ ]a[ ]new[ ]character[ ]in[ ]this[ ]game\?[ ][ ]\(Type {
		return NEW_CHARACTER;
	}

 /*---------------------------------------------------------------------- Password? ----*/
<INITIAL>^(\x1b\[35m)?Password\? {
		return ENTER_PASSWORD;
	}

 /*----------------------------------------------------- Repeat password to verify. ----*/
<INITIAL>^(\x1b\[1;33m)?Repeat[ ]password[ ]to[ ]verify\. {
		return REPEAT_PASSWORD;
	}

 /*------------------------------------------------------------------ Who's Playing ----*/
<INITIAL>^(\x1b\[1;44m)?[ ]{31}Who\'s[ ]Playing[ ]{33}/\r {
		yy_push_state(scWhosPlaying, yyscanner);
		yyextra->yyCrCount = 0;
		return WHOS_PLAYING_BEG;
	}
<scWhosPlaying>^(\x1b\[1;31m)?Nuisance[ ]3rd[ ]Class[ ] {
		yy_push_state(scWhosPlaying1, yyscanner);
		yyextra->yyPlayerRank = -1;
		return PLAYER_RANK;
	}
<scWhosPlaying>^(\x1b\[1;31m)?Nuisance[ ]2nd[ ]Class[ ] {
		yy_push_state(scWhosPlaying1, yyscanner);
		yyextra->yyPlayerRank = -2;
		return PLAYER_RANK;
	}
<scWhosPlaying>^(\x1b\[1;31m)?Nuisance[ ]1st[ ]Class[ ] {
		yy_push_state(scWhosPlaying1, yyscanner);
		yyextra->yyPlayerRank = -3;
		return PLAYER_RANK;
	}
<scWhosPlaying>^(\x1b\[1;31m)?Menace[ ]3rd[ ]Class[ ] {
		yy_push_state(scWhosPlaying1, yyscanner);
		yyextra->yyPlayerRank = -4;
		return PLAYER_RANK;
	}
<scWhosPlaying>^(\x1b\[1;31m)?Menace[ ]2nd[ ]Class[ ] {
		yy_push_state(scWhosPlaying1, yyscanner);
		yyextra->yyPlayerRank = -5;
		return PLAYER_RANK;
	}
<scWhosPlaying>^(\x1b\[1;31m)?Menace[ ]1st[ ]Class[ ] {
		yy_push_state(scWhosPlaying1, yyscanner);
		yyextra->yyPlayerRank = -6;
		return PLAYER_RANK;
	}
<scWhosPlaying>^(\x1b\[1;31m)?Smuggler[ ]3rd[ ]Class[ ] {
		yy_push_state(scWhosPlaying1, yyscanner);
		yyextra->yyPlayerRank = -7;
		return PLAYER_RANK;
	}
<scWhosPlaying>^(\x1b\[1;31m)?Smuggler[ ]2nd[ ]Class[ ] {
		yy_push_state(scWhosPlaying1, yyscanner);
		yyextra->yyPlayerRank = -8;
		return PLAYER_RANK;
	}
<scWhosPlaying>^(\x1b\[1;31m)?Smuggler[ ]1st[ ]Class[ ] {
		yy_push_state(scWhosPlaying1, yyscanner);
		yyextra->yyPlayerRank = -9;
		return PLAYER_RANK;
	}
<scWhosPlaying>^(\x1b\[1;31m)?Smuggler[ ]Savant[ ] {
		yy_push_state(scWhosPlaying1, yyscanner);
		yyextra->yyPlayerRank = -10;
		return PLAYER_RANK;
	}
<scWhosPlaying>^(\x1b\[1;31m)?Robber[ ] {
		yy_push_state(scWhosPlaying1, yyscanner);
		yyextra->yyPlayerRank = -11;
		return PLAYER_RANK;
	}
<scWhosPlaying>^(\x1b\[1;31m)?Terrorist[ ] {
		yy_push_state(scWhosPlaying1, yyscanner);
		yyextra->yyPlayerRank = -12;
		return PLAYER_RANK;
	}
<scWhosPlaying>^(\x1b\[1;31m)?Pirate[ ] {
		yy_push_state(scWhosPlaying1, yyscanner);
		yyextra->yyPlayerRank = -13;
		return PLAYER_RANK;
	}
<scWhosPlaying>^(\x1b\[1;31m)?Infamous[ ]Pirate[ ] {
		yy_push_state(scWhosPlaying1, yyscanner);
		yyextra->yyPlayerRank = -14;
		return PLAYER_RANK;
	}
<scWhosPlaying>^(\x1b\[1;31m)?Notorious[ ]Pirate[ ] {
		yy_push_state(scWhosPlaying1, yyscanner);
		yyextra->yyPlayerRank = -15;
		return PLAYER_RANK;
	}
<scWhosPlaying>^(\x1b\[1;31m)?Dread[ ]Pirate[ ] {
		yy_push_state(scWhosPlaying1, yyscanner);
		yyextra->yyPlayerRank = -16;
		return PLAYER_RANK;
	}
<scWhosPlaying>^(\x1b\[1;31m)?Galactic[ ]Scourge[ ] {
		yy_push_state(scWhosPlaying1, yyscanner);
		yyextra->yyPlayerRank = -17;
		return PLAYER_RANK;
	}
<scWhosPlaying>^(\x1b\[1;31m)?Enemy[ ]of[ ]the[ ]State[ ] {
		yy_push_state(scWhosPlaying1, yyscanner);
		yyextra->yyPlayerRank = -18;
		return PLAYER_RANK;
	}
<scWhosPlaying>^(\x1b\[1;31m)?Enemy[ ]of[ ]the[ ]People[ ] {
		yy_push_state(scWhosPlaying1, yyscanner);
		yyextra->yyPlayerRank = -19;
		return PLAYER_RANK;
	}
<scWhosPlaying>^(\x1b\[1;31m)?Enemy[ ]of[ ]Humankind[ ] {
		yy_push_state(scWhosPlaying1, yyscanner);
		yyextra->yyPlayerRank = -20;
		return PLAYER_RANK;
	}
<scWhosPlaying>^(\x1b\[1;31m)?Heinous[ ]Overlord[ ] {
		yy_push_state(scWhosPlaying1, yyscanner);
		yyextra->yyPlayerRank = -21;
		return PLAYER_RANK;
	}
<scWhosPlaying>^(\x1b\[1;31m)?Prime[ ]Evil[ ] {
		yy_push_state(scWhosPlaying1, yyscanner);
		yyextra->yyPlayerRank = -22;
		return PLAYER_RANK;
	}
<scWhosPlaying>^(\x1b\[32m)?Civilian[ ] {
		yy_push_state(scWhosPlaying1, yyscanner);
		yyextra->yyPlayerRank = 0;
		return PLAYER_RANK;
	}
<scWhosPlaying>^(\x1b\[32m)?Private[ ] {
		yy_push_state(scWhosPlaying1, yyscanner);
		yyextra->yyPlayerRank = 1;
		return PLAYER_RANK;
	}
<scWhosPlaying>^(\x1b\[32m)?Private[ ]1st[ ]Class[ ] {
		yy_push_state(scWhosPlaying1, yyscanner);
		yyextra->yyPlayerRank = 2;
		return PLAYER_RANK;
	}
<scWhosPlaying>^(\x1b\[32m)?Lance[ ]Corporal[ ] {
		yy_push_state(scWhosPlaying1, yyscanner);
		yyextra->yyPlayerRank = 3;
		return PLAYER_RANK;
	}
<scWhosPlaying>^(\x1b\[32m)?Corporal[ ] {
		yy_push_state(scWhosPlaying1, yyscanner);
		yyextra->yyPlayerRank = 4;
		return PLAYER_RANK;
	}
<scWhosPlaying>^(\x1b\[32m)?Sergeant[ ] {
		yy_push_state(scWhosPlaying1, yyscanner);
		yyextra->yyPlayerRank = 5;
		return PLAYER_RANK;
	}
<scWhosPlaying>^(\x1b\[32m)?Staff[ ]Sergeant[ ] {
		yy_push_state(scWhosPlaying1, yyscanner);
		yyextra->yyPlayerRank = 6;
		return PLAYER_RANK;
	}
<scWhosPlaying>^(\x1b\[32m)?Gunnery[ ]Sergeant[ ] {
		yy_push_state(scWhosPlaying1, yyscanner);
		yyextra->yyPlayerRank = 7;
		return PLAYER_RANK;
	}
<scWhosPlaying>^(\x1b\[32m)?1st[ ]Sergeant[ ] {
		yy_push_state(scWhosPlaying1, yyscanner);
		yyextra->yyPlayerRank = 8;
		return PLAYER_RANK;
	}
<scWhosPlaying>^(\x1b\[32m)?Sergeant[ ]Major[ ] {
		yy_push_state(scWhosPlaying1, yyscanner);
		yyextra->yyPlayerRank = 9;
		return PLAYER_RANK;
	}
<scWhosPlaying>^(\x1b\[32m)?Warrant[ ]Officer[ ] {
		yy_push_state(scWhosPlaying1, yyscanner);
		yyextra->yyPlayerRank = 10;
		return PLAYER_RANK;
	}
<scWhosPlaying>^(\x1b\[32m)?Chief[ ]Warrant[ ]Officer[ ] {
		yy_push_state(scWhosPlaying1, yyscanner);
		yyextra->yyPlayerRank = 11;
		return PLAYER_RANK;
	}
<scWhosPlaying>^(\x1b\[32m)?Ensign[ ] {
		yy_push_state(scWhosPlaying1, yyscanner);
		yyextra->yyPlayerRank = 12;
		return PLAYER_RANK;
	}
<scWhosPlaying>^(\x1b\[32m)?Lieutenant[ ]J\.G\.[ ] {
		yy_push_state(scWhosPlaying1, yyscanner);
		yyextra->yyPlayerRank = 13;
		return PLAYER_RANK;
	}
<scWhosPlaying>^(\x1b\[32m)?Lieutenant[ ] {
		yy_push_state(scWhosPlaying1, yyscanner);
		yyextra->yyPlayerRank = 14;
		return PLAYER_RANK;
	}
<scWhosPlaying>^(\x1b\[32m)?Lieutenant[ ]Commander[ ] {
		yy_push_state(scWhosPlaying1, yyscanner);
		yyextra->yyPlayerRank = 15;
		return PLAYER_RANK;
	}
<scWhosPlaying>^(\x1b\[32m)?Commander[ ] {
		yy_push_state(scWhosPlaying1, yyscanner);
		yyextra->yyPlayerRank = 16;
		return PLAYER_RANK;
	}
<scWhosPlaying>^(\x1b\[32m)?Captain[ ] {
		yy_push_state(scWhosPlaying1, yyscanner);
		yyextra->yyPlayerRank = 17;
		return PLAYER_RANK;
	}
<scWhosPlaying>^(\x1b\[32m)?Commodore[ ] {
		yy_push_state(scWhosPlaying1, yyscanner);
		yyextra->yyPlayerRank = 18;
		return PLAYER_RANK;
	}
<scWhosPlaying>^(\x1b\[32m)?Rear[ ]Admiral[ ] {
		yy_push_state(scWhosPlaying1, yyscanner);
		yyextra->yyPlayerRank = 19;
		return PLAYER_RANK;
	}
<scWhosPlaying>^(\x1b\[32m)?Vice[ ]Admiral[ ] {
		yy_push_state(scWhosPlaying1, yyscanner);
		yyextra->yyPlayerRank = 20;
		return PLAYER_RANK;
	}
<scWhosPlaying>^(\x1b\[32m)?Admiral[ ] {
		yy_push_state(scWhosPlaying1, yyscanner);
		yyextra->yyPlayerRank = 21;
		return PLAYER_RANK;
	}
<scWhosPlaying>^(\x1b\[32m)?Fleet[ ]Admiral[ ] {
		yy_push_state(scWhosPlaying1, yyscanner);
		yyextra->yyPlayerRank = 22;
		return PLAYER_RANK;
	}
<scWhosPlaying>^(\x1b\[1;34m)?Ambassador[ ] {
		yy_push_state(scWhosPlaying1, yyscanner);
		yyextra->yyPlayerRank = 23;
		return PLAYER_RANK;
	}
<scWhosPlaying1>\x1b\[1;36m[^\x1b]+ {
		strcpy_s(yyextra->yyPlayerName, sizeof(yyextra->yyPlayerName), yyget_text(yyscanner)+7);
		BEGIN(scWhosPlaying2);
		return PLAYER_NAME;
	}
<scWhosPlaying1>\x1b\[36m[^\x1b]+ {
		strcpy_s(yyextra->yyPlayerName, sizeof(yyextra->yyPlayerName), yyget_text(yyscanner)+5);
		BEGIN(scWhosPlaying2);
		return PLAYER_NAME;
	}
<scWhosPlaying1>[^\r\x1b]+[ ]\[[0-9]+\]/\r {
		int len = strrchr(yyget_text(yyscanner), ' ') - yyget_text(yyscanner);
		memcpy_s(yyextra->yyPlayerName, len, yyget_text(yyscanner), len);
		yyextra->yyPlayerName[len]=0;
		yyless(len);
		BEGIN(scWhosPlaying2);
		return PLAYER_NAME;
	}
<scWhosPlaying1>[^\r\x1b]+/\r {
		strcpy_s(yyextra->yyPlayerName, sizeof(yyextra->yyPlayerName), yyget_text(yyscanner));
		BEGIN(scWhosPlaying2);
		return PLAYER_NAME;
	}
<scWhosPlaying2>\x1b\[1;36m[0-9]+ {
		yyextra->yyPlayerCorp = strtoul(yyget_text(yyscanner)+7, NULL, 10);
		return PLAYER_CORP;
	}
<scWhosPlaying2>\[[0-9]+\]/\r {
		yyextra->yyPlayerCorp = strtoul(yyget_text(yyscanner)+1, NULL, 10);
		return PLAYER_CORP;
	}
<scWhosPlaying1,scWhosPlaying2>\r {
		yy_pop_state(yyscanner);
	}
<scWhosPlaying>\r {
		yyextra->yyCrCount++;
		if (yyextra->yyCrCount >= 3)
		{
			yy_pop_state(yyscanner);
			return WHOS_PLAYING_END;
		}
	}
<scWhosPlaying>^(\x1b\[35m)?\[Pause\] {
		yy_pop_state(yyscanner);
		yyless(0);
		yy_set_bol(1);
		return WHOS_PLAYING_END;
	}
<scWhosPlaying>^(\x1b\[32m)?Notice:[ ]If[ ]you[ ]don't[ ]play[ ]for[ ] {
		yy_pop_state(yyscanner);
		return WHOS_PLAYING_END;
	}

 /*--------------------------------- or would you rather use your BBS name of NAME? ----*/
 /*--------------------------------------------- Use (N)ew Name or (B)BS Name [B] ? ----*/
 /*------------------------------------------------- What Alias do you want to use? ----*/
 /*-------------------------------------------------- NAME is what y ou want? (Y/N) ----*/
 /*-------------------------------- What do you want to name your ship? (30 letter) ----*/
 /*----------------------------------------------------- SHIPNAME is what you want? ----*/
 /*----------------------------------------------------------------- Sector Display ----*/
 /*----------------------------------------------------------------- Command Prompt ----*/
<INITIAL>^(\x1b\[35m)?Command[ ]\[ {
		yy_push_state(scCommandPrompt, yyscanner);
		return COMMAND_PROMPT;
	}
<scCommandPrompt,scComputerPrompt,scCorporatePrompt>[0-9]{2}:[0-9]{2}:[0-9]{2} {
		yyextra->yyTimeRemain = strtoul(yyget_text(yyscanner), NULL, 10) * 3600 + strtoul(yyget_text(yyscanner)+3, NULL, 10) * 60 + strtoul(yyget_text(yyscanner)+6, NULL, 10);
		return TIME_REMAIN;
	}
<scCommandPrompt,scComputerPrompt,scCorporatePrompt>[0-9]+ {
		yyextra->yyCurrentSector = strtoul(yyget_text(yyscanner), NULL, 10);
		return CURRENT_SECTOR;
	}
<scCommandPrompt,scComputerPrompt,scCorporatePrompt>\)\? {
		yyless(1);
		return PUMP;
	}
<scCommandPrompt>\?[ ]:[ ][A-Z] {
		yyextra->yyCommandChar = *(yyget_text(yyscanner) + yyget_leng(yyscanner) -1);
		yy_pop_state(yyscanner);
		return COMMAND_CHAR;
	}
<scCommandPrompt>\?[ ]:[ ][0-9]+ {
		yyextra->yyCommandDigits = strtoul(yyget_text(yyscanner) + 4, NULL, 10);
		yy_pop_state(yyscanner);
		return COMMAND_DIGITS;
	}
<scCommandPrompt>\?[ ]:[ ] {
		yy_pop_state(yyscanner);
	}

 /*---------------------------------------------------------------- Computer Prompt ----*/
<INITIAL>^(\x1b\[35m)?Computer[ ]command[ ]\[ {
		yy_push_state(scComputerPrompt, yyscanner);
		return COMPUTER_PROMPT;
	}
<scComputerPrompt>\?[ ][A-Z] {
		yyextra->yyComputerChar = *(yyget_text(yyscanner) + yyget_leng(yyscanner) - 1);
		yy_pop_state(yyscanner);
		return COMPUTER_CHAR;
	}
<scComputerPrompt,scCorporatePrompt>\?[ ] {
		yy_pop_state(yyscanner);
	}

 /*--------------------------------------------------------------- Corporate Prompt ----*/
<INITIAL>^(\x1b\[35m)?Corporate[ ]command[ ]\[ {
		yy_push_state(scCorporatePrompt, yyscanner);
		return CORPORATE_PROMPT;
	}
<scCorporatePrompt>\?[ ][A-Z] {
		yyextra->yyCorporateChar = *(yyget_text(yyscanner) + yyget_leng(yyscanner) - 1);
		yy_pop_state(yyscanner);
		return CORPORATE_CHAR;
	}

 /*------------------------------------------------------------------ Planet Prompt ----*/
 /*----------------------------------------------------------------- Citadel Prompt ----*/
 /*---------------------------------------------------------------- Stardock Prompt ----*/
 /*-------------------------------------------------------------------- Bank Prompt ----*/
 /*---------------------------------------------------------------- Shipyard Prompt ----*/
 /*---------------------------------------------------------------- Hardware Prompt ----*/
 /*-------------------------------------------------------------------- Course Plot ----*/
<INITIAL>^(\x1b\[32m)?The[ ]shortest[ ]path[ ] {
		yy_push_state(scCoursePlot1, yyscanner);
	}
<scCoursePlot1>[0-9]+[ ]hops {
	}
<scCoursePlot1>[0-9]+[ ]turns {
	}
<scCoursePlot1>(\x1b\[0;32m)?[ ]is {
		BEGIN(scCoursePlot2);
		return COURSE_PLOT_BEG;
	}
<scCoursePlot2>[0-9]+ {
		yyextra->yyCoursePlotSector = strtoul(yyget_text(yyscanner), NULL, 10);
		return COURSE_PLOT_SEC;
	}
<scCoursePlot2>[0-9]+\)?/\r {
		yyextra->yyCoursePlotSector = strtoul(yyget_text(yyscanner), NULL, 10);
		yy_pop_state(yyscanner);
		return COURSE_PLOT_SEC;
	}
<scCoursePlot2>> {
		return COURSE_PLOT_GT;
	}
	
 /*----------------------------------------------------------------------- CIM Mode ----*/
<*>^:[ ] {
		switch (YY_START)
		{
			case scCimMode       :                           break;
			case scCimWarps      : /* fall through */
			case scCimExpl       : /* fall through */
			case scCimPorts      : /* fall through */
			case scCimCoursePlot : yy_pop_state(yyscanner);           break;
			default              : yy_push_state(scCimMode, yyscanner); break;
		}
		return CIM_PROMPT;
	}
<scCimMode,scCimWarps,scCimExpl,scCimPorts,scCimCoursePlot>:[ ]ENDINTERROG {
		switch (YY_START)
		{
			case scCimWarps      : /* fall through */
			case scCimExpl       : /* fall through */
			case scCimPorts      : /* fall through */
			case scCimCoursePlot : yy_pop_state(yyscanner); /* fall through */
			case scCimMode       : yy_pop_state(yyscanner); break;
			default              :                 break;
		}
		return END_CIM;
	}
	
 /*---------------------------------------------------------------- CIM Course Plot ----*/
<scCimMode>^[ ][ ]TO[ ]>[ ][0-9]+ {
		yy_push_state(scCimCoursePlot, yyscanner);
		return COURSE_PLOT_BEG;
	}
<scCimCoursePlot>[0-9]+ {
		yyextra->yyCoursePlotSector = strtoul(yyget_text(yyscanner), NULL, 10);
		return COURSE_PLOT_SEC;
	}
<scCimCoursePlot>\*\*\*[ ]Error[ ]-[ ]No.*/\r {
		return COURSE_PLOT_ERROR;
	}
<scCimCoursePlot>> {
		return COURSE_PLOT_GT;
	}
	
 /*--------------------------------------------------------------- CIM Sector Warps ----*/
<scCimMode>^[ ]*[0-9]+([ ]+[0-9]+){7,}/\r {
		yy_push_state(scCimExpl, yyscanner);
	}
<scCimMode>^[ ]*[0-9]+([ ]+[0-9]+){0,6}/\r {
		yyless(0);
		yy_set_bol(1);
		yy_push_state(scCimWarps, yyscanner);
		return CIM_WARPS_BEG;
	}
<scCimWarps>^[ ]*[0-9]+ {
		yyextra->yyCimSector = strtoul(yyget_text(yyscanner), NULL, 10);
		return CIM_WARPS_SEC;
	}
<scCimWarps>[ ]+ {
		return CIM_WARPS_SPC;
	}
<scCimWarps>[0-9]+ {
		yyextra->yyCimSectorOut = strtoul(yyget_text(yyscanner), NULL, 10);
		return CIM_WARPS_OUT;
	}

 /*---------------------------------------------------------------- CIM Port Report ----*/
<scCimMode>^[ 0-9\-]+% {
		yyless(0);
		yy_set_bol(1);
		yy_push_state(scCimPorts, yyscanner);
		return CIM_PORTS_BEG;
	}
<scCimPorts>^[ ]*[0-9]+[ ]/[ -] {
		yyextra->yyCimSector = strtoul(yyget_text(yyscanner), NULL, 10);
		return CIM_PORTS_SEC;
	}
<scCimPorts>[ ]+[0-9]+[ ]/[ ]*[0-9]+% {
		yyextra->yyCimProdQuan = strtol(yyget_text(yyscanner), NULL, 10);
		return CIM_PORT_PROD_QUAN;
	}
<scCimPorts>-[ ]*[0-9]+[ ]/[ ]*[0-9]+% {
		yyextra->yyCimProdQuan = - strtol(yyget_text(yyscanner)+1, NULL, 10);
		return CIM_PORT_PROD_QUAN;
	}
<scCimPorts>[0-9]+% {
		yyextra->yyCimProdPerc = strtoul(yyget_text(yyscanner), NULL, 10);
		return CIM_PORT_PROD_PERC;
	}

 /*----------------------------------------------------------------------- Fig List ----*/
<INITIAL>^(\x1b\[1;36m)?[ ]Sector[ ]{4}Fighters[ ]{4}Personal\/Corp[ ]{4}Mode[ ]{8}Tolls[ ]{2}/\r {
		yy_push_state(scFigList, yyscanner);
		return FIG_LIST_BEG;
	}
<scFigList>^(\x1b\[1;33m)?[ 0-9TMB,]{18} {
		/* toll amount */
	}
<scFigList>^(\x1b\[1;36m)?[ ]+[0-9]+ {
		yyextra->yyFigListSector = strtoul(yyget_text(yyscanner) + yyget_leng(yyscanner) - 5, NULL, 10);
		return FIG_LIST_SECTOR;
	}
<scFigList>(\x1b\[33m)?[0-9 ]{11}[0-9TMB] {
		switch (*(yyget_text(yyscanner) + yyget_leng(yyscanner) - 1))
		{
			case 'T': yyextra->yyFigListCount =       1000 * strtoul(yyget_text(yyscanner) + yyget_leng(yyscanner) - 12, NULL, 10); break;
			case 'M': yyextra->yyFigListCount =    1000000 * strtoul(yyget_text(yyscanner) + yyget_leng(yyscanner) - 12, NULL, 10); break;
			case 'B': yyextra->yyFigListCount = 1000000000 * strtoul(yyget_text(yyscanner) + yyget_leng(yyscanner) - 12, NULL, 10); break;
			default : yyextra->yyFigListCount =              strtoul(yyget_text(yyscanner) + yyget_leng(yyscanner) - 12, NULL, 10); break;
		}
		return FIG_LIST_COUNT;
	}
<scFigList>[ ]Corp[ ] {
		return FIG_LIST_CORP;
	}
<scFigList>[ ]Personal[ ] {
		return FIG_LIST_PERSONAL;
	}
<scFigList>[ ]Defensive[ ] {
		return FIG_LIST_DEFENSIVE;
	}
<scFigList>[ ]Toll[ ] {
		return FIG_LIST_TOLL;
	}
<scFigList>[ ]Offensive[ ] {
		return FIG_LIST_OFFENSIVE;
	}
<scFigList>[ ]Total {
		yy_pop_state(yyscanner);
		return FIG_LIST_END;
	}
<scFigList>^(\x1b\[35m)?Command[ ]\[ {
		yyless(0);
		yy_set_bol(1);
		yy_pop_state(yyscanner);
		return FIG_LIST_ABORT;
	}

 /*---------------------------------------------------------------------- Mine List ----*/
 /*-------------------------------------------------------------------- Limpet List ----*/
 /*---------------------------------------------------------------------- Void List ----*/
 /*------------------------------------------------------------------ Explored List ----*/
 /*---------------------------------------------------------------- Unexplored List ----*/
 /*------------------------------------------------------------------ Eprobe Output ----*/
 /*---------------------------------------------------------------------- Ship Info ----*/
 /*------------------------------------------------------------------- Ship Catalog ----*/
 /*---------------------------------------------------------------------- Ship Semi ----*/
 /*----------------------------------------------------------------------- V Screen ----*/
 /*-------------------------------------------------------------------- Planet List ----*/
 /*---------------------------------------------------------------------- Ship List ----*/
 /*--------------------------------------------------------------------- Xport List ----*/
 /*------------------------------------------------------------ Planet Scanner List ----*/
 /*----------------------------------------------------------------- Planet Display ----*/
 /*------------------------------------------------------------------- Port Display ----*/
 /*--------------------------------------------------------------------- QuickStats ----*/
<*>^\x1b\[30;47m[ ]Sect[ ][0-9]+\xb3 {
		yy_push_state(scQuickStats, yyscanner);
		yyextra->yyCurrentSector = strtoul(yyget_text(yyscanner)+14, NULL, 10);
		return QUICKSTATS_SECTOR;
	}
<*>^[ ]Sect[ ][0-9]+\xb3 {
		yy_push_state(scQuickStats, yyscanner);
		yyextra->yyCurrentSector = strtoul(yyget_text(yyscanner)+6, NULL, 10);
		return QUICKSTATS_SECTOR;
	}
<scQuickStats>[^0-9\xb3]+[0-9]+, {
		int i;
		/* Copy yyget_text(yyscanner) because unput() trashes yyget_text(yyscanner) */
		memcpy_s(yyextra->yyBuffer, sizeof(yyextra->yyBuffer), yyget_text(yyscanner), yyget_leng(yyscanner));
		yyextra->yyBuffer[yyget_leng(yyscanner)] = 0;
		for (i = yyget_leng(yyscanner) - 2; i >= 0; i--)
		{
			/* unput each character except the comma
			 * so that they can be parsed again
			 */
			unput(yyextra->yyBuffer[i]);
		}
	}
<scQuickStats>Turns[ ][0-9]+ {
		yyextra->yyCurrentTurns = strtoul(yyget_text(yyscanner)+5, NULL, 10);
		return CURRENT_TURNS;
	}
<scQuickStats>Creds[ ][0-9]+ {
		yyextra->yyCurrentCreds = strtoul(yyget_text(yyscanner)+6, NULL, 10);
		return CURRENT_CREDS;
	}
<scQuickStats>Figs[ ][0-9]+ {
		yyextra->yyCurrentFigs = strtoul(yyget_text(yyscanner)+5, NULL, 10);
		return CURRENT_FIGS;
	}
<scQuickStats>Shlds[ ][0-9]+ {
		yyextra->yyCurrentShields = strtoul(yyget_text(yyscanner)+6, NULL, 10);
		return CURRENT_SHIELDS;
	}
<scQuickStats>Hlds[ ][0-9]+ {
		yyextra->yyCurrentHolds = strtoul(yyget_text(yyscanner)+5, NULL, 10);
		return CURRENT_HOLDS;
	}
<scQuickStats>Ore[ ][0-9]+ {
		yyextra->yyCurrentFuelOre = strtoul(yyget_text(yyscanner)+4, NULL, 10);
		return CURRENT_FUELORE;
	}
<scQuickStats>Org[ ][0-9]+ {
		yyextra->yyCurrentOrganics = strtoul(yyget_text(yyscanner)+4, NULL, 10);
		return CURRENT_ORGANICS;
	}
<scQuickStats>Equ[ ][0-9]+ {
		yyextra->yyCurrentEquipment = strtoul(yyget_text(yyscanner)+4, NULL, 10);
		return CURRENT_EQUIPMENT;
	}
<scQuickStats>Col[ ][0-9]+ {
		yyextra->yyCurrentColos = strtoul(yyget_text(yyscanner)+4, NULL, 10);
		return CURRENT_COLOS;
	}
<scQuickStats>Phot[ ][0-9]+ {
		yyextra->yyCurrentPhotons = strtoul(yyget_text(yyscanner)+5, NULL, 10);
		return CURRENT_PHOTONS;
	}
<scQuickStats>Armd[ ][0-9]+ {
		yyextra->yyCurrentArmids = strtoul(yyget_text(yyscanner)+5, NULL, 10);
		return CURRENT_ARMIDS;
	}
<scQuickStats>Lmpt[ ][0-9]+ {
		yyextra->yyCurrentLimpets = strtoul(yyget_text(yyscanner)+5, NULL, 10);
		return CURRENT_LIMPETS;
	}
<scQuickStats>GTorp[ ][0-9]+ {
		yyextra->yyCurrentGenesisTorps = strtoul(yyget_text(yyscanner)+6, NULL, 10);
		return CURRENT_GENESISTORPS;
	}
<scQuickStats>TWarp[ ]No {
		yyextra->yyCurrentTwarp = 0;
		return CURRENT_TWARP;
	}
<scQuickStats>TWarp[ ][1-2] {
		yyextra->yyCurrentTwarp = strtoul(yyget_text(yyscanner)+6, NULL, 10);
		return CURRENT_TWARP;
	}
<scQuickStats>Clks[ ][0-9]+ {
		yyextra->yyCurrentCloaks = strtoul(yyget_text(yyscanner)+5, NULL, 10);
		return CURRENT_CLOAKS;
	}
<scQuickStats>Beacns[ ][0-9]+ {
		yyextra->yyCurrentBeacons = strtoul(yyget_text(yyscanner)+7, NULL, 10);
		return CURRENT_BEACONS;
	}
<scQuickStats>AtmDt[ ][0-9]+ {
		yyextra->yyCurrentAtomicDetonators = strtoul(yyget_text(yyscanner)+6, NULL, 10);
		return CURRENT_ATOMDETONATORS;
	}
<scQuickStats>Crbo[ ][0-9]+ {
		yyextra->yyCurrentCorbomite = strtoul(yyget_text(yyscanner)+5, NULL, 10);
		return CURRENT_CORBOMITE;
	}
<scQuickStats>EPrb[ ][0-9]+ {
		yyextra->yyCurrentEprobes = strtoul(yyget_text(yyscanner)+5, NULL, 10);
		return CURRENT_EPROBES;
	}
<scQuickStats>MDis[ ][0-9]+ {
		yyextra->yyCurrentMineDisrupters = strtoul(yyget_text(yyscanner)+5, NULL, 10);
		return CURRENT_MINEDISRUPTERS;
	}
<scQuickStats>PsPrb[ ]No {
		yyextra->yyCurrentPsychicProbe = 0;
		return CURRENT_PSYCHICPROBE;
	}
<scQuickStats>PsPrb[ ]Yes {
		yyextra->yyCurrentPsychicProbe = 1;
		return CURRENT_PSYCHICPROBE;
	}
<scQuickStats>PlScn[ ]No {
		yyextra->yyCurrentPlanetScanner = 0;
		return CURRENT_PLANETSCANNER;
	}
<scQuickStats>PlScn[ ]Yes {
		yyextra->yyCurrentPlanetScanner = 1;
		return CURRENT_PLANETSCANNER;
	}
<scQuickStats>LRS[ ]None {
		yyextra->yyCurrentLongRangeScanner = 0;
		return CURRENT_LONGRANGESCANNER;
	}
<scQuickStats>LRS[ ]Dens {
		yyextra->yyCurrentLongRangeScanner = 1;
		return CURRENT_LONGRANGESCANNER;
	}
<scQuickStats>LRS[ ]Holo {
		yyextra->yyCurrentLongRangeScanner = 2;
		return CURRENT_LONGRANGESCANNER;
	}
<scQuickStats>Aln[ ]-?[0-9]+ {
		yyextra->yyCurrentAlignment = strtol(yyget_text(yyscanner)+4, NULL, 10);
		return CURRENT_ALIGNMENT;
	}
<scQuickStats>Exp[ ][0-9]+ {
		yyextra->yyCurrentExperience = strtoul(yyget_text(yyscanner)+4, NULL, 10);
		return CURRENT_EXPERIENCE;
	}
<scQuickStats>Corp[ ][0-9]+ {
		yyextra->yyCurrentCorp = strtoul(yyget_text(yyscanner)+4, NULL, 10);
		return CURRENT_CORP;
	}
<scQuickStats>Ship[ ][0-9]+[ ] {
		BEGIN(scQuickStatsBrief);
		yyextra->yyCurrentShipNum = strtoul(yyget_text(yyscanner)+5, NULL, 10);
		return CURRENT_SHIP_NUM;
	}
 /*! @note note to self - interdictor is not shown in quickstats, go figure! */
<scQuickStatsBrief>[^\r]+ {
		int len = (yyget_leng(yyscanner) > sizeof(yyextra->yyCurrentShipBrief)-1) ? sizeof(yyextra->yyCurrentShipBrief)-1 : yyget_leng(yyscanner);
		yy_pop_state(yyscanner);
		memcpy_s(yyextra->yyCurrentShipBrief, len, yyget_text(yyscanner), len);
		yyextra->yyCurrentShipBrief[len] = 0;
		return CURRENT_SHIP_BRIEF;
	}
 
 /*------------------------------------------------------------------- Misc Tokens ----*/
<*>\x1b\[2J { yy_set_bol(1); }
<*>\x1b\[([0-9]+(;[0-9]+)*)?[mHfABCDsuJKhl]
<*>\r
<*>\n
<*>.

%%

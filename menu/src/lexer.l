%option reentrant
%option extra-type="struct FlexScannerData2*"
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
#include "FlexScannerData2.h"

#include <windows.h>

%}

/*================================================================ START CONDITIONS ----*/
%x scArgs
%x scUnk

cd [Cc][Dd]
ls [Ll][Ss]
run [Rr][Uu][Nn]
kill [Kk][Ii][Ll][Ll]
ban [Bb][Aa][Nn]
unban [Uu][Nn][Bb][Aa][Nn]
allow [Aa][Ll][Ll][Oo][Ww]

/*=========================================================================== RULES ====*/
%%
 /*------------------------------------------------------------- Backspace Handling ----*/
<*>^[^\r\n\x08]{1,}\x08 {
		int i;
		/* Copy yyget_text(yyscanner) because unput() trashes yyget_text(yyscanner) */
		memset(yyextra->yyBuffer, 0, sizeof(yyextra->yyBuffer));
		memcpy_s(yyextra->yyBuffer, sizeof(yyextra->yyBuffer), yyget_text(yyscanner), yyget_leng(yyscanner));
		for (i = yyget_leng(yyscanner) - 2; i > 0; --i)
		{
			/* unput each character except the backspace pair 
			 * so that they can be parsed again
			 */
			unput(yyextra->yyBuffer[i-1]);
		}
		yy_set_bol(1);
	}

<*>[^\r\n\x08]{1,}\x08 {
		int i;
		/* Copy yyget_text(yyscanner) because unput() trashes yyget_text(yyscanner) */
		memset(yyextra->yyBuffer, 0, sizeof(yyextra->yyBuffer));
		memcpy_s(yyextra->yyBuffer, sizeof(yyextra->yyBuffer), yyget_text(yyscanner), yyget_leng(yyscanner));
		for (i = yyget_leng(yyscanner) - 2; i > 0; --i)
		{
			/* unput each character except the backspace pair 
			 * so that they can be parsed again
			 */
			unput(yyextra->yyBuffer[i-1]);
		}
	}
	
<*>^\x08 { yy_set_bol(1); }

 /*------------------------------------------------------------- Backspace Handling ----*/
<INITIAL>^[ ]*{cd}[ ]*/\r    {                                                  return CD;    }
<INITIAL>^[ ]*{cd}[ ]+       { yy_push_state(scArgs, yyscanner);                return CD;    }
<INITIAL>^[ ]*{ls}[ ]*/\r    {                                                  return LS;    }
<INITIAL>^[ ]*{ls}[ ]+       { yy_push_state(scArgs, yyscanner);                return LS;    }
<INITIAL>^[ ]*{run}[ ]*/\r   {                                                  return RUN;   }
<INITIAL>^[ ]*{run}[ ]+      { yy_push_state(scArgs, yyscanner);                return RUN;   }
<INITIAL>^[ ]*{kill}[ ]*/\r  {                                                  return KILL;  }
<INITIAL>^[ ]*{kill}[ ]+     { yy_push_state(scArgs, yyscanner);                return KILL;  }
<INITIAL>^[ ]*{ban}[ ]*/\r   {                                                  return BAN;   }
<INITIAL>^[ ]*{ban}[ ]+      { yy_push_state(scArgs, yyscanner);                return BAN;   }
<INITIAL>^[ ]*{unban}[ ]*/\r {                                                  return UNBAN; }
<INITIAL>^[ ]*{unban}[ ]+    { yy_push_state(scArgs, yyscanner);                return UNBAN; }
<INITIAL>^[ ]*{allow}[ ]*/\r {                                                  return ALLOW; }
<INITIAL>^[ ]*{allow}[ ]+    { yy_push_state(scArgs, yyscanner);                return ALLOW; }
<INITIAL>^[ ]*[^\r\n\x08 ]+  { yy_push_state(scUnk, yyscanner);  yyless(0);     return UNK;   }
<INITIAL>\r                  {                                   yy_set_bol(1); return CR;    }
<scArgs,scUnk>\r             { yy_pop_state(yyscanner);          yy_set_bol(1); return CR;    }

<scArgs,scUnk>[^\r\n\x08]+/\r {
		memset( yyextra->yyArgs, 0, sizeof( yyextra->yyArgs ) );
		memcpy_s( yyextra->yyArgs, sizeof( yyextra->yyArgs ), yyget_text( yyscanner ), yyget_leng( yyscanner ) );
		return ARGS;
	}

 /*------------------------------------------------------------------- Misc Tokens ----*/
<*>\n { yy_set_bol(1); }

%%

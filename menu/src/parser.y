%name MenuParse

%extra_argument { RPW::TW::MenuParser* pParser }

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
#include "MenuParser.h"

#define WIN32_LEAN_AND_MEAN
#include "windows.h"

#include <assert.h>
#include <string>

#define NDEBUG

extern "C"
{
#include "FlexScannerData2.h"
}

void destroy2(std::string* s)
{
	delete s;
}

} /*include*/

/*--------------------------- types ---------------------------------*/
%token_type { unsigned long }
%type args { std::string* }
%type unk { std::string* }

/*--------------------------- destructors ---------------------------*/
/* note: these destructors get called automatically when a type is 
 *       created by the parser, but unused, e.g. due to syntax error.
 *       it is the programmer's responsibility to call the destructors
 *       manually in all other cases, i.e. where access to the type
 *       is gained, whether the type is actually used or not.
 */

%destructor args { destroy2($$); }
%destructor unk { destroy2($$); }

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

one_message ::= CD    args(A) CR . { pParser->MenuManager().cd(*A);    destroy2(A); }
one_message ::= CD            CR . { pParser->MenuManager().cd();                  }
one_message ::= LS    args(A) CR . { pParser->MenuManager().ls(*A);    destroy2(A); }
one_message ::= LS            CR . { pParser->MenuManager().ls();                  }
one_message ::= RUN   args(A) CR . { pParser->MenuManager().run(*A);   destroy2(A); }
one_message ::= RUN           CR . { pParser->MenuManager().run();                 }
one_message ::= KILL  args(A) CR . { pParser->MenuManager().kill(*A);  destroy2(A); }
one_message ::= KILL          CR . { pParser->MenuManager().kill();                }
one_message ::= BAN   args(A) CR . { pParser->MenuManager().ban(*A);   destroy2(A); }
one_message ::= BAN           CR . { pParser->MenuManager().ban();                 }
one_message ::= UNBAN args(A) CR . { pParser->MenuManager().unban(*A); destroy2(A); }
one_message ::= UNBAN         CR . { pParser->MenuManager().unban();               }
one_message ::= ALLOW args(A) CR . { pParser->MenuManager().allow(*A); destroy2(A); }
one_message ::= ALLOW         CR . { pParser->MenuManager().allow();               }
one_message ::= UNK   args(A) CR . { pParser->MenuManager().error(*A); destroy2(A); }
one_message ::=               CR . { pParser->MenuManager().quit(); pParser->LeaveMenu();                }

/*----------------------------- terminals ---------------------------*/
args(A) ::= ARGS . { A = new std::string(pParser->ScannerData().yyArgs); }

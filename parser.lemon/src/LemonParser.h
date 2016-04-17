/// @file LemonParser.h
/// @author rev

#pragma once

#include "IDataActor.h"
#include "IParser.h"

#include <vector>

extern "C"
{
#include "FlexScannerData1.h"
}

typedef void* yyscan_t;

namespace RPW
{
	namespace TW
	{
		class LemonParser : public RPW::TW::IParser
		{
		public:
			LemonParser( void );
			virtual ~LemonParser();

			void Inject( IDataActor& actor );
			bool Initialize( void );
			void Cleanup( void );

			// IParser Interface
			virtual bool Parse( const std::string& data, bool eof = false );
			virtual bool Parse( const char* const data, unsigned long len, bool eof = false );
			virtual bool IsInitialized( void );

			// lemon support
			void ParseAccept( void );
			void SyntaxError( void );
			void ParseFailure( void );

			RPW::TW::IDataActor& Actor( void );

			FlexScannerData1& ScannerData() { return *m_pScannerData; }

		protected:
			std::string m_yyWrapText;
			void* m_pParser;         ///< parser pointer
			long m_token0;           ///< default token type
			RPW::TW::IDataActor* m_pActor;      ///< actor interface
			FlexScannerData1* m_pScannerData;
			yyscan_t m_scanner;
		};

	}
}
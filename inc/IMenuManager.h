/// @file IMenuManager.h
/// @author rev

#pragma once

namespace RPW
{
	namespace TW
	{

		class IMenuManager
		{
		public:
			virtual ~IMenuManager( void ) {}
			virtual void cd( void ) = 0;
			virtual void cd( std::string& folder ) = 0;
			virtual void ls( void ) = 0;
			virtual void ls( std::string& huh ) = 0;
			virtual void run( void ) = 0;
			virtual void run( std::string& script ) = 0;
			virtual void kill( void ) = 0;
			virtual void kill( std::string& script ) = 0;
			virtual void ban( void ) = 0;
			virtual void ban( std::string& ip ) = 0;
			virtual void unban( void ) = 0;
			virtual void unban( std::string& ip ) = 0;
			virtual void allow( void ) = 0;
			virtual void allow( std::string& ip ) = 0;
			virtual void quit( void ) = 0;

			virtual void accept( void ) = 0;
			virtual void error( std::string& huh ) = 0;
			virtual void error( void ) = 0;
			virtual void fail( void ) = 0;
		};

	}
}

/// @file IScriptManager.h
/// @author rev

namespace RPW
{
	namespace TW
	{

		class IScriptManager
		{
		public:
			virtual void cd( void ) = 0;
			virtual void cd( std::string& folder ) = 0;
			virtual void ls( void ) = 0;
			virtual void run( std::string& script ) = 0;
			virtual void kill( std::string& script ) = 0;
			virtual void show( void ) = 0;
		};

	}
}

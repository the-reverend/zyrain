/// @file ISecurityManager.h
/// @author rev

namespace RPW
{
	namespace TW
	{

		class ISecurityManager
		{
		public:
			virtual void ban( std::string& ip ) = 0;
			virtual void unban( std::string& ip ) = 0;
			virtual void allow( std::string& ip ) = 0;
		};

	}
}

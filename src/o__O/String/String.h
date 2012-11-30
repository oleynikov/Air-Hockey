#pragma once

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

/*

	Class, performing some extra operations with strings:
	1. `Explode` splits the string by the char delimiter
	
*/

namespace o__O
{
	
	class String
	{
		public:
			static void						Split( std::vector<std::string>& , std::string , char );
			static std::string				FTS ( float );
			static std::string				ITS ( const int );
	};
	
}
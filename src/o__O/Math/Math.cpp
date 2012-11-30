#include "Math.h"

using namespace o__O;

bool			Math::CoinFlip ( void )
{

	if ( random() % 2 == 0 )
	{
		return true;
	}
	
	return false;
};
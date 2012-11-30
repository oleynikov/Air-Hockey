#include "String.h"

using namespace o__O;

void String::Split( std::vector<std::string>& output , std::string input , char delimiter )
{
	// calculating the size of the `input` string
	int inputSize = input.size();

	// we are not going to add empty strings to the `output` vector
	if ( inputSize > 0 )
	{
		// searching for the position of the first delimiter.
		size_t firstDelimiter = input.find_first_of ( delimiter );

		// if `firstDelimiter` is greater that `inputSize` this will mean 
		// that there are no delimiters is the input
		if ( firstDelimiter < inputSize )
		{
			// if the first character of the input is NOT a `delimiter`
			// we are going to add the first fragment to the output
			if ( firstDelimiter != 0 )
			{
				std::string firstFragment = input.substr ( 0 , firstDelimiter );
				output.push_back ( firstFragment );
			};
			// then we cut off the first fragment and the first delimiter and call the `Explode`
			// function again with `input` string containing the rest of the string
			std::string restOfInput = input.substr ( firstDelimiter + 1 , inputSize - firstDelimiter - 1 );
			String::Split ( output , restOfInput , delimiter );
		}
		// if there is no `delimiter` in the input, we just add the `input` to the `output`
		// and quit the recursion
		else
		{
			output.push_back ( input );
		};	
	};
};

std::string String::FTS ( float input )
{
	std::stringstream stream;
	
	stream << input;
	
	return stream.str();
};

std::string String::ITS ( int input )
{
	std::stringstream stream;
	
	stream << input;
	
	return stream.str();
};
#pragma once

#include <iostream>

namespace o__O
{

	template <typename type>
	class Pointer
	{

		private:
			type* pointer;

		public:
			
			Pointer ( void );
			Pointer ( const Pointer<type>& pointer );
			~Pointer ( void );
		
	};
	
}

template <typename type>
o__O::Pointer<type>::Pointer ( void )
{

	/*
	
		 Default constructor
		 
	*/
	
	
	
	//	Initializing the pointer with NULL 
	pointer = NULL;
	
}

template <typename type>
o__O::Pointer<type>::Pointer ( const Pointer<type>& pointerRfr )
{

	/*
	
		Copy constructor
		
	*/
	
	
	
	//	If pointer is not trying to copy itself
	if ( this != &pointerRfr )
	{
	
		pointer = pointerRfr.pointer;
	
	}

}



template <typename type>
o__O::Pointer<type>::~Pointer ( void )
{

	/*
	
		Destructor
		
	*/
	
	
	
	delete pointer;
	
}
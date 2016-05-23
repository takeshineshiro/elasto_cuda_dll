// cuda_dll.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "cuda_dll.h"


// This is an example of an exported variable
CUDA_DLL_API int ncuda_dll=0;

// This is an example of an exported function.
CUDA_DLL_API int fncuda_dll(void)
{
	return 42;
}

// This is the constructor of a class that has been exported.
// see cuda_dll.h for the class definition
Ccuda_dll::Ccuda_dll()
{
	return;
}

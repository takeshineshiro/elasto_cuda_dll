// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the CUDA_DLL_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// CUDA_DLL_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef CUDA_DLL_EXPORTS
#define CUDA_DLL_API __declspec(dllexport)
#else
#define CUDA_DLL_API __declspec(dllimport)
#endif

// This class is exported from the cuda_dll.dll
class CUDA_DLL_API Ccuda_dll {
public:
	Ccuda_dll(void);
	// TODO: add your methods here.
};

extern CUDA_DLL_API int ncuda_dll;

CUDA_DLL_API int fncuda_dll(void);

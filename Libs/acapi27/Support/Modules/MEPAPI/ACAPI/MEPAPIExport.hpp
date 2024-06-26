#ifndef GS_MEPAPI_EXPORT_HPP
#define GS_MEPAPI_EXPORT_HPP

// from GSRoot
#include "PlatformDLLExport.hpp"


#if defined (MEPAPI_COMPILE)
#define MEPAPI_EXPORT		PLATFORM_DLL_EXPORT
#define MEPAPI_IMPL_EXPORT	PLATFORM_DLL_EXPORT
#else
#define MEPAPI_EXPORT		PLATFORM_DLL_IMPORT
#define MEPAPI_IMPL_EXPORT	PLATFORM_DLL_IMPORT
#endif


#endif /* GS_MEPAPI_EXPORT_HPP */

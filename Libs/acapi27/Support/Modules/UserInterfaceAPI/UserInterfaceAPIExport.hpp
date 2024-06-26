#ifndef USERINTERFACEAPI_EXPORT
#define USERINTERFACEAPI_EXPORT

#pragma once

#include "PlatformDLLExport.hpp"

#if defined (USERINTERFACEAPI_DLL_COMPILE)
	#define USERINTERFACEAPI_DLL_EXPORT		PLATFORM_DLL_EXPORT
#else
	#define USERINTERFACEAPI_DLL_EXPORT		PLATFORM_DLL_IMPORT
#endif

#endif

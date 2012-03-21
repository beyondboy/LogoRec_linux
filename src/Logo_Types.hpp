#ifndef __LOGO_TYPES_H__
#define __LOGO_TYPES_H__

#define	LOGOAPI_EXPORTS


#define LOGO_DLL

#if defined(_WIN32) && defined(LOGO_DLL)
	#if defined(LOGO_EXPORTS)
		#define LOGO_API __declspec(dllexport)
	#else
		#define LOGO_API __declspec(dllimport)
	#endif
#endif


typedef int LogoStatus;

enum eLOGO_STAUS
{
  LOGO_StsOK			=	0,  
  LOGO_StsUNError		=	-1,
  LOGO_StsNullPtr		=	-2,
  LOGO_StsConfError		=	-3,
  LOGO_StsBadMem		=	-4,
  LOGO_StsFileError		=	-5
};

const int KNN_MAX	=  100;

const int SIFT_DIMENSION = 128;

#endif
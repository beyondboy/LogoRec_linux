#ifndef __LOGO_UTILS_H__
#define __LOGO_UTILS_H__

//#include "Logo_Precomp.hpp"
#include "Logo_Types.hpp"
#include <string>

namespace LogRec
{
	LogoStatus GetVisualDictOfSIFT(std::string dstVocabFile, int thresh_size, int k, std::string filePath, std::string fileList);
}

#endif

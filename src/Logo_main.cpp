#include "Logo_Precomp.hpp"
#include "Logo_Logger.hpp"
#include "Logo_Utils.hpp"
#include "Logo_Core.hpp"

using namespace LogRec;

void util_test(std::string basedir)
{
	LogRec::LOG_SET_MIN_LEVEL(LogRec::LEVEL_ALL);
	LogRec::LOG_SET_OPTION(LogRec::LOG_OPTION_ALL);
  std::string confpath = basedir + "conf/conf.txt";
	LogRec::GetVisualDictOfSIFT("vv.txt", 150000, 1500,"/home/beyond_boy/logorec/FlickrLogos/classes/","trainingset.txt");
	hello();

/*	LogRec::CTriTrain ctr;
	ctr.LoadConf(confpath);
	ctr.Init();
	cout << "Init:" << confpath << endl;

	LogRec::CTriAngleQuery cqr;
	cqr.LoadConf(confpath);
	cqr.Query();
*/
	system("pause");
}

int main(int argc, char **argv)
{//
  if (argc !=2 )
	{
    cout << argv[0] << " [basedir] " << endl;
	  return -1;
	}
	std::string basedir = argv[1];
	LogRec::SET_LOG_BASEDIR(basedir);
	util_test(basedir);
	return 0;
}

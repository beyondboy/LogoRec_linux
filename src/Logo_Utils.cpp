#include "Logo_Precomp.hpp"
#include "Logo_Types.hpp"
#include "Logo_Logger.hpp"
#include "Logo_Utils.hpp"

namespace LogRec
{
	LogoStatus GetVisualDictOfSIFT(std::string dstVocabFile, int thresh_size, int k, std::string filePath, std::string fileList)
	{
		std::cout << "---------START NOW----------" << std::endl;
		ifstream fin((filePath+fileList).c_str());

		if (!fin)
		{
			//cerr << "open file failed!" << endl;
			LogRec::LOG_LEVEL_ERROR;
			LogRec::PRINT_LOG_MESSAGE("--failed to open file!--");
			return LOGO_StsFileError;
		}
		std::string path = filePath;

		//read images from given path
		string line;

		//用来聚类求visual vocabulary 的矩阵
		cv::Mat m_Features;
		int length = SIFT_DIMENSION;
		m_Features.create(thresh_size, SIFT_DIMENSION, CV_32FC1);
		
		int picNum = 0;
		int i_totalF = 0;
		float* p_imgPtr = m_Features.ptr<float>(0);


		LogRec::LOG_SET_OPTION(LogRec::LOG_OPTION_INFO);
		std::cout << "-------READ LIST-------" << std::endl;
		while ( getline(fin,line) )
		{
			istringstream filenameExtractor;

			string fileDir;
			filenameExtractor.str(line.c_str());
			filenameExtractor >> fileDir;// >> str;

			string fileName;
			filenameExtractor >> fileName;
			string fullNamePath = path+fileDir+"/"+fileName;
			//根据路径抽取图片特征点

			cv::Mat image = cv::imread(fullNamePath,CV_LOAD_IMAGE_GRAYSCALE);//
			if( !image.data )
			{
				LogRec::LOG_LEVEL_WARN;
				LogRec::PRINT_LOG_MESSAGE("---failed to open file: %s---",fullNamePath.c_str());				
				continue;
			}

//#define _SURF_

#ifdef	_SURF_
			int hessian = 500;
			SurfFeatureDetector detector( hessian );
			SurfDescriptorExtractor extractor(4,2,1);
			Mat descriptors;
			LogRec::LOG_LEVEL_INFO;
			LogRec::PRINT_LOG_MESSAGE("--Use Surf");
#else
			SiftFeatureDetector detector;
			SiftDescriptorExtractor extractor;
			Mat descriptors;
			LogRec::LOG_LEVEL_INFO;
			LogRec::PRINT_LOG_MESSAGE("--Use Sift");
#endif			
			std::vector<cv::KeyPoint> keypoints;

			detector.detect(image,keypoints);
		
			extractor.compute(image,keypoints,descriptors);


			for(int i = 0; i < descriptors.rows; i++ )
			{
				//const float* descriptor = (const float*)descriptors.ptr<float>(i);
				const float* descriptor = (const float*)(descriptors.data + length*sizeof(float)*i);
				//descriptors
				memcpy(p_imgPtr, descriptor, length*sizeof(float));
				p_imgPtr += length;
				
				i_totalF++;
				/**/if (i_totalF > thresh_size)
				{
					break;
				}
			}

			if (i_totalF > thresh_size)
			{
				break;
			}
			picNum++;

			LogRec::LOG_LEVEL_INFO;
			LogRec::PRINT_LOG_MESSAGE("PIC_NAME: %s TOTAL_FEATURES: %d PIC_NUM: %d \n",
				line.c_str(),i_totalF,picNum);
			//image = NULL;
		}

		std::cout << "-------CLUSTERING-------" << std::endl;
		
		LogRec::LOG_LEVEL_INFO;
		LogRec::PRINT_LOG_MESSAGE("TOTAL_FEATURES: %d",i_totalF);
		
		BOWKMeansTrainer bowKms(k);
		//bowKms.add(m_Features);
		double tCount = (double)cvGetTickCount();
		LogRec::LOG_LEVEL_INFO;
		LogRec::PRINT_LOG_MESSAGE("\n<-----------------------KMEANS BEGIN----------------------->\n");
		//cout << "begin kmeans------------>>>>>>>>>>>>>>" << endl;
		cv::Mat m_vocab = bowKms.cluster(m_Features);
		LogRec::LOG_LEVEL_INFO;
		LogRec::PRINT_LOG_MESSAGE("<-----------------------KMEANS OVER----------------------->");
		tCount = (double)cvGetTickCount() - tCount;

		ofstream ofin((filePath+dstVocabFile).c_str());
		//float farr[2][3] = {{4.233,0.987,4.32},{0.44,0.666,0.34}};
		//printf("The Clustering result!\n");
		LogRec::LOG_LEVEL_INFO;
		LogRec::PRINT_LOG_MESSAGE("<-----------------------CLUSTERING RESULT----------------------->");
		
		for (int j = 0; j < m_vocab.rows; j++)
		{
			string sline;
			for (int k = 0; k < m_vocab.cols; k++)
			{
				string tstr;
				ostringstream ostr;
				ostr << m_vocab.at<float>(j,k) << " ";
				sline += ostr.str();
				//istringstream istr(ostr.str());
				//istr >> tstr;
			}
			ofin << sline << endl;
			//printf("\n");
		}

		ostringstream lline;
		lline << "cost time:" << tCount;
		ofin << lline.str() << endl;
		LogRec::LOG_SET_OPTION(LogRec::LOG_OPTION_ALL);
		return LOGO_StsOK;
	}
	
}                                                                                  

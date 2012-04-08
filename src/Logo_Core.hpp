#ifndef __LOGO_CORE_H__
#define __LOGO_CORE_H__

#include "Logo_Precomp.hpp"
#include "Logo_Base.hpp"

namespace LogRec
{

	void hello();
	struct LogoConf
	{
		//golbal conf
		std::string m_sConfPath;
		std::string m_sDataSetPath;//training,query and validation data
		std::string m_valDicFile;
		
		//train
		std::string m_sTrainFilePath;
		std::string m_sTrainList;

		//validate
		std::string m_sValidateFilePath;
		std::string m_sValidateList;

		//query
		std::string m_sQueryFilePath;
		std::string m_sQueryList;

		//model 
		std::string m_sModelDataPath;
		std::string m_sModeListlFile;

		//log
		std::string m_sLogFilePath;
		std::string m_sLogFileType;
		//		
	};

	struct SModel
	{
	public:
		bool add_edgeIndex(long edge)
		{
			if ( !m_edgeIndex.insert(edge).second )
			{
				return false;
			}
			return true;
		}

		bool add_triIndex(long long triangle)
		{
			if ( !m_triIndex.insert(triangle).second )
			{
				return false;
			}
			return true;
		}
		bool saveModel(const std::string &m_sModelDataPath , const std::string &className);
		bool clearModel();
	private:
		std::string m_className;
		std::set<long> m_edgeIndex;
		std::set<long long> m_triIndex;
	};

	class CTriBase
	{
	public:

		CTriBase();
		~CTriBase();
		bool IsPointAlreadyIn(std::vector<LogRec::LogoSLRPoint> &VSet,LogRec::LogoSLRPoint &Pt);
		bool LoadCodeBook();
	
		int getVisualWordCode(float fdes[]);
	
		bool LoadConf(std::string confFile);

		static LogParams& getGlobalParams();

		static const LogoConf& getGlobalConf();

	protected:

		std::vector<std::string> m_sModelList;
		std::map<std::string, std::string> m_confList;
		std::vector<LogRec::LogoCodeBookFormat> m_visualDict;
		static LogRec::LogParams m_gParams; 
		//LogRec::LogParams m_gParams; 
		static LogRec::LogoConf  m_gConf;

		static std::list<SModel> m_models;
		//LogRec::LogoConf  m_gConf;
	};

	class CTriTrain : public CTriBase
	{
	public:

		CTriTrain();

		~CTriTrain();

		bool Init();

		bool Train();	

		bool ExtractFeatures( std::string className, std::vector<std::string> &sLogoList );

		bool ReadTrainSRLFileList();
	
		//bool TrainSRLSingleLogoClass(std::string className, std::vector<std::string> &sLogoList );

		//test function
		
		bool DataClearn();

		static SModel& GetModel()
		{
			return m_tModel;
		}

		static bool SaveSingleModel( std::string calssName );

		static void SaveModels();

	protected:

		std::map<std::string,std::vector<std::string> > m_mLogoList;	

		static SModel m_tModel;

	};

	class CTriAngleQuery : public CTriBase
	{//Test and Validatation
	public:
		CTriAngleQuery();
		~CTriAngleQuery();
		bool Query();
		bool GetQueryResult();

		bool GetCandidateQueryEdge();

		bool LoadModel();

		bool GetFeatureOfSinglePic(std::string &className);

		bool GetRandomEdgesByMontCarlo();
		
		bool GetSimpleRandomEdge();
		
		bool GetRandomEdgeByReserviorSample();

		bool getKNNpoints();

		bool getQueryList();

		bool clearnCache();

	public:

		//边索引
		std::map<std::string, std::set<long> > m_edgeIndex;
		//三角形索引
		std::map<std::string, std::set<long long> > m_triIndex;
		//图片特征点
		std::vector<LogRec::LogoSLRPoint> m_vPicFeature;
		//随机采样的边
		std::vector<LogRec::LogoSLREdge> m_vRandEdge;
		//候选边
		std::vector<LogRec::LogoSLREdge> m_vCandidateEdge;
		//候选点
		std::vector<LogRec::LogoSLRPoint> m_vCandidatePoint;
		//统计结果
		std::map<std::string, std::set<long long> > m_resultCal;
		//
		std::string m_sQuery;
		
		//batch query
		//std::vector<std::pair<std::string,std::string> > m_vQList;
		std::map<std::string, std::list<string> > m_mQlist;
	};

}

#endif

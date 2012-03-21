#include "Logo_Precomp.hpp"
//#include "Logo_Base.h"
#include "Logo_Common.hpp"
#include "Logo_Core.hpp"
#include "Logo_Logger.hpp"

namespace LogRec
{
	//init of static member variables
	void hello()
	{
		cout << "HELLO WORLD!" << endl;
	}
	LogRec::LogParams CTriBase::m_gParams; 

	LogRec::LogoConf CTriBase::m_gConf;
	
	SModel CTriTrain::m_tModel;

	CTriBase::CTriBase()
	{}
	CTriBase::~CTriBase()
	{}

	LogParams& CTriBase::getGlobalParams()
	{
		return m_gParams;
	}

	const LogoConf& CTriBase::getGlobalConf()
	{
		return m_gConf;
	}

	bool CTriBase::LoadConf(std::string confFile)
	{
		LogRec::LOG_SET_MIN_LEVEL(LogRec::LEVEL_OFF);//LEVEL_ALL
	
		std::ifstream ifconf(confFile.c_str());

		if ( !ifconf )
		{
			//std::cerr << "open conf File failed! " << __FILE__ << __LINE__ << std::endl;
			LogRec::LOG_LEVEL_ERROR
			LogRec::PRINT_LOG_MESSAGE("Load Conf file error!\n");
			return false;
		}

		//std::map<std::string, std::string> m_confList;	
		std::string key;
		std::string value;
		std::string sline;

		while(std::getline(ifconf,sline))
		{
			LogRec::LogoCutBlankBE(sline);
			if ( sline.find('#') == 0 )
			{
				m_confList["comments"] += sline;
			}
			else if ( ( sline.find('#') != std::string::npos ) )
			{
				std::string line = sline.substr( 0,sline.find_first_of('#') );
				key = line.substr(0,line.find_first_of('='));
				value = line.substr(line.find_first_of('=')+1);

				LogRec::LOG_LEVEL_INFO
				LogRec::PRINT_LOG_MESSAGE("KEY: %s VALUE: %s \n",key.c_str(),value.c_str());
				//std::cout << "key:" << key << " value:" << value << std::endl;
				LogRec::LogoCutBlankBE(key);
				LogRec::LogoCutBlankBE(value);
				m_confList[key] = value;
				m_confList["comments"] += sline.substr(sline.find_first_of('#'));
			}
			else
			{
				std::string line = sline;
				key = line.substr(0,line.find_first_of('='));
				value = line.substr(line.find_first_of('=')+1);
				LogRec::LogoCutBlankBE(key);
				LogRec::LogoCutBlankBE(value);
				LogRec::LOG_LEVEL_INFO
				LogRec::PRINT_LOG_MESSAGE("KEY: %s VALUE: %s \n",key.c_str(),value.c_str());				
				//std::cout << "key:" << key << " value:" << value << std::endl;
				m_confList[key] = value;
			}
		//golbal conf
		m_gConf.m_sConfPath = m_confList["conf_file_path"];
		m_gConf.m_sDataSetPath = m_confList["dataset_path"];//training,query and validation data
		m_gConf.m_valDicFile = m_confList["visual_dict_file"];
		
		//train
		m_gConf.m_sTrainFilePath = m_confList["train_file_path"];
		m_gConf.m_sTrainList = m_confList["train_list"];

		//validate
		m_gConf.m_sValidateFilePath = m_confList["validate_file_path"];
		m_gConf.m_sValidateList = m_confList["validate_list"];

		//query
		m_gConf.m_sQueryFilePath = m_confList["query_file_path"];
		m_gConf.m_sQueryList = m_confList["query_list"];

		//model 
		m_gConf.m_sModelDataPath = m_confList["mode_file_path"];
		m_gConf.m_sModeListlFile = m_confList["model_list"];

		//log
		m_gConf.m_sLogFilePath = m_confList["log_file_path"];
		m_gConf.m_sLogFileType = m_confList["log_file_type"];
			//...
		}

		LogRec::LOG_SET_MIN_LEVEL(LogRec::LEVEL_ALL);//LEVEL_OFF
		return true;
	}
	//load visual vocabulary
	bool CTriBase::LoadCodeBook()
	{//读取visual words vocabulary
		ifstream fin((m_gConf.m_sConfPath+m_gConf.m_valDicFile).c_str());//("flickr_logos_32_dataset_training_set_annotation.txt");

		if (!fin)
		{
			//cout << "file open error!" << __FILE__ << __LINE__ << endl;//use log in the future!
			LogRec::LOG_LEVEL_ERROR
			LogRec::PRINT_LOG_MESSAGE( "failed to load codebook! ");
			return false;
		}

		string line;
		int iSig = 1;
		while (getline(fin,line))
		{
			LogRec::LogoCodeBookFormat lcb;
			istringstream eleExtractor;
			eleExtractor.str(line.c_str());
			for(int i = 0; i < SIFT_DIMENSION; i++)
			{
				eleExtractor >> lcb.m_fDescriptor[i];
			}
			lcb.m_iSig = iSig;
			iSig++;
			m_visualDict.push_back(lcb);
		}
		return true;
	}

	bool CTriBase::IsPointAlreadyIn(std::vector<LogRec::LogoSLRPoint> &VSet,LogRec::LogoSLRPoint &Pt)
	{
		for (std::vector<LogRec::LogoSLRPoint>::iterator itr = VSet.begin(); itr != VSet.end(); itr++)
		{
			if (Pt == *itr)
			{
				return true;
			}
		}
		return false;
	}

	int CTriBase::getVisualWordCode(float fdes[])
	{
		//float fmin = 0.0;
		int sig = 0;
		float fmin = LogRec::LogoEuclidean(fdes,m_visualDict[0].m_fDescriptor,SIFT_DIMENSION);
		for (std::vector<LogRec::LogoCodeBookFormat>::iterator itr = m_visualDict.begin();
			itr != m_visualDict.end(); itr++)
		{
			float ft = LogRec::LogoEuclidean(fdes,itr->m_fDescriptor,SIFT_DIMENSION);
			if (fmin > ft)
			{
				fmin = ft;
				sig = itr->m_iSig;
			}
		}
		return sig;
	}

	bool SModel::saveModel(const std::string &m_sModelDataPath , const std::string &className)
	{
		//	边索引和三角形索引
		std::string edgeIndexFile = m_sModelDataPath+"/"+className+"_EDGE_INDEX.txt";
		
		ofstream edgeOf(edgeIndexFile.c_str(),std::ios::app);
		
		edgeOf << className << " ";

		for (std::set<long>::iterator eitr = m_edgeIndex.begin(); eitr != m_edgeIndex.end(); eitr++)
		{
			edgeOf << *eitr << " ";
		}

		edgeOf << endl;
		edgeOf.close();

		std::string triIndexFile = m_sModelDataPath+"/"+className+"_TRI_INDEX.txt";
		ofstream triOf(triIndexFile.c_str(),std::ios::app);

		triOf << className << " ";
		for (std::set<long long>::iterator eitr = m_triIndex.begin(); eitr != m_triIndex.end(); eitr++)
		{
			triOf << *eitr << " ";
		}
		triOf << endl;
		triOf.close();
		//DataClearn();
		return true;
	}

	bool SModel::clearModel()
	{
		m_edgeIndex.clear();
		m_triIndex.clear();
	  return true;
	}

	CTriTrain::CTriTrain(){}
	CTriTrain::~CTriTrain(){}

	bool CTriTrain::SaveSingleModel( std::string calssName )
	{
		m_tModel.saveModel(m_gConf.m_sModelDataPath,calssName);
		return true;
	}
	void CTriTrain::SaveModels()
	{
		m_tModel.saveModel(m_gConf.m_sModelDataPath,"ALL_CLASS");
	}

	struct CLogCom
	{
	public:
		CLogCom()
		{
		}
		CLogCom(LogRec::LogoSLRPoint lpt,float dis)
		: m_lpt(lpt),m_dis(dis)
		{
		}
		bool operator () (const CLogCom &C1, const CLogCom &C2)
		{
			return C1.m_dis < C2.m_dis;
		}
	public:
		LogRec::LogoSLRPoint m_lpt;
		float m_dis;
	};
	bool operator < (const CLogCom &C1, const CLogCom &C2)
	{
		return C1.m_dis < C2.m_dis;
	}


	bool FNNPoints(LogRec::LogFPtF &ptA, list<LogRec::LogoSLRPoint> &PSetB,list<LogRec::LogFPtF> &VSetB, std::list<LogRec::LogoSLRPoint> &vlist, int n)
	{
		if (n > PSetB.size())
		{
			return false;
		}
		LogRec::LOG_LEVEL_INFO;
		LogRec::PRINT_LOG_MESSAGE("NOW BEGIN FIND %d NEAREST POINTS FROM %d POINTS",n,PSetB.size());
		std::vector<CLogCom> vcomp;
		vcomp.reserve(PSetB.size()+1);
		//std::list<LogRec::LogoSLRPoint> vlist;
		std::vector<float> fv;
		fv.reserve(n);
		std::vector<LogRec::LogoSLRPoint> pv;
		pv.reserve(n);

		std::list<LogRec::LogoSLRPoint>::iterator pbitr = PSetB.begin();
		for (std::list<LogRec::LogFPtF>::iterator vbitr = VSetB.begin(); pbitr != PSetB.end() && vbitr != VSetB.end(); vbitr++,pbitr++)
		{
			vcomp.push_back(CLogCom(*pbitr,LogRec::LogoEuclidean(ptA.m_fDescriptor,vbitr->m_fDescriptor,SIFT_DIMENSION)));
		}
		//把前n小的放到第n个位置前
		std::nth_element(vcomp.begin(),vcomp.begin()+n-1,vcomp.end());
		//std::nth_element(vcomp.begin(),vcomp.begin()+n-1,vcomp.end(),CLogCom());
		//vlist.insert(vlist.begin(),pv.begin(),pv.end());
		for (int i = 0; i < n; i++)
		{
			vlist.push_back(vcomp[i].m_lpt);
		}
		return false;
	}
/**
 *
 *typedef for simplicity
 *
 **/
	typedef std::list<LogRec::LogoSLREdge> edgeList;

	typedef std::list<LogRec::LogoSLRPoint> ptList;

	typedef std::list<LogRec::LogFPtF> pftList;

	typedef std::map<int, pftList > LogPicVWFP;

	typedef std::map<int, ptList > LogPicVW; 

	typedef std::map<std::string, LogPicVW > LogoSTClass;

	typedef std::map<std::string, LogPicVWFP > LogoFPSTClass;


	void GetInitCandidateEdge( LogPicVW &PSetA, LogPicVW &PSetB, LogPicVWFP &PVWSetA, LogPicVWFP &PVWSetB, edgeList &EdgeSetA, edgeList &EdgeSetB )
	{

		//根据特征数来自适应条件每个特征点最多匹配的点数
		//判断是否需要优化m_gParams

		//找出B中和A中某特征点最接近的n个特征点
		//该结构存放和A中每一个点距离最近的B中n(300)个点
		//map的第一维表示A中特征点的label(visual word),第二维的链表表示A中所有具有该label值的特征点
		//链表中的链表表示与该特征点(第一维链表中的某个元素)具有相同label值且距离最近的B中的n个点
		std::map<int, std::list<std::list<LogRec::LogoSLRPoint> > > mmlist;

		std::map<int, std::list<LogRec::LogFPtF> >::iterator pvitr = PVWSetA.begin();
		
		for (LogPicVW::iterator pitr = PSetA.begin(); pitr != PSetA.end(); pitr++,pvitr++)
		{
			list<LogRec::LogFPtF>::iterator pvvitr = pvitr->second.begin();
			for(list<LogRec::LogoSLRPoint>::iterator ppitr = pitr->second.begin();
				ppitr != pitr->second.end(); ppitr++,pvvitr++)
			{
				//对A中的每一个特征点，只保留B中和其lable相同的距离最近的前m_iMaxComNum(300)个
				if ( PSetB[pitr->first].size() < CTriBase::getGlobalParams().m_iMaxComNum )
				{
					//和另外一个结构同步即可
					mmlist[pitr->first].push_back(PSetB[pitr->first]);
				}
				else
				{
					std::list<LogRec::LogoSLRPoint> loglstpt; 
					//FindkNearstPoint(*pvvitr,PSetB[pitr->first],PVWSetB[pitr->first],loglstpt,m_gParams.m_iMaxComNum);
					FNNPoints(*pvvitr,PSetB[pitr->first],PVWSetB[pitr->first],loglstpt,CTriBase::getGlobalParams().m_iMaxComNum);				
					mmlist[pitr->first].push_back(loglstpt);
				}
			}
		}

		int times = 0;
		int i = 0;
		//用来除重的辅助结构
		/*std::tr1::hash<long> hA;
		std::tr1::hash<long> hB;*/

		/*std::hash_map<long,long,std::tr1::hash<long> > hA;
		std::hash_map<long,long,std::tr1::hash<long> > hB;*/
		//std::hash_map<long,long> hA;
		//std::hash_map<long,long> hB;/**/
		__gnu_cxx::hash_set<long> hA;
		__gnu_cxx::hash_set<long> hB;/**/
		
		//std::tr1::ha
		//
		std::map<int, std::list<std::list<LogRec::LogoSLRPoint> > >::iterator first_itr_mmlist = mmlist.begin();
		//PSetA存放了图片A中特征点的所有信息，第一维为特征点的label,第二维为label值相同的所有点，放在一个List中		
		//注：该list与mmlist中的list同步
	
		//第一重循环，按不同label值个数来进行遍历
		int rc = 0;
		int maxRound = 100000000;

		for (LogPicVW::iterator first_itr_psetA = PSetA.begin(); (first_itr_psetA != PSetA.end()) && i < PSetA.size(); 
			first_itr_psetA++,first_itr_mmlist++)
		{
			//变量具有共同label的多个点集
			int j = 0;
			if (times > maxRound)
			{
			  break;
			}
			//第二重循环，遍历A中具有相同的某个label值的的所有点
			for(list<LogRec::LogoSLRPoint>::iterator sitr_psetA = first_itr_psetA->second.begin(); 
				sitr_psetA != first_itr_psetA->second.end(); sitr_psetA++)
			{
				int k = 0;
				if (times > maxRound)
				{
				  break;
				}
				std::map<int, std::list<std::list<LogRec::LogoSLRPoint> > >::iterator mtt		= first_itr_mmlist;
				std::map<int, std::list<std::list<LogRec::LogoSLRPoint> > >::iterator second_itr_mmlist	= ++mtt;
				
				//第三重循环，依次遍历除了第一重循环外的其他label
				for (LogPicVW::iterator t = first_itr_psetA, spitr_psetA = ++t; spitr_psetA != PSetA.end(); spitr_psetA++,second_itr_mmlist++)
				{
					int m = 0;
				  if (times > maxRound)
					{
					  break;
					}
					//第四重循环，遍历下一个具有共同label的的所有特征点
					bool repeatEdgeA = false;
					for(list<LogRec::LogoSLRPoint>::iterator cspitr_psetA = spitr_psetA->second.begin();
						cspitr_psetA != spitr_psetA->second.end(); cspitr_psetA++)
					{

						if (times > maxRound)
						{
						  break;
						}
						//提前优化，两个点构成的边如果长度小于某个阀值(5pixel),则去掉
						if(LogRec::LogoGetTwoPointsDistance(*sitr_psetA,*cspitr_psetA) <= CTriBase::getGlobalParams().m_eMin)
						{
							continue;
						}
						int l = 0;
						//下五、六重循环主要处理的情况：
						/**
						 *
						 *对于图片A中某两个具有不同label的特征点构成的边，找出B中和其对应的所有边
						 *
						 *方法：根据A中的某个特征点
						 *
						 **/
						for(list<LogRec::LogoSLRPoint>::iterator fist_points_itr = (first_itr_mmlist->second.begin())->begin();
							fist_points_itr != (first_itr_mmlist->second.begin())->end(); fist_points_itr++)
						{
							int q = 0;

						  if (times > maxRound)
						  {
							  break;
						  }
							for (list<LogRec::LogoSLRPoint>::iterator second_points_itr = (second_itr_mmlist->second.begin())->begin();
							second_points_itr != (second_itr_mmlist->second.begin())->end(); second_points_itr++)
							{
								if(LogRec::LogoGetTwoPointsDistance(*fist_points_itr,*second_points_itr) <= CTriBase::getGlobalParams().m_eMin)
								{
									continue;
								}

								if (times > maxRound)
								{
									LogRec::LOG_LEVEL_INFO;
									LogRec::WRITE_LOG_MESSAGE("==EDGE SIZE LARGE THAN maxRound==");
									break;
								}

								if ( times%5000000 == 0 )
								{
									LogRec::LOG_LEVEL_INFO;
									LogRec::PRINT_LOG_MESSAGE("==CURRENT ROUND: %d==",times);
								}
								times++;
								LogRec::LogoSLREdge edgeA(*sitr_psetA,*cspitr_psetA);
								LogRec::LogoSLREdge edgeB(*fist_points_itr,*second_points_itr);
								float simEdge = LogRec::LogoGetSimEdgeScore(edgeA,edgeB,CTriBase::getGlobalParams().m_alta,CTriBase::getGlobalParams().m_sigma);
								if (simEdge > CTriBase::getGlobalParams().m_eSimThreshold)// && (!IsEdgeAlreadyIn(EdgeSet,edgeA)))
								{									
									LogRec::LogoGetEdgeSig(edgeA,CTriBase::getGlobalParams().m_binSizeOfOriDir);
									LogRec::LogoGetEdgeSig(edgeB,CTriBase::getGlobalParams().m_binSizeOfOriDir);
									/*LogRec::LogoSLREdge edgeAA(edgeA.m_slrPointB,edgeA.m_slrPointA);
									LogRec::LogoSLREdge edgeBB(edgeB.m_slrPointB,edgeB.m_slrPointA);
									float asimEdge = LogRec::LogoGetSimEdgeScore(edgeAA,edgeBB,m_gParams.m_alta,m_gParams.m_sigma);*/
									/*if (edgeA.m_edgeSig == edgeB.m_edgeSig)
									{
										cout << "EDGE SIG EQUAL!" << endl;
									}*/
									if (simEdge > 0.99999)
									{
										std::cout << simEdge << std::endl;
									}

									if (hA.find(edgeA.m_edgeSig) == hA.end())
									{
										hA.insert(edgeA.m_edgeSig);// = edgeA.m_edgeSig;
										EdgeSetA.push_back(edgeA);
									}

									if (hB.find(edgeA.m_edgeSig) == hB.end())
									{
										hB.insert(edgeA.m_edgeSig);// = edgeA.m_edgeSig;
										EdgeSetB.push_back(edgeB);
									}
																			
								}								
							}
						}
					}
				}
			}
		}//end of sixth for
	//cout << times << " round" << endl;
	}

	bool elementIsInList(std::list<int> &iList, int iE)
	{
		for (std::list<int>::iterator itr = iList.begin(); itr != iList.end(); itr++)
		{
			if ( *itr == iE )
			{
				return true;
			}
		}
		return false;
	}

	void getCandidatePoints( edgeList &EdgeSetA, std::list<LogRec::LogoSLREdge> &EdgeSetB, ptList &VSetA, ptList &VSetB )
	{
		//存放顶点的hash_map
		map<int,list<int> > AVcache;
		map<int,list<int> > BVcache;
		//存放边索引的hash,用来进行加速
		//利用hash_map来加速，以少量的空间代价换取时间
		for	(std::list<LogRec::LogoSLREdge>::iterator eitr = EdgeSetA.begin();
			eitr != EdgeSetA.end(); eitr++)
		{
			LogRec::LogoSLREdge EA = *eitr;

			if (AVcache.find(EA.m_slrPointA.m_ix) == AVcache.end())
			{
				AVcache[EA.m_slrPointA.m_ix].push_back(EA.m_slrPointA.m_iy);
				VSetA.push_back(EA.m_slrPointA);
			}
			else
			{
				if ( !elementIsInList(AVcache[EA.m_slrPointA.m_ix],EA.m_slrPointA.m_iy) )
				{
					AVcache[EA.m_slrPointA.m_ix].push_back(EA.m_slrPointA.m_iy);
					VSetA.push_back(EA.m_slrPointA);
				}
			}

			if (BVcache.find(EA.m_slrPointB.m_ix) == BVcache.end())
			{
				BVcache[EA.m_slrPointB.m_ix].push_back(EA.m_slrPointB.m_iy);
				VSetA.push_back(EA.m_slrPointB);
			}
			else
			{
				if ( !elementIsInList(BVcache[EA.m_slrPointB.m_ix],EA.m_slrPointB.m_iy) )
				{
					BVcache[EA.m_slrPointB.m_ix].push_back(EA.m_slrPointB.m_iy);
					VSetA.push_back(EA.m_slrPointB);
				}
			}

		}

		for	(std::list<LogRec::LogoSLREdge>::iterator eitrb = EdgeSetB.begin();
			eitrb != EdgeSetB.end(); eitrb++)
		{
			LogRec::LogoSLREdge EB = *eitrb;
			//LogRec::LogoGetEdgeSig(EB,10);

			if (BVcache.find(EB.m_slrPointA.m_ix) == BVcache.end())
			{
				BVcache[EB.m_slrPointA.m_ix].push_back(EB.m_slrPointA.m_iy);
				VSetB.push_back(EB.m_slrPointA);
			}
			else
			{
				if ( !elementIsInList(BVcache[EB.m_slrPointA.m_ix],EB.m_slrPointA.m_iy) )
				{
					BVcache[EB.m_slrPointA.m_ix].push_back(EB.m_slrPointA.m_iy);
					VSetB.push_back(EB.m_slrPointA);
				}
			}

			if (BVcache.find(EB.m_slrPointB.m_ix) == BVcache.end())
			{
				BVcache[EB.m_slrPointB.m_ix].push_back(EB.m_slrPointB.m_iy);
				VSetB.push_back(EB.m_slrPointB);
			}
			else
			{
				if ( !elementIsInList(BVcache[EB.m_slrPointB.m_ix],EB.m_slrPointB.m_iy) )
				{
					BVcache[EB.m_slrPointB.m_ix].push_back(EB.m_slrPointB.m_iy);
					VSetB.push_back(EB.m_slrPointB);
				}
			}
		}
	}

	void GetCandidateSignatureRelease( edgeList &AEdgeSet,edgeList &BEdgeSet,ptList &VSetA, ptList &VSetB, std::string picA, std::string picB, std::string className )
	{
		//找在图像A特征点上所有符合约束条件的三角形，并生成签名
		const int MAX_TRI_NUM = 10000000;
		const int MAX_HIT_NUM = 3000;
		int iHitNum = 0;
		//std::map<long long, long long> llmap;
		std::set<long long> lcache;
		//__gnu_cxx::hash_map<long long, long long> llmap;
			//处理图像B
		for (std::list<LogRec::LogoSLRPoint>::iterator vitrb = VSetB.begin();
			vitrb != VSetB.end(); vitrb++)
		{
			for	(std::list<LogRec::LogoSLREdge>::iterator eitrb = BEdgeSet.begin();
				eitrb != BEdgeSet.end(); eitrb++)
			{
			
				LogRec::LogoTri lsltri(*eitrb,*vitrb);
				if (!LogRec::IsTriHasSameLabel(lsltri) && LogRec::IsInProportion(lsltri)
					&& LogRec::IsLargerThanShortestLen(lsltri,CTriBase::getGlobalParams().m_eMin)) 	
				{
					LogRec::LogoGetTtriAngles(lsltri);
					if (IsLargeThanSmallestAngle(lsltri,CTriBase::getGlobalParams().m_triMinAngle) 
						&& (LogRec::LogoGetTriangleArea(lsltri) < CTriBase::getGlobalParams().m_triMaxArea))
					{
						LogRec::LogoGetTriRelaOri(lsltri);
						LogRec::LogoPackTriInfoToSignature(lsltri,CTriBase::getGlobalParams().m_binSizeOfTriDir,CTriBase::getGlobalParams().m_binSizeOfOriDir);
						//llmap[lsltri.m_signature] = lsltri.m_signature;
						lcache.insert(lsltri.m_signature);
						LogRec::LogoSLREdge eBA(lsltri.m_sEdge.m_slrPointB,lsltri.m_sEdge.m_slrPointA);
						LogRec::LogoTri ltr3(eBA,lsltri.m_sPoint);
						LogRec::LogoGetTtriAngles(ltr3);
						LogRec::LogoGetTriRelaOri(ltr3);
						LogRec::LogoPackTriInfoToSignature(ltr3,CTriBase::getGlobalParams().m_binSizeOfTriDir,CTriBase::getGlobalParams().m_binSizeOfOriDir);
					  lcache.insert(lsltri.m_signature);
						iHitNum++;
						if (iHitNum > MAX_TRI_NUM)
						{
							break;
						}
					//llmap[ltr3.m_signature] = ltr3.m_signature;					
					}
				}
			}

			if (iHitNum > MAX_TRI_NUM)
			{
				break;
			}
		}

		int hit = 0;
		for (std::list<LogRec::LogoSLRPoint>::iterator vitr = VSetA.begin();
				vitr != VSetA.end(); vitr++)
		{
		  int count = 0; 
			for	(std::list<LogRec::LogoSLREdge>::iterator eitr = AEdgeSet.begin();
				eitr != AEdgeSet.end(); eitr++)
			{			
				LogRec::LogoTri lsltri(*eitr,*vitr);
				//****约束条件1：组成三角形的三个特征点不能有相同的label;
				//****约束条件2：三条边的比例范围在[1/3,3]	
				//****约束条件3: 最短边长大于 5 pixel
				//****约束条件4: 最小角度大于20°
				//****约束条件5：最大面积不超过20000 pixel²
				if ( !LogRec::IsTriHasSameLabel(lsltri) && LogRec::IsInProportion(lsltri )
					&& LogRec::IsLargerThanShortestLen(lsltri,CTriBase::getGlobalParams().m_eMin))					
				{
					LogRec::LogoGetTtriAngles(lsltri);
					if ((IsLargeThanSmallestAngle(lsltri,CTriBase::getGlobalParams().m_triMinAngle)) 
					    && (LogRec::LogoGetTriangleArea(lsltri) < CTriBase::getGlobalParams().m_triMaxArea))
					{
						LogRec::LogoGetTriRelaOri(lsltri);
						//三角形三个内角和三个特征点相对角度的矢量化的bin size分别为5和10
						//每个三角形的六个签名
						//ABC

						LogRec::LogoPackTriInfoToSignature(lsltri,CTriBase::getGlobalParams().m_binSizeOfTriDir,CTriBase::getGlobalParams().m_binSizeOfOriDir);
						if (lcache.find(lsltri.m_signature) != lcache.end())
						{
							//model.add_triIndex(lsltri.m_signature);
							CTriTrain::GetModel().add_triIndex(lsltri.m_signature);
							//分别插入三条边的索引Index
							LogRec::LogoSLREdge eAB(lsltri.m_sEdge);
							LogRec::LogoGetEdgeSig(eAB,CTriBase::getGlobalParams().m_binSizeOfOriDir);
							//model.add_edgeIndex(eAB.m_edgeSig);
							//if ()
							CTriTrain::GetModel().add_edgeIndex(eAB.m_edgeSig);
								//llTTmap[ppltra->m_signature] = *ppltra;
						
							LogRec::LogoSLREdge eBC(lsltri.m_sEdge.m_slrPointB,lsltri.m_sPoint);
							LogRec::LogoGetEdgeRelativeAngles(eBC);
							LogRec::LogoGetEdgeSig(eBC,CTriBase::getGlobalParams().m_binSizeOfOriDir);
							//model.add_edgeIndex(eBC.m_edgeSig);
							CTriTrain::GetModel().add_edgeIndex(eBC.m_edgeSig);

							LogRec::LogoSLREdge eCA(lsltri.m_sPoint,lsltri.m_sEdge.m_slrPointA);
							LogRec::LogoGetEdgeRelativeAngles(eCA);
							LogRec::LogoGetEdgeSig(eCA,CTriBase::getGlobalParams().m_binSizeOfOriDir);
							//model.add_edgeIndex(eCA.m_edgeSig);
							CTriTrain::GetModel().add_edgeIndex(eCA.m_edgeSig);
							
							LogRec::LogoSLREdge eBA(lsltri.m_sEdge.m_slrPointB,lsltri.m_sEdge.m_slrPointA);
							LogRec::LogoTri ltr3(eBA,lsltri.m_sPoint);
							LogRec::LogoGetTtriAngles(ltr3);
							LogRec::LogoGetTriRelaOri(ltr3);
							LogRec::LogoPackTriInfoToSignature(ltr3,CTriBase::getGlobalParams().m_binSizeOfTriDir,CTriBase::getGlobalParams().m_binSizeOfOriDir);

						//model.add_triIndex(ltr3.m_signature);
							CTriTrain::GetModel().add_triIndex(ltr3.m_signature);
						//m_triIndex.insert(ltr5.m_signature);					
							count++;
							hit += 2;
							if (count > CTriBase::getGlobalParams().m_maxReTri)
							{
									break;
							}
						}
					}
				}
			}
		}

	//void GetCandidateSignatureRelease( edgeList &AEdgeSet,edgeList &BEdgeSet,ptList &VSetA, ptList &VSetB, SModel& model )
		LogRec::LOG_SET_OPTION(LogRec::LOG_OPTION_INFO);
		LogRec::WRITE_LOG_MESSAGE("[class]:%s [picA]:%s [picB]:%s [EdgeA]:%d [EdgeB]:%d [VA]:%d [VB]:%d  [HIT]:%d", className.c_str(), picA.c_str(), picB.c_str(), AEdgeSet.size(), BEdgeSet.size(),VSetA.size(),VSetB.size(),hit);
		LogRec::LOG_SET_OPTION(LogRec::LOG_OPTION_ALL);
		cout << "hit:" << hit << endl;
	}

	void ImgsPairTraining(LogoSTClass::iterator &itr, LogoSTClass::iterator &sitr, LogoFPSTClass::iterator &ditr, LogoFPSTClass::iterator &sditr, std::string className)
	{

		//std::list<LogRec::LogoTri> ltriA;
		//std::list<LogRec::LogoTri> ltriB;
		//用hash表来存放签名值
		//std::hash_set<long long> llset;
		//std::map<long long, long long> llmap;

		//llmap的同签名值映射
		//std::map<long long, LogRec::LogoTri> llTTmap;
		//std::set<long long> ltrset;

		//有相同label的点存放在一起，map的key作为特征点的label值，value为所有这样点的集合，用list来表示
		LogPicVW PSetA;
		LogPicVW PSetB;

		//用来提高精度和加速的结构
		LogPicVWFP PVWSetA;
		LogPicVWFP PVWSetB;
		
		//初始的候选边集合
		std::list<LogRec::LogoSLREdge> EdgeSetA;
		std::list<LogRec::LogoSLREdge> EdgeSetB;
		/***
		 *  找出两个特征点集合中有相同label的子集合
		**/
		//GetSubSetOfSameLabelOfPairImgs(PSetA,PSetB,fitr,sitr,PVWSetA,PVWSetB,ffitr,fsitr);

		LogPicVWFP::iterator vfmitr = ditr->second.begin();
		for (LogPicVW::iterator ffitr = itr->second.begin(); ffitr != itr->second.end(); ffitr++,vfmitr++)
		{
			if ( sitr->second.find(ffitr->first) != (sitr->second.end()) )//need to be verified in the future!
			{
				PSetA[ffitr->first] = ffitr->second;
				//PSetA;
				PVWSetA[vfmitr->first] = vfmitr->second;

				PSetB[ffitr->first] = sitr->second[ffitr->first];

				PVWSetB[vfmitr->first] = sditr->second[vfmitr->first];

				if (ffitr->second.size() > 300 || sitr->second[ffitr->first].size() > 300) 
				{
					LogRec::LOG_LEVEL_WARN;
					LogRec::PRINT_LOG_MESSAGE("SIZE OF VISUAL %d IN IMGA: %d IN IMGB:%d", ffitr->first,            
						ffitr->second.size(),sitr->second[ffitr->first].size());
				}
			}
		}


		//找出满足条件的候选边集合,即边上对应点label相同，相对角度也相同
		GetInitCandidateEdge( PSetA,PSetB,PVWSetA,PVWSetB,EdgeSetA,EdgeSetB );
		
		LogRec::LOG_LEVEL_INFO;
		LogRec::PRINT_LOG_MESSAGE( "SIZE OF INIT CANDIDATE EDGE A:%d  B:%d",EdgeSetA.size(),EdgeSetB.size() );
		//std::cout << "initial NUM edges A: " << EdgeSetA.size() << " B:" << EdgeSetB.size() << std::endl;

		std::list<LogRec::LogoSLREdge> AEdgeSet;
		std::list<LogRec::LogoSLREdge> BEdgeSet;

		//存放对应的EdgeSet中点
		std::list<LogRec::LogoSLRPoint> VSetA;
		std::list<LogRec::LogoSLRPoint> VSetB;


		//获取候选边和边中的点
		getCandidatePoints( EdgeSetA, EdgeSetB, VSetA, VSetB );
		//GetCandidatePointAndEdge( EdgeSetA, EdgeSetB, AEdgeSet, BEdgeSet, VSetA, VSetB );

		LogRec::LOG_LEVEL_INFO;
		LogRec::PRINT_LOG_MESSAGE( "SIZE OF CANDIDATE EDGE A:%d  B:%d PA:%d PB:%d",EdgeSetA.size(),EdgeSetB.size(),VSetA.size(),VSetB.size() );
		//std::cout << "candidate NUM edges A: " << AEdgeSet.size() << " B:" << BEdgeSet.size() << std::endl;

		//std::map<long long,long long> llvsig;
		//获取候选签名
		SModel model;

		GetCandidateSignatureRelease(EdgeSetA,EdgeSetB, VSetA, VSetB,itr->first,sitr->first,className);

		//model.saveModel(CTriBase::getGlobalConf().m_sModelDataPath,className);

	}

	bool SingleClassTraining( std::string className,LogoSTClass &sFeature, LogoFPSTClass &sDFeature )
	{
		//结对 n(n-1)/2 times 或者 n^2 次
		LogoFPSTClass::iterator ditr = sDFeature.begin();
		for (LogoSTClass::iterator itr = sFeature.begin(); itr != sFeature.end(); itr++,ditr++)
		{
			LogoFPSTClass::iterator ft = ditr;//sDFeature.begin(); //ft = ++ft, 
			LogoFPSTClass::iterator fditr = ++ft; 
			LogoSTClass::iterator ftr = itr; 
			for (LogoSTClass::iterator sitr = ++ftr; sitr != sFeature.end() ;sitr++, fditr++)
			{
				//存放符合要求的三角形
				if (itr != sitr)
				{
					LogRec::LOG_LEVEL_INFO;
					LogRec::PRINT_LOG_MESSAGE("NOW TRAINING PIC: %s AND PIC: %s ",itr->first.c_str(),sitr->first.c_str());
					ImgsPairTraining(itr,sitr,ditr,fditr,className);		
				}
			}
		}		
		return true;
	}


	bool CTriTrain::ExtractFeatures(std::string className, std::vector<std::string> &sLogoList)
	{		
		/**
		*
		*存放某一类logo的所有训练数的据特征提取信息，除了描述点以外
		*包括该类每一张图片所提取出来的除了descriptor以外的其他附加信息
		*
		**/
		LogoSTClass sFeature;	
		//结构同上，但只有描述点、签名(visual word)、和特征点坐标(descriptor)
		//作用：图片A中的每一个特征点，找出B中与之lable相同的其欧式距离最近的n个点
		LogoFPSTClass sDFeature;

		std::vector<LogRec::LogoTri> ltri;
		//存放所有logo及其相应的label的特征点
		
		for (std::vector<std::string>::iterator itr = sLogoList.begin(); itr != sLogoList.end(); itr++)
		{
			string fullNamePath = m_gConf.m_sDataSetPath+className+"/"+(*itr);
			//根据路径抽取图片特征点
			const char* image_filename = fullNamePath.c_str();
			Mat image = imread(fullNamePath, CV_LOAD_IMAGE_GRAYSCALE);//CV_LOAD_IMAGE_GRAYSCALE

			///######################
			if( !image.data )
			{
				LogRec::LOG_LEVEL_ERROR;
				LogRec::PRINT_LOG_MESSAGE("--(!) Error read image %s \n",itr->c_str());
				return false;
			}	
			///######################
//#define _SURF_

//#undef	_SURF_

#ifdef	_SURF_
			int hessian = 500;
			SurfFeatureDetector detector( hessian );
			SurfDescriptorExtractor extractor;
			//SurfDescriptorExtractor extractor(4,2,1);
			Mat descriptors;
			LogRec::LOG_LEVEL_INFO;
			LogRec::PRINT_LOG_MESSAGE("--Use Surf");
#else
			//SiftFeatureDetector detector;
			SiftFeatureDetector *detector = new SiftFeatureDetector;
			//SiftFeatureDetector detector(0.01,0.5,6,5);
			SiftDescriptorExtractor extractor;
			Mat descriptors;
			LogRec::LOG_LEVEL_INFO;
			LogRec::PRINT_LOG_MESSAGE("--Use Sift extractor and descriptors--");
			//LogRec::WRITE_LOG_MESSAGE("--Use Sift");
#endif
			std::vector<KeyPoint> keypoints;
			detector->detect( image,keypoints );
			

			if(keypoints.size() < 300)
			{
				delete detector;
				keypoints.clear();
				detector = new SiftFeatureDetector(0.01,0.5,6,5);
				detector->detect( image,keypoints );
			}
			else	if(keypoints.size() < 500)
			{
				delete detector;
				keypoints.clear();
				detector = new SiftFeatureDetector(0.02,0.5,6,5);
				detector->detect( image,keypoints );
			}
			else if(keypoints.size() < 700)
			{
				delete detector;
				keypoints.clear();
				detector = new SiftFeatureDetector(0.02,4,6,5);
				detector->detect( image,keypoints );
			}
			else if(keypoints.size() < 800)
			{
				delete detector;
				keypoints.clear();
				detector = new SiftFeatureDetector(0.03,5,6,5);
				detector->detect( image,keypoints );
			}
			else
			{
			  ;
			}
			
			extractor.compute( image, keypoints, descriptors );
			
			int copyDescriptors = 0;
			unsigned int threshold = 10000;			
			copyDescriptors = descriptors.rows;
			if (keypoints.size() > threshold)
			{
				LogRec::PRINT_LOG_MESSAGE("PIC:%s's KEYPOINTS SIZE:%d", image_filename,keypoints.size());
			  copyDescriptors = threshold;
			}

			
			int length = SIFT_DIMENSION;//keypoints.size();
			/*
			if (descriptors.rows > threshold)
			{
				LogRec::WRITE_LOG_MESSAGE("PIC:%s's DESCRIPTOR SIZE:%d", image_filename,descriptors.rows);
				copyDescriptors = threshold;
				m_gParams.m_iMaxComNum = 10;
			}
			else
			{
				copyDescriptors = descriptors.rows;
				m_gParams.m_iMaxComNum = 100;
			}
			*/
			
			//std::vector<LogRec::LogoPoint> vlp;
			//std::vector<LogRec::LogoCodeBookFormat> vDecriptors;
				//提取到矩阵里
			//关闭日志的其他信息(文件、行和时间)
			LogRec::LOG_LEVEL_INFO;
			LogRec::PRINT_LOG_MESSAGE("PIC: %s HAS %d FEATURES\n",itr->c_str(),descriptors.rows);

			LogRec::LOG_SET_OPTION(LogRec::LOG_OPTION_INFO);
			for (int i = 0; i < copyDescriptors; i++)
			{
				LogRec::LogoSLRPoint slrp;
				LogRec::LogFPtF lfpf;
				//const float* descriptor = (const float*)img_reader.ptr;
				float* p_imgPtr = (float*)descriptors.ptr<float>(i);
				//method one
				/*for (int j = 0; j < descriptors.cols; j++)
				{
					lfpf.m_fDescriptor[j] = p_imgPtr[j];
				}*/
				memcpy(lfpf.m_fDescriptor, p_imgPtr, length*sizeof(float));

				//把特征点方向区间由(-180,180)转化到(0,360)
				if (keypoints[i].angle < 0.0)
				{
					keypoints[i].angle += 360;
				}

				CvSURFPoint cvsurf = KeyPointToSurfPoint( keypoints[i] );
				
				/*if (keypoints[i].angle > 180 || keypoints[i].angle < -180)
				{
					LogRec::PRINT_LOG_MESSAGE("CURRENT POINT ANGLE %f", keypoints[i].angle);
				}*/

				int visualWord = getVisualWordCode(p_imgPtr);

				slrp = LogRec::logoSLRPoint(cvsurf,visualWord);

				lfpf.m_ix = slrp.m_ix;
				lfpf.m_iy = slrp.m_iy;
				lfpf.m_iSig = slrp.m_uVisualWord;

				if (i%500 == 0)
				{
					LogRec::PRINT_LOG_MESSAGE("--THE---%d ROUND [POINTS(%f,%f) SIZE(%d) ORI(%f)]--",
						i,cvsurf.pt.x,cvsurf.pt.y,cvsurf.size,cvsurf.dir);
				}	

				sFeature[*itr][visualWord].push_back(slrp);
				sDFeature[*itr][visualWord].push_back(lfpf);
			}
			LogRec::LOG_SET_OPTION(LogRec::LOG_OPTION_ALL);
		}

		LogRec::PRINT_LOG_MESSAGE("NOW BEGIN SCLASSTRAINING!");
		SingleClassTraining( className, sFeature,  sDFeature );
		
		LogRec::CTriTrain::SaveSingleModel(className);
		LogRec::CTriTrain::GetModel().clearModel();

		return true;
	}


	bool CTriTrain::Train()
	{
		for (std::map<std::string,std::vector<std::string> >::iterator itr = m_mLogoList.begin(); 
			itr != m_mLogoList.end(); itr++)
		{
			LogRec::LOG_LEVEL_INFO;
			LogRec::PRINT_LOG_MESSAGE("class: %s ",itr->first.c_str());

			for (std::vector<string>::iterator vtr = itr->second.begin(); vtr != itr->second.end(); vtr++)
			{
				LogRec::LOG_LEVEL_INFO;
				LogRec::PRINT_LOG_MESSAGE("%s ",(*vtr).c_str());
				//std::cout << *vtr << "  ";
			}

			ExtractFeatures( itr->first, itr->second );
		}
		return true;
	}

	bool CTriTrain::Init()
	{
		if ( !ReadTrainSRLFileList() )
		{
			LogRec::LOG_LEVEL_ERROR
			LogRec::PRINT_LOG_MESSAGE("Read train file list failed!");
			return false;
		}
		if ( !LoadCodeBook() )
		{
			LogRec::LOG_LEVEL_ERROR
			LogRec::PRINT_LOG_MESSAGE("LoadCodeBook failed!");
			return false;
		}
		if ( !Train() )
		{
			LogRec::LOG_LEVEL_ERROR
			LogRec::PRINT_LOG_MESSAGE("PairTraining failed!");
			return false;
		}
		return true;
	}

	bool CTriTrain::ReadTrainSRLFileList()
	{
		ifstream fin((m_gConf.m_sConfPath+m_gConf.m_sTrainList).c_str());

		if (!fin)
		{
			LogRec::LOG_LEVEL_ERROR
			LogRec::PRINT_LOG_MESSAGE("failed to open trainListfile! ");
			return false;
		}

		std::string line;

		while(std::getline(fin,line))
		{
			std::istringstream is;
			is.str(line.c_str());
			std::string sClass;
			std::string sName;
			is >> sClass;
			is >> sName;
			m_mLogoList[sClass].push_back(sName);
		}

		return true;
	}

	CTriAngleQuery::CTriAngleQuery()
	{
	}

	CTriAngleQuery::~CTriAngleQuery()
	{
	}

	bool CTriAngleQuery::LoadModel()
	{
		std::ifstream ifcin;

		ifcin.open((m_gConf.m_sModelDataPath+m_gConf.m_sModeListlFile).c_str(),std::ios_base::in);

		if (!ifcin)
		{
			LogRec::LOG_LEVEL_ERROR;//(stderr,"open modelfile failed!\n");
			LogRec::PRINT_LOG_MESSAGE("OPEN MODEL FILE FAILED!");
			return false;
		}

		std::string sClassName;
		//读入模型
		while (ifcin >> sClassName)
		{
			m_sModelList.push_back(sClassName);

			std::ifstream ifEdge((m_gConf.m_sModelDataPath+sClassName+"_EDGE_INDEX.txt").c_str());
			//ifEdge.open((sClassName+"_edge.txt").c_str());
			std::ifstream ifTri((m_gConf.m_sModelDataPath+sClassName+"_TRI_INDEX.txt").c_str());
			//ifTri.open((sClassName+"_tri.txt").c_str());

			if (!ifEdge || !ifTri)
			{
				LogRec::LOG_LEVEL_ERROR;//(stderr,"open modelfile failed!\n");
				LogRec::PRINT_LOG_MESSAGE("OPEN MODEL FILE: %s FAILED!",sClassName.c_str());
				//fprintf(stderr,"open %s failed!\n", sClassName.c_str());
				ifEdge.close();
				ifTri.close();
				continue;
			}

			std::string line;
			while (std::getline(ifEdge,line))
			{
				std::string sLogo;
				std::istringstream iStrGetter;
				iStrGetter.str(line.c_str());
				iStrGetter >> sLogo;
				long lEdgeSig;
				while (iStrGetter >> lEdgeSig)
				{
					m_edgeIndex[sLogo].insert(lEdgeSig);
				}

			}
			while (std::getline(ifTri,line))
			{
				std::string sLogo;
				std::istringstream iStrGetter;
				iStrGetter.str(line.c_str());
				iStrGetter >> sLogo;
				long long llTriSig;
				while (iStrGetter >> llTriSig)
				{
					m_triIndex[sLogo].insert(llTriSig);
				}
			}
			ifEdge.close();
			ifTri.close();
		}

		ifcin.close();
		//提取图片特征并同时进行矢量量化
		return true;
	}

	bool CTriAngleQuery::getQueryList()
	{
		//std::string qList = m_confList["query_list"];
		ifstream ifin( (m_gConf.m_sConfPath+m_gConf.m_sQueryList).c_str() );
		
		if ( !ifin )
		{
			LogRec::LOG_LEVEL_ERROR;
			LogRec::PRINT_LOG_MESSAGE("OPEN QUERY LIST: %s FAILED!",m_gConf.m_sQueryList.c_str());
		}

		std::string line;
		while (std::getline(ifin,line))
		{
			std::istringstream is;
			is.str(line.c_str());
			std::string sClass;
			std::string sPic;
			is >> sClass >> sPic;
			//is >> sPic;
			m_mQlist[sClass].push_back(sPic);
		}

		return true;
	}

	bool CTriAngleQuery::GetFeatureOfSinglePic(std::string &className)
	{
		//对单个图片的特征进行抽取，进行矢量量化，把descriptor转化为visual word
		std::string sfile = m_gConf.m_sConfPath+m_sQuery;
		const char* imgfile = (const char*)sfile.c_str();
		//std::string className = "adidas";
		string fullNamePath = m_gConf.m_sDataSetPath+className+"/"+m_sQuery;
			//根据路径抽取图片特征点
		const char* image_filename = fullNamePath.c_str();
		Mat image = imread(fullNamePath, CV_LOAD_IMAGE_COLOR);//CV_LOAD_IMAGE_GRAYSCALE

		///######################
		if( !image.data )
		{
			LogRec::LOG_LEVEL_ERROR;
			LogRec::PRINT_LOG_MESSAGE("--(!) Error read image %s \n",m_sQuery.c_str());
			return false;
		}	
			///######################
//#define _SURF_

//#undef	_SURF_

#ifdef	_SURF_
		int hessian = 500;
		SurfFeatureDetector detector( hessian );
		SurfDescriptorExtractor extractor(4,2,1);
		Mat descriptors;
		LogRec::LOG_LEVEL_INFO;
		LogRec::PRINT_LOG_MESSAGE("--Use Surf");
#else
/*		//SiftFeatureDetector detector;
		SiftFeatureDetector detector(0.02,0.5,6,5);
		SiftDescriptorExtractor extractor;
		Mat descriptors;
		LogRec::LOG_LEVEL_INFO;
		LogRec::PRINT_LOG_MESSAGE("--Use Sift");
		//LogRec::WRITE_LOG_MESSAGE("--Use Sift");
#endif
		std::vector<KeyPoint> keypoints;
		detector.detect( image,keypoints );

		extractor.compute( image, keypoints, descriptors );
		int length = SIFT_DIMENSION;//keypoints.size();
		int copyDescriptors = 0;
*/ /////

//SiftFeatureDetector detector;
			SiftFeatureDetector *detector = new SiftFeatureDetector;
			//SiftFeatureDetector detector(0.01,0.5,6,5);
			SiftDescriptorExtractor extractor;
			Mat descriptors;
			LogRec::LOG_LEVEL_INFO;
			LogRec::PRINT_LOG_MESSAGE("--Use Sift extractor and descriptors--");
			//LogRec::WRITE_LOG_MESSAGE("--Use Sift");
#endif
			std::vector<KeyPoint> keypoints;
			detector->detect( image,keypoints );
			

			if(keypoints.size() < 300)
			{
				delete detector;
				keypoints.clear();
				detector = new SiftFeatureDetector(0.01,0.5,6,5);
				detector->detect( image,keypoints );
			}
			else	if(keypoints.size() < 500)
			{
				delete detector;
				keypoints.clear();
				detector = new SiftFeatureDetector(0.02,0.5,6,5);
				detector->detect( image,keypoints );
			}
			else if(keypoints.size() < 700)
			{
				delete detector;
				keypoints.clear();
				detector = new SiftFeatureDetector(0.02,4,6,5);
				detector->detect( image,keypoints );
			}
			else if(keypoints.size() < 800)
			{
				delete detector;
				keypoints.clear();
				detector = new SiftFeatureDetector(0.03,5,6,5);
				detector->detect( image,keypoints );
			}
			else
			{
			  ;
			}
			unsigned int threshold = 5000;
			if (keypoints.size() > threshold)
			{
				LogRec::PRINT_LOG_MESSAGE("PIC:%s's KEYPOINTS SIZE:%d", image_filename,keypoints.size());
			}

			extractor.compute( image, keypoints, descriptors );
			
			int length = SIFT_DIMENSION;//keypoints.size();
			int copyDescriptors = 0;

////
		//std::vector<LogRec::LogoPoint> vlp;
		//std::vector<LogRec::LogoCodeBookFormat> vDecriptors;
			//提取到矩阵里
		//关闭日志的其他信息(文件、行和时间)


		LogRec::LOG_LEVEL_INFO;

		LogRec::LOG_SET_OPTION(LogRec::LOG_OPTION_INFO);
		for (int i = 0; i < descriptors.rows; i++)
		{
			LogRec::LogoSLRPoint slrp;
			//LogRec::LogFPtF lfpf;
			//const float* descriptor = (const float*)img_reader.ptr;
			float* p_imgPtr = (float*)descriptors.ptr<float>(i);
			//method one
			/*for (int j = 0; j < descriptors.cols; j++)
			{
				lfpf.m_fDescriptor[j] = p_imgPtr[j];
			}*/
			//memcpy(lfpf.m_fDescriptor, p_imgPtr, length*sizeof(float));
			//把特征点方向区间由(-180,180)转化到(0,360)
			if (keypoints[i].angle < 0.0)
			{
				keypoints[i].angle += 360;
			}
			CvSURFPoint cvsurf = KeyPointToSurfPoint( keypoints[i] );
				
			/*if (keypoints[i].angle > 180 || keypoints[i].angle < -180)
			{
				LogRec::PRINT_LOG_MESSAGE("CURRENT POINT ANGLE %f", keypoints[i].angle);
			}*/

			int visualWord = getVisualWordCode(p_imgPtr);

			slrp = LogRec::logoSLRPoint(cvsurf,visualWord);

			m_vPicFeature.push_back(slrp);

			if (i%200 == 0)
			{
				LogRec::PRINT_LOG_MESSAGE("--THE---%d ROUND [POINTS(%f,%f) SIZE(%d) ORI(%f)]--",
					i,cvsurf.pt.x,cvsurf.pt.y,cvsurf.size,cvsurf.dir);

				/*LogRec::WRITE_LOG_MESSAGE("TTTT--THE---%d ROUND [POINTS(%f,%f) SIZE(%d) ORI(%f)]--",
					i,cvsurf.pt.x,cvsurf.pt.y,cvsurf.size,cvsurf.dir);
				std::cout << "---THE--- " << i << " ROUND" 
						    << "POINTS: (" <<  lfpf.m_ix << "," 
							<< lfpf.m_iy << ") " << "size:" << cvsurf.size 
							<< " ori: " << cvsurf.dir << std::endl;*/
			}
		}
		LogRec::LOG_SET_OPTION(LogRec::LOG_OPTION_ALL);
		return true;
	}

	bool CTriAngleQuery::GetSimpleRandomEdge()
	{
		int totalSize = m_vPicFeature.size()*m_vPicFeature.size();

		/**/if ( totalSize < m_gParams.m_iMontCarloDensity )
		{
			for ( int i = 0; i < m_vPicFeature.size(); i++ )
			{
				for ( int j= 0; j < m_vPicFeature.size(); j++ )
				{
					if ( i != j )
					{
						m_vRandEdge.push_back(LogRec::LogoSLREdge(m_vPicFeature[i],m_vPicFeature[j]));
					}
				}
			}
		}
		else
		{
			srand(time(NULL));
			for ( int i = 0; i < m_gParams.m_iMontCarloDensity; i++ )
			{
				int x = (rand())%(m_vPicFeature.size());
				int y = (rand())%(m_vPicFeature.size());
				if (x == y)
				{  
					if ((x+1) < m_vPicFeature.size())
					{
						m_vRandEdge.push_back( LogRec::LogoSLREdge(m_vPicFeature[x], m_vPicFeature[y+1]));
					}
				}
				else
				{
					m_vRandEdge.push_back( LogRec::LogoSLREdge(m_vPicFeature[x], m_vPicFeature[y]) );
				}
			}
		}
		return true;
	}

	bool CTriAngleQuery::getKNNpoints()
	{
		for (std::vector<LogRec::LogoSLRPoint>::iterator first_itr = m_vPicFeature.begin(); first_itr != m_vPicFeature.end();
			first_itr++)
		{
			for (std::vector<LogRec::LogoSLRPoint>::iterator second_itr = first_itr+1; second_itr != m_vPicFeature.end();
				second_itr++)
			{
				float fdis = LogRec::LogoGetTwoPointsDistance(*first_itr, *second_itr);
				if ((fdis > m_gParams.m_minNN) && (fdis < m_gParams.m_maxNN))
				{
					m_vRandEdge.push_back(LogRec::LogoSLREdge(*first_itr, *second_itr));
				}
			}
		}
		return true;
	}

	bool CTriAngleQuery::GetCandidateQueryEdge()
	{
		for (std::vector<LogRec::LogoSLREdge>::iterator itr = m_vRandEdge.begin(); itr != m_vRandEdge.end(); itr++)
		{
			LogRec::LogoGetEdgeRelativeAngles(*itr);
			LogRec::LogoGetEdgeSig(*itr,LogRec::fToi(m_gParams.m_binSizeOfOriDir));
			for (std::map<std::string, std::set<long> >::iterator mitr = m_edgeIndex.begin(); mitr != m_edgeIndex.end(); mitr++)
			{
				if (mitr->second.find(itr->m_edgeSig) != mitr->second.end())
				{
					m_vCandidateEdge.push_back(*itr);
					if (!IsPointAlreadyIn(m_vCandidatePoint,itr->m_slrPointA))
					{
						m_vCandidatePoint.push_back(itr->m_slrPointA);
					}
					if (!IsPointAlreadyIn(m_vCandidatePoint,itr->m_slrPointB))
					{
						m_vCandidatePoint.push_back(itr->m_slrPointB);
					}
				}
			}
		}

		return true;
	}

	bool CTriAngleQuery::GetQueryResult()
	{
		for ( std::vector<LogRec::LogoSLRPoint>::iterator itrP = m_vCandidatePoint.begin(); itrP != m_vCandidatePoint.end(); itrP++ )
		{
			for ( std::vector<LogRec::LogoSLREdge>::iterator itrE = m_vCandidateEdge.begin(); itrE != m_vCandidateEdge.end(); itrE++ )
			{
				LogRec::LogoTri logoTri( *itrE,*itrP );
				if ( !LogRec::IsTriHasSameLabel( logoTri ) )
				{
					LogRec::LogoGetTtriAngles( logoTri );
					LogRec::LogoGetTriRelaOri( logoTri );
					LogRec::LogoPackTriInfoToSignature( logoTri,m_gParams.m_binSizeOfTriDir,m_gParams.m_binSizeOfOriDir );
					for ( std::map<std::string, std::set<long long> >::iterator itrTri = m_triIndex.begin(); 
						itrTri != m_triIndex.end(); itrTri++ )
					{
						if ( itrTri->second.find(logoTri.m_signature) != itrTri->second.end())
						{
							if ( m_resultCal.find(itrTri->first) == m_resultCal.end() )
							{
								m_resultCal[itrTri->first] = 1;
							}
							else
							{
								m_resultCal[itrTri->first] ++;
							}
						}
					}
				}
			}
		}
		return true;
	}

	template <typename T>
	std::string ToString(T e)
	{
		std::ostringstream os;
		os << e;
		return std::string(os.str());
	}

	bool printResult(std::map<std::string,int> &m_resultCal, std::vector<std::string> &m_sModelList, std::string &sClass, std::string &sQ)
	{
		std::cout << "The final Statistical results are :" << std::endl;
		std::string sRes = sClass+":"+sQ+" ";
		for (std::vector<std::string>::iterator itrR = m_sModelList.begin(); itrR != m_sModelList.end(); itrR++)
		{
			std::cout << *itrR << " : " 
				      << (m_resultCal.find(*itrR) == m_resultCal.end() ? 0 : m_resultCal[*itrR]) 
					  << std::endl;
			sRes += (*itrR+"="+ToString((m_resultCal.find(*itrR) == m_resultCal.end() ? 0 : m_resultCal[*itrR])))+"|";
		}
		LogRec::LOG_SET_OPTION(LogRec::LOG_OPTION_INFO);
		LogRec::WRITE_LOG_MESSAGE("[res] %s",sRes.c_str());
		LogRec::LOG_SET_OPTION(LogRec::LOG_OPTION_ALL);
		return true;
	}

	bool CTriAngleQuery::clearnCache()
	{
		m_vPicFeature.clear();
		//随机采样的边
		m_vRandEdge.clear();
		//候选边
		m_vCandidateEdge.clear();
		//候选点
		m_vCandidatePoint.clear();
		//统计结果
		m_resultCal.clear();

		return true;
	}

	bool CTriAngleQuery::Query()
	{			
		std::string sClassName;

		LoadCodeBook();

		LoadModel();
//		cout << "please input className and queryPic: [classname] [picname]" << endl;
		getQueryList();

		for (std::map<std::string, std::list<std::string> >::iterator fitr = m_mQlist.begin(); 
			fitr != m_mQlist.end(); fitr++)
		{
			for (std::list<std::string>::iterator sitr = fitr->second.begin();
				sitr != fitr->second.end(); sitr++)
			{
				sClassName = fitr->first;
				m_sQuery = *sitr;
				GetFeatureOfSinglePic(sClassName);
				//LogoGetRandomEdgesByMontCarlo(vPicFeature,vRandEdge);
				//GetRandomEdgeByReserviorSample();
				GetSimpleRandomEdge();

				getKNNpoints();

				GetCandidateQueryEdge();

				GetQueryResult();

				printResult( m_resultCal ,m_sModelList ,sClassName, m_sQuery);

				clearnCache();
			}
		}
		//while(sClassName != "0")
		//{
		//	cout << "please input className and queryPic: [classname] [picname] (end with 0)" << endl;
	
		//	cin >> sClassName >>  m_sQuery;
		//	cout << "class:" << sClassName << " pic:" << m_sQuery << endl;
	
		//	GetFeatureOfSinglePic(sClassName);
		//	//LogoGetRandomEdgesByMontCarlo(vPicFeature,vRandEdge);
		//	GetRandomEdgeByReserviorSample();
	
		//	GetCandidateQueryEdge();

		//	GetQueryResult();

		//	printResult( m_resultCal ,m_sModelList );

		//	clearnCache();
		//}
		return true;
	}

}

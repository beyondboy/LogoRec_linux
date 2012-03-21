#include "Logo_Precomp.hpp"
#include "Logo_Common.hpp"
#include "Logo_Logger.hpp"

namespace LogRec
{
	const   double   PI   =   3.1415926535897932;

	template <typename T>
	std::string LogoToString(T e)
	{
		std::ostringstream os;
		os << e;
		return std::string(os.str());
	}

	template <typename T>
	extern T LogoFromString(std::string s,T yum)
	{
		T e;
		std::istringstream is(s);
		is >> e;
		return e;
	}

	bool LogoCutBlank(std::string &str)
	{
		if (str.empty())
		{
			//std::cerr << "str is empty!" << std::endl;
			LogRec::LOG_LEVEL_WARN
			LogRec::PRINT_LOG_MESSAGE("The str is empty!\n");
			return false;
		}
		for (size_t i = 0; i < str.length(); i++)
		{
			if (isspace(str.at(i)))
			{
				str.erase(i,1);
			}
		}
		return true;
	}

	bool LogoCutBlankBE(std::string &str)
	{
		if (str.empty())
		{
			//std::cerr << "str is empty!" << std::endl;
			LogRec::LOG_LEVEL_WARN
			LogRec::PRINT_LOG_MESSAGE("The str is empty!\n");
			return false;
		}
		while (!str.empty() && isspace(str.at(0)))
		{
			str.erase(0,1);
		}
		if (str.empty())
		{
			return true;
		}
		size_t iEnd = str.size() - 1;
		while (!str.empty() && isspace(str.at(iEnd)))
		{
			str.erase(iEnd,1);
			iEnd = str.size() - 1;
		}
		return true;
	}

	bool LogoIsAllAlpha(const std::string &str)
	{
		size_t len = str.length();
		for (int i = 0; i < len; i++)
		{
			if (!std::isalpha(str.at(i)))
			{
				return false;
			}
		}
		return true;
	}


	LogoSLRPoint logoSLRPoint(CvSURFPoint &surfp,unsigned short visualWord)
	{
		LogoSLRPoint slrp;
		slrp = surfp;
		slrp.m_uVisualWord = visualWord;
		return slrp;
	}

	LogoSLRPoint logoSLRPoint(KeyPoint &keypt,unsigned short visualWord)
	{
		LogoSLRPoint slrp;
		slrp = keypt;
		slrp.m_uVisualWord = visualWord;
		return slrp;
	}

	CvSURFPoint  KeyPointToSurfPoint(KeyPoint &keypt)
	{
		CvSURFPoint cvSurfpt;
		cvSurfpt.pt.x = keypt.pt.x;
		cvSurfpt.pt.y = keypt.pt.y;
		cvSurfpt.dir = keypt.angle;
		cvSurfpt.size = keypt.size;
		cvSurfpt.hessian = 0;
		cvSurfpt.laplacian = 0;
		return cvSurfpt;
	}

	bool LogoIsDigit(const std::string &str)
	{
		size_t len = str.length();
		for (int i = 0; i < len; i++)
		{
			if (!std::isdigit(str.at(i)) || (str.at(i) == '.'))
			{
				return false;
			}
		}
		return true;
	}

	int fToi(float value)
	{
		return (int)(value + (value >= 0 ? 0.5 : -0.5));
	}

	float LogoEuclidean(float fx[], float fy[], int n)
	{
		float res = 0.0;
		for (int i = 0; i < n; i++)
		{
			res += std::pow((fx[i] - fy[i]),2);
		}
		return std::sqrt(res);
	}

	float LogoManhaton(float fx[], float fy[], int n)
	{
		float res = 0.0;
		for (int i = 0; i < n; i++)
		{
			res += std::fabs(fx[i]-fy[i]);
		}
		return res;
	}

	//
	float LogoSimPointScore(float delta,float alta,float sigma)
	{
		return (alta*std::exp(-1*(std::pow(delta,2)/(2*std::pow(sigma,2)))));
	}

	float LogoSimEdgeScore(float deltaI, float deltaJ,float alta,float sigma)
	{
		return (LogoSimPointScore(deltaI,alta,sigma)*LogoSimPointScore(deltaJ,alta,sigma));
	}

	//求边中特征点的相对方向角度(0,180)
	bool LogoGetEdgeRelativeAngles(LogoSLRPoint &ptI, LogoSLRPoint &ptJ, float &ralpha, float &rbeta)
	{
		if (ptI.m_dir > 180.0)
		{
			ptI.m_dir -= 180.0;
		}
		if (ptJ.m_dir > 180.0)
		{
			ptJ.m_dir -= 180.0;
		}
		/*float slopI = std::tanf(dirI*(2*PI/360));
		float slopJ = std::tanf(dirJ*(2*PI/360));*/
		float slopIJ;
		float angleIJ;

		if (ptI.m_ix == ptJ.m_ix)
		{
			angleIJ = 90;
		}
		else
		{
			slopIJ = (ptI.m_iy - ptJ.m_iy)/(ptI.m_ix - ptJ.m_ix);
			angleIJ = (std::atan(slopIJ))*(360/(2*PI));
		}

		
		ralpha = std::abs(ptI.m_dir-angleIJ);
		if (ralpha > 180.0)
		{
			ralpha -= 180.0;
		}

		rbeta = std::abs(ptJ.m_dir-angleIJ);

		if (rbeta > 180.0)
		{
			rbeta -= 180.0;
		}

		return true;
	}

	bool LogoGetEdgeRelativeAnglesSURF(LogRec::LogoSLREdge &slre)
	{
		float angleAB;
		float slopeAB;
		if (slre.m_slrPointA.m_ix == slre.m_slrPointB.m_ix)
		{
			angleAB = 90.0;
		}
		else
		{
			slopeAB = (float)(slre.m_slrPointA.m_iy-slre.m_slrPointB.m_iy)/(float)(slre.m_slrPointA.m_ix-slre.m_slrPointB.m_ix);
			angleAB = (std::atan(slopeAB))*(360/(2*PI));
		}

		if (angleAB < 0.0)
		{//斜率小于0
			float realS = 180 + angleAB;
			float realX = std::abs(angleAB);
			if (slre.m_slrPointA.m_iy < slre.m_slrPointB.m_iy)
			{	//A端在下	
				if (slre.m_slrPointA.m_dir < realS)
				{
					slre.m_rAngleA = 360 - (realS - slre.m_slrPointA.m_dir);
				}
				else
				{
					slre.m_rAngleA = slre.m_slrPointA.m_dir - realS;
				}

				if (slre.m_slrPointB.m_dir < (360 + angleAB))
				{
					slre.m_rAngleB = slre.m_slrPointB.m_dir - angleAB;
				}
				else
				{
					slre.m_rAngleB = realX - (360-slre.m_slrPointB.m_dir);
				}
			}
			else
			{//A端在上
				if (slre.m_slrPointA.m_dir < (360 + angleAB))
				{
					slre.m_rAngleA = slre.m_slrPointA.m_dir - angleAB;
				}
				else
				{
					slre.m_rAngleA = realX - (360-slre.m_slrPointA.m_dir);
				}

				if (slre.m_slrPointB.m_dir < realS)
				{
					slre.m_rAngleB= 360 - (realS - slre.m_slrPointB.m_dir);
				}
				else
				{
					slre.m_rAngleB= slre.m_slrPointB.m_dir - realS;
				}
			}
		}
		else if(angleAB > 0.0)
		{
			float realS = 180 - angleAB;
			float realX = std::abs(angleAB);
			if (slre.m_slrPointA.m_iy > slre.m_slrPointB.m_iy)
			{//A端在上
				
				if (slre.m_slrPointA.m_dir < (360 - realS))
				{
					slre.m_rAngleA = slre.m_slrPointA.m_dir + realS;
				}
				else
				{
					slre.m_rAngleA = realS - (360-slre.m_slrPointA.m_dir);
				}

				if (slre.m_slrPointB.m_dir > angleAB)
				{
					slre.m_rAngleB = slre.m_slrPointB.m_dir - angleAB;
				}
				else
				{
					slre.m_rAngleB = 360 - angleAB + slre.m_slrPointB.m_dir;
				}
			}
			else
			{//A端在下
				if (slre.m_slrPointB.m_dir < (360 - realS))
				{
					slre.m_rAngleB = slre.m_slrPointB.m_dir + realS;
				}
				else
				{
					slre.m_rAngleB = realS - (360-slre.m_slrPointB.m_dir);
				}

				if (slre.m_slrPointA.m_dir > angleAB)
				{
					slre.m_rAngleA = slre.m_slrPointA.m_dir - angleAB;
				}
				else
				{
					slre.m_rAngleA = 360 - angleAB + slre.m_slrPointA.m_dir;
				}
			}
		}
		else
		{//斜率为0的情况
			if (slre.m_slrPointA.m_ix < slre.m_slrPointA.m_ix)
			{
				slre.m_rAngleA = slre.m_slrPointA.m_dir;
				if (slre.m_slrPointB.m_dir < 180)
				{
					slre.m_rAngleB = slre.m_slrPointB.m_dir + 180;
				}
				else
				{
					slre.m_rAngleB = 360 - slre.m_slrPointB.m_dir;
				}
			}
			else
			{
				slre.m_rAngleB = slre.m_slrPointB.m_dir;
				if (slre.m_slrPointA.m_dir < 180)
				{
					slre.m_rAngleA = slre.m_slrPointA.m_dir + 180;
				}
				else
				{
					slre.m_rAngleA = 360 - slre.m_slrPointA.m_dir;
				}
			}
		}

		return true;
	}

	//求边AB的真实相对方向
	bool LogoGetEdgeRelativeAngles(LogRec::LogoSLREdge &slre)
	{
		float angleAB;
		float slopeAB;
		if (slre.m_slrPointA.m_ix == slre.m_slrPointB.m_ix)
		{
			angleAB = 90.0;
		}
		else
		{
			slopeAB = (float)(slre.m_slrPointA.m_iy-slre.m_slrPointB.m_iy)/(float)(slre.m_slrPointA.m_ix-slre.m_slrPointB.m_ix);
			angleAB = (std::atan(slopeAB))*(360/(2*PI));
		}

		if (angleAB < 0.0)
		{//斜率小于0
			float realS = 180 + angleAB;
			float realX = std::abs(angleAB);
			if (slre.m_slrPointA.m_iy < slre.m_slrPointB.m_iy)
			{	//A端在下	
				if (slre.m_slrPointA.m_dir < realS)
				{
					slre.m_rAngleA = 360 - (realS - slre.m_slrPointA.m_dir);
				}
				else
				{
					slre.m_rAngleA = slre.m_slrPointA.m_dir - realS;
				}

				if (slre.m_slrPointB.m_dir < (360 + angleAB))
				{
					slre.m_rAngleB = slre.m_slrPointB.m_dir - angleAB;
				}
				else
				{
					slre.m_rAngleB = realX - (360-slre.m_slrPointB.m_dir);
				}
			}
			else
			{//A端在上
				if (slre.m_slrPointA.m_dir < (360 + angleAB))
				{
					slre.m_rAngleA = slre.m_slrPointA.m_dir - angleAB;
				}
				else
				{
					slre.m_rAngleA = realX - (360-slre.m_slrPointA.m_dir);
				}

				if (slre.m_slrPointB.m_dir < realS)
				{
					slre.m_rAngleB= 360 - (realS - slre.m_slrPointB.m_dir);
				}
				else
				{
					slre.m_rAngleB= slre.m_slrPointB.m_dir - realS;
				}
			}
		}
		else if(angleAB > 0.0)
		{
			float realS = 180 - angleAB;
			float realX = std::abs(angleAB);
			if (slre.m_slrPointA.m_iy > slre.m_slrPointB.m_iy)
			{//A端在上
				
				if (slre.m_slrPointA.m_dir < (360 - realS))
				{
					slre.m_rAngleA = slre.m_slrPointA.m_dir + realS;
				}
				else
				{
					slre.m_rAngleA = realS - (360-slre.m_slrPointA.m_dir);
				}

				if (slre.m_slrPointB.m_dir > angleAB)
				{
					slre.m_rAngleB = slre.m_slrPointB.m_dir - angleAB;
				}
				else
				{
					slre.m_rAngleB = 360 - angleAB + slre.m_slrPointB.m_dir;
				}
			}
			else
			{//A端在下
				if (slre.m_slrPointB.m_dir < (360 - realS))
				{
					slre.m_rAngleB = slre.m_slrPointB.m_dir + realS;
				}
				else
				{
					slre.m_rAngleB = realS - (360-slre.m_slrPointB.m_dir);
				}

				if (slre.m_slrPointA.m_dir > angleAB)
				{
					slre.m_rAngleA = slre.m_slrPointA.m_dir - angleAB;
				}
				else
				{
					slre.m_rAngleA = 360 - angleAB + slre.m_slrPointA.m_dir;
				}
			}
		}
		else
		{//斜率为0的情况
			if (slre.m_slrPointA.m_ix < slre.m_slrPointA.m_ix)
			{
				slre.m_rAngleA = slre.m_slrPointA.m_dir;
				if (slre.m_slrPointB.m_dir < 180)
				{
					slre.m_rAngleB = slre.m_slrPointB.m_dir + 180;
				}
				else
				{
					slre.m_rAngleB = 360 - slre.m_slrPointB.m_dir;
				}
			}
			else
			{
				slre.m_rAngleB = slre.m_slrPointB.m_dir;
				if (slre.m_slrPointA.m_dir < 180)
				{
					slre.m_rAngleA = slre.m_slrPointA.m_dir + 180;
				}
				else
				{
					slre.m_rAngleA = 360 - slre.m_slrPointA.m_dir;
				}
			}
		}

		return true;
	}

		//获取边的签名
	bool LogoGetEdgeSig(LogRec::LogoSLREdge &slre,int binSizeOfFeatureOri)
	{
		//LogoGetEdgeRelativeAngles(slre.m_slrPointA,slre.m_slrPointB,slre.m_rAngleA,slre.m_rAngleB);
		//LogoGetEdgeRelativeAngles(slre);
		unsigned int uA = 0;
		unsigned int uB = 0;
		if (slre.m_slrPointA.m_uVisualWord > slre.m_slrPointB.m_uVisualWord)
		{
			uA = slre.m_slrPointB.m_uVisualWord;
			uB = slre.m_slrPointA.m_uVisualWord;
		}
		else
		{
			
			uA = slre.m_slrPointA.m_uVisualWord;
			uB = slre.m_slrPointB.m_uVisualWord;
		}
			
		int ra = 0;
		int rb = 0;
		if (slre.m_rAngleA > slre.m_rAngleB)
		{
			ra = fToi(slre.m_rAngleB/(float)binSizeOfFeatureOri);
			rb = fToi(slre.m_rAngleA/(float)binSizeOfFeatureOri);
		}
		else
		{
			ra = fToi(slre.m_rAngleA/(float)binSizeOfFeatureOri);
			rb = fToi(slre.m_rAngleB/(float)binSizeOfFeatureOri);			
		}

		long yum = uA;		
		long lsig = 0;
		lsig = lsig | (yum<<20);

		yum = uB;
		lsig = lsig | (yum<<8);

		yum = ra;
		lsig = lsig | (yum<<4);

		yum = rb;
		lsig = lsig | yum;

		slre.m_edgeSig = lsig;

		return true;
	}

	//边的相似度打分
	float LogoGetSimEdgeScore(LogRec::LogoSLREdge &slreA,LogRec::LogoSLREdge &slreB,float alta,float sigma)
	{
		//LogoGetEdgeRelativeAngles(slreA.m_slrPointA,slreA.m_slrPointB,slreA.m_rAngleA,slreA.m_rAngleB);
		LogoGetEdgeRelativeAngles(slreA);
		LogoGetEdgeRelativeAngles(slreB);
		//LogoGetEdgeRelativeAngles(slreB.m_slrPointA,slreB.m_slrPointB,slreB.m_rAngleA,slreB.m_rAngleB);
		return LogoSimEdgeScore(slreA.m_rAngleA - slreB.m_rAngleA, slreA.m_rAngleB - slreB.m_rAngleB,alta,sigma);
	}

	//求三角形每个特征点的相对方向
	bool LogoGetTriRelaOri(LogRec::LogoTri &slrTri)
	{
		float fA;
		float fB;
		LogRec::LogoSLREdge eAB(slrTri.m_sEdge);
		LogRec::LogoSLREdge eBC(slrTri.m_sEdge.m_slrPointB,slrTri.m_sPoint);
		LogRec::LogoSLREdge eCA(slrTri.m_sPoint,slrTri.m_sEdge.m_slrPointA);

		//LogoGetEdgeRelativeAngles(slrTri.m_sEdge.m_slrPointA,slrTri.m_sEdge.m_slrPointB,fA,fB);
		LogoGetEdgeRelativeAngles(eAB);
		slrTri.m_rapha = eAB.m_rAngleA;

		//LogoGetEdgeRelativeAngles(slrTri.m_sEdge.m_slrPointB,slrTri.m_sPoint,fA,fB);
		LogoGetEdgeRelativeAngles(eBC);
		slrTri.m_rbeta = eBC.m_rAngleA;

		//LogoGetEdgeRelativeAngles(slrTri.m_sPoint,slrTri.m_sEdge.m_slrPointA,fA,fB);
		LogoGetEdgeRelativeAngles(eCA);
		slrTri.m_rgama = eCA.m_rAngleA;

		return true;
	}
	//两点之间的距离
    float LogoGetTwoPointsDistance(LogRec::LogoSLRPoint ptA, LogRec::LogoSLRPoint ptB)
	{
		return std::sqrt(std::pow(ptA.m_ix-ptB.m_ix,2)+std::pow(ptA.m_iy-ptB.m_iy,2));
	}

	bool IsLargerThanShortestLen(LogRec::LogoTri & slrTri, float sLen)
	{
		if ((LogoGetTwoPointsDistance(slrTri.m_sEdge.m_slrPointA,slrTri.m_sEdge.m_slrPointB) < sLen)
			|| (LogoGetTwoPointsDistance(slrTri.m_sEdge.m_slrPointA,slrTri.m_sPoint) < sLen) 
			|| (LogoGetTwoPointsDistance(slrTri.m_sEdge.m_slrPointB,slrTri.m_sPoint) < sLen))
		{
			return false;
		}

		return true;
	}
		//判断三边相互之比是否符合(1/3,3的范围)
	bool IsInProportion(LogRec::LogoTri & slrTri)
	{
		float fmin = 1.0/3.0;
		float fmax = 3.0;
		float dAB = LogoGetTwoPointsDistance(slrTri.m_sEdge.m_slrPointA,slrTri.m_sEdge.m_slrPointB);
		float dAC = LogoGetTwoPointsDistance(slrTri.m_sEdge.m_slrPointA,slrTri.m_sPoint);
		float dBC = LogoGetTwoPointsDistance(slrTri.m_sEdge.m_slrPointB,slrTri.m_sPoint);
		float prop_a = dAB/dAC;
		float prop_b = dBC/dAB;
		if (prop_a > fmax || prop_a < fmin
			|| prop_b > fmax || prop_b < fmin)
		{
			return false;
		}
		return true;
	}

	bool IsTriHasSameLabel(LogRec::LogoTri & slrTri)
	{
		if (slrTri.m_sEdge.m_slrPointA.m_uVisualWord == slrTri.m_sEdge.m_slrPointB.m_uVisualWord
			|| slrTri.m_sEdge.m_slrPointA.m_uVisualWord == slrTri.m_sPoint.m_uVisualWord 
			|| slrTri.m_sEdge.m_slrPointB.m_uVisualWord == slrTri.m_sPoint.m_uVisualWord)
		{
			return true;
		}
		return false;
	}

	bool LogoGetTtriAngles(LogRec::LogoTri & slrTri)
	{
		//根据三角形三个顶点坐标求出三个内角
		int vABx = slrTri.m_sEdge.m_slrPointB.m_ix-slrTri.m_sEdge.m_slrPointA.m_ix;
		int vABy = slrTri.m_sEdge.m_slrPointB.m_iy-slrTri.m_sEdge.m_slrPointA.m_iy;
		int vACx = slrTri.m_sPoint.m_ix-slrTri.m_sEdge.m_slrPointA.m_ix;
		int vACy = slrTri.m_sPoint.m_iy-slrTri.m_sEdge.m_slrPointA.m_iy;
		float dAB = LogoGetTwoPointsDistance(slrTri.m_sEdge.m_slrPointA,slrTri.m_sEdge.m_slrPointB);
		float dAC = LogoGetTwoPointsDistance(slrTri.m_sEdge.m_slrPointA,slrTri.m_sPoint);
		float VcosineA = (float)(vABx*vACx + vABy*vACy)/(dAB*dAC);
		slrTri.m_thetaA = (360/(2*PI))*std::acos(VcosineA);

		int vBAx = slrTri.m_sEdge.m_slrPointA.m_ix-slrTri.m_sEdge.m_slrPointB.m_ix;
		int vBAy = slrTri.m_sEdge.m_slrPointA.m_iy-slrTri.m_sEdge.m_slrPointB.m_iy;
		int vBCx = slrTri.m_sPoint.m_ix-slrTri.m_sEdge.m_slrPointB.m_ix;
		int vBCy = slrTri.m_sPoint.m_iy-slrTri.m_sEdge.m_slrPointB.m_iy;
		float dBA = LogoGetTwoPointsDistance(slrTri.m_sEdge.m_slrPointA,slrTri.m_sEdge.m_slrPointB);
		float dBC = LogoGetTwoPointsDistance(slrTri.m_sEdge.m_slrPointB,slrTri.m_sPoint);
		float VcosineB = (float)(vBAx*vBCx + vBAy*vBCy)/(dBA*dBC);
		slrTri.m_thetaB = (360/(2*PI))*std::acos(VcosineB);

		int vCBx = slrTri.m_sEdge.m_slrPointB.m_ix-slrTri.m_sPoint.m_ix;
		int vCBy = slrTri.m_sEdge.m_slrPointB.m_iy-slrTri.m_sPoint.m_iy;

		int vCAx = slrTri.m_sEdge.m_slrPointA.m_ix - slrTri.m_sPoint.m_ix;
		int vCAy = slrTri.m_sEdge.m_slrPointA.m_iy - slrTri.m_sPoint.m_iy;
		float dCB = LogoGetTwoPointsDistance(slrTri.m_sPoint,slrTri.m_sEdge.m_slrPointB);
		float dCA = LogoGetTwoPointsDistance(slrTri.m_sEdge.m_slrPointA,slrTri.m_sPoint);
		float VcosineC = (float)(vCBx*vCAx + vCBy*vCAy)/(dCB*dCA);

		slrTri.m_thetaC = (360/(2*PI))*std::acos(VcosineC);

		return true;
	}

	bool IsLargeThanSmallestAngle(LogRec::LogoTri & slrTri, float sAngle)
	{
		if (slrTri.m_thetaA < sAngle || slrTri.m_thetaB < sAngle || slrTri.m_thetaC < sAngle)
		{
			return false;
		}
		return true;
	}

	bool LogoPackInfoPackInSignature(LogRec::LogoTri & slrTri,int binSizeOfShapeAngle,int binSizeOfFeatureOri)
	{  
		//角度用最小的两个角，按从小到大顺序pack，特征方向角度按从小到大pack
		float theta1 = 0;
		float theta2 = 0;
		float ori1 = 0;
		float ori2 = 0;
		float ori3 = 0;
		unsigned short uA = 0;
		unsigned short uB = 0;
		unsigned short uC = 0;

		if (slrTri.m_thetaA >= slrTri.m_thetaB)
		{
			if (slrTri.m_thetaB >= slrTri.m_thetaC)
			{
				theta1 = slrTri.m_thetaC/binSizeOfShapeAngle;
				theta2 = slrTri.m_thetaB/binSizeOfShapeAngle;
			}
			else if (slrTri.m_thetaA >= slrTri.m_thetaC)
			{
				theta1 = slrTri.m_thetaB/binSizeOfShapeAngle;
				theta2 = slrTri.m_thetaC/binSizeOfShapeAngle;
			}
			else
			{
				theta1 = slrTri.m_thetaB/binSizeOfShapeAngle;
				theta2 = slrTri.m_thetaA/binSizeOfShapeAngle;
			}
		}
		else
		{
			if (slrTri.m_thetaA >= slrTri.m_thetaC)
			{
				theta1 = slrTri.m_thetaC/binSizeOfShapeAngle;
				theta2 = slrTri.m_thetaA/binSizeOfShapeAngle;
			}
			else if (slrTri.m_thetaB >= slrTri.m_thetaC)
			{
				theta1 = slrTri.m_thetaA/binSizeOfShapeAngle;
				theta2 = slrTri.m_thetaC/binSizeOfShapeAngle;
			}
			else
			{
				theta1 = slrTri.m_thetaA/binSizeOfShapeAngle;
				theta2 = slrTri.m_thetaB/binSizeOfShapeAngle;
			}
		}

		if (slrTri.m_rapha >= slrTri.m_rbeta)
		{
			if (slrTri.m_rbeta >= slrTri.m_rgama)
			{
				ori1 = slrTri.m_rgama/binSizeOfFeatureOri;
				ori2 = slrTri.m_rbeta/binSizeOfFeatureOri;
				ori3 = slrTri.m_rapha/binSizeOfFeatureOri;
			}
			else if (slrTri.m_rapha >= slrTri.m_rgama)
			{
				ori1 = slrTri.m_rbeta/binSizeOfFeatureOri;
				ori2 = slrTri.m_rgama/binSizeOfFeatureOri;
				ori3 = slrTri.m_rapha/binSizeOfFeatureOri;
			}
			else
			{
				ori1 = slrTri.m_rbeta/binSizeOfFeatureOri;
				ori2 = slrTri.m_rapha/binSizeOfFeatureOri;
				ori3 = slrTri.m_rgama/binSizeOfFeatureOri;
			}
		}
		else
		{
			if (slrTri.m_rapha >= slrTri.m_rgama)
			{
				ori1 = slrTri.m_rgama/binSizeOfFeatureOri;
				ori2 = slrTri.m_rapha/binSizeOfFeatureOri;
				ori3 = slrTri.m_rbeta/binSizeOfFeatureOri;
			}
			else if (slrTri.m_rbeta >= slrTri.m_rgama)
			{
				ori1 = slrTri.m_rapha/binSizeOfFeatureOri;
				ori2 = slrTri.m_rgama/binSizeOfFeatureOri;
				ori3 = slrTri.m_rbeta/binSizeOfFeatureOri;
			}
			else
			{
				ori1 = slrTri.m_rapha/binSizeOfFeatureOri;
				ori2 = slrTri.m_rbeta/binSizeOfFeatureOri;
				ori3 = slrTri.m_rgama/binSizeOfFeatureOri;
			}
		}


		//保证签名按升序组合
		if (slrTri.m_sEdge.m_slrPointA.m_uVisualWord > slrTri.m_sEdge.m_slrPointB.m_uVisualWord)
		{
			if (slrTri.m_sEdge.m_slrPointA.m_uVisualWord > slrTri.m_sPoint.m_uVisualWord)
			{
				if (slrTri.m_sEdge.m_slrPointB.m_uVisualWord > slrTri.m_sPoint.m_uVisualWord)
				{
					uA = slrTri.m_sPoint.m_uVisualWord;
					uB = slrTri.m_sEdge.m_slrPointB.m_uVisualWord;
					uC = slrTri.m_sEdge.m_slrPointA.m_uVisualWord;
				}
				else
				{
					uA = slrTri.m_sEdge.m_slrPointB.m_uVisualWord;
					uB = slrTri.m_sPoint.m_uVisualWord;
					uC = slrTri.m_sEdge.m_slrPointA.m_uVisualWord;
				}
			}
			else
			{
				uA = slrTri.m_sEdge.m_slrPointB.m_uVisualWord;
				uB = slrTri.m_sEdge.m_slrPointA.m_uVisualWord;
				uC = slrTri.m_sPoint.m_uVisualWord;
			}
		}
		else
		{
			if (slrTri.m_sEdge.m_slrPointA.m_uVisualWord < slrTri.m_sPoint.m_uVisualWord)
			{
				if (slrTri.m_sEdge.m_slrPointB.m_uVisualWord < slrTri.m_sPoint.m_uVisualWord)
				{
					uA = slrTri.m_sEdge.m_slrPointA.m_uVisualWord;
					uB = slrTri.m_sEdge.m_slrPointB.m_uVisualWord;
					uC = slrTri.m_sPoint.m_uVisualWord;
				}
				else
				{
					uA = slrTri.m_sEdge.m_slrPointA.m_uVisualWord;
					uB = slrTri.m_sPoint.m_uVisualWord;
					uC = slrTri.m_sEdge.m_slrPointB.m_uVisualWord;
				}
			}
			else
			{
				uA = slrTri.m_sPoint.m_uVisualWord;
				uB = slrTri.m_sEdge.m_slrPointA.m_uVisualWord;
				uC = slrTri.m_sEdge.m_slrPointB.m_uVisualWord;
			}
		}

		long long yum = (long long)uA;		
		long long llsig = 0;
		llsig = llsig | (yum<<52);

		yum = (long long)uB;
		llsig = llsig | (yum<<40);

		yum = (long long)uC;
		llsig = llsig | (yum<<28);

		yum = fToi(theta1);
		llsig = llsig | (yum<<23);

		yum = fToi(theta2);
		llsig = llsig | (yum<<18);

		yum = fToi(ori1);
		llsig = llsig | (yum<<12);

		yum = fToi(ori2);
		llsig = llsig | (yum<<6);

		yum = fToi(ori3);
		llsig = llsig | yum;

		slrTri.m_signature = llsig;

		return true;
	}

	bool LogoPackTriInfoToSignature_old(LogRec::LogoTri & slrTri,int binSizeOfShapeAngle,int binSizeOfFeatureOri)
	{
		float theta1 = 0;
		float theta2 = 0;
		float ori1 = 0;
		float ori2 = 0;
		float ori3 = 0;
		unsigned short uA = 0;
		unsigned short uB = 0;
		unsigned short uC = 0;


		uA = slrTri.m_sEdge.m_slrPointA.m_uVisualWord;
		uB = slrTri.m_sEdge.m_slrPointB.m_uVisualWord;
		uC = slrTri.m_sPoint.m_uVisualWord;

		theta1 = slrTri.m_thetaA/(float)binSizeOfShapeAngle;
		theta2 = slrTri.m_thetaB/(float)binSizeOfShapeAngle;

		ori1 = slrTri.m_rapha/(float)binSizeOfFeatureOri;
		ori2 = slrTri.m_rbeta/(float)binSizeOfFeatureOri;
		ori3 = slrTri.m_rgama/(float)binSizeOfFeatureOri;

		long long yum = (long long)uA;		
		long long llsig = 0;
		llsig = llsig | (yum<<52);

		yum = (long long)uB;
		llsig = llsig | (yum<<40);

		yum = (long long)uC;
		llsig = llsig | (yum<<28);

		yum = fToi(theta1);
		llsig = llsig | (yum<<23);

		yum = fToi(theta2);
		llsig = llsig | (yum<<18);

		yum = fToi(ori1);
		llsig = llsig | (yum<<12);

		yum = fToi(ori2);
		llsig = llsig | (yum<<6);

		yum = fToi(ori3);
		llsig = llsig | yum;

		slrTri.m_signature = llsig;
		return true;
	}

	bool LogoPackTriInfoToSignature(LogRec::LogoTri & slrTri,int binSizeOfShapeAngle,int binSizeOfFeatureOri)
	{
		/*float theta1 = 0;
		float theta2 = 0;
		float ori1 = 0;
		float ori2 = 0;
		float ori3 = 0;
		unsigned short uA = 0;
		unsigned short uB = 0;
		unsigned short uC = 0;*/

		unsigned int uLable[3];
		float	fRelaOri[3];
		float   fTheta[3];

		uLable[0] = slrTri.m_sEdge.m_slrPointA.m_uVisualWord;
		uLable[1] = slrTri.m_sEdge.m_slrPointB.m_uVisualWord;
		uLable[2] = slrTri.m_sPoint.m_uVisualWord;

		fTheta[0] = slrTri.m_thetaA/(float)binSizeOfShapeAngle;
		fTheta[1] = slrTri.m_thetaB/(float)binSizeOfShapeAngle;
		fTheta[2] = slrTri.m_thetaC/(float)binSizeOfShapeAngle;

		fRelaOri[0] = slrTri.m_rapha/(float)binSizeOfFeatureOri;
		fRelaOri[1] = slrTri.m_rbeta/(float)binSizeOfFeatureOri;
		fRelaOri[2] = slrTri.m_rgama/(float)binSizeOfFeatureOri;

		std::sort(uLable,uLable+sizeof(uLable)/sizeof(unsigned int));
		std::sort(fTheta,fTheta+sizeof(fTheta)/sizeof(float));
		std::sort(fRelaOri,fRelaOri+sizeof(fRelaOri)/sizeof(float));

		long long yum = (long long)uLable[0];		
		long long llsig = 0;
		llsig = llsig | (yum<<52);

		yum = (long long)uLable[1];
		llsig = llsig | (yum<<40);

		yum = (long long)uLable[2];
		llsig = llsig | (yum<<28);

		yum = fToi(fTheta[0]);
		llsig = llsig | (yum<<23);

		yum = fToi(fTheta[1]);
		llsig = llsig | (yum<<18);

		yum = fToi(fRelaOri[0]);
		llsig = llsig | (yum<<12);

		yum = fToi(fRelaOri[1]);
		llsig = llsig | (yum<<6);

		yum = fToi(fRelaOri[2]);
		llsig = llsig | yum;

		slrTri.m_signature = llsig;
		return true;
	}

	//获取三角形面积
	float LogoGetTriangleArea(LogRec::LogoTri &slrTri)
	{
		CvPoint2D32f PA = cvPoint2D32f( slrTri.m_sEdge.m_slrPointA.m_ix, slrTri.m_sEdge.m_slrPointA.m_iy );
		CvPoint2D32f PB = cvPoint2D32f( slrTri.m_sEdge.m_slrPointB.m_ix, slrTri.m_sEdge.m_slrPointB.m_iy );
		CvPoint2D32f PC = cvPoint2D32f( slrTri.m_sPoint.m_ix, slrTri.m_sPoint.m_iy );

		float tria = std::abs(cvTriangleArea(PA,PB,PC));

		return tria;
	}
}//end of LogRec namespace

#ifndef	__LOGO_BASE_H__
#define	__LOGO_BASE_H__

namespace LogRec
{

	struct LogFPtF
	{
		int m_ix;
		int m_iy;
		float m_fDescriptor[SIFT_DIMENSION];//DIMENSION_LEN dimension descriptor of a single feature
		int m_iSig;//4 digit (1000,...,9999)
	};

	struct LogParams
	{//实验参数设置
		LogParams()
		: m_alta(1.0), m_sigma(8.0), m_eSimThreshold(0.9), m_binSizeOfTriDir(10.0),
		  m_binSizeOfOriDir(24.0),m_triMinAngle(15.0), m_eMin(10.0), m_triMaxArea(20000.0),
		  m_minProportion(1.0/3.0), m_maxProportion(3.0), m_iMaxComNum(50), 
		  m_iMontCarloDensity(100000), m_minNN(10.0), m_maxNN(30.0), m_maxReTri(8.0)
		{

		}

		LogParams( float alta, float sigma,float eSimThreshold,float binSizeOfTriDir,
			float binSizeOfOriDir,float triMinAngle, float eMin, float triMaxArea,
			float minProportion, float maxProportion ,int iMaxComNum, int iMontCarloDensity, float minNN, float maxNN, float maxReTri)
		: m_alta(alta), m_sigma(sigma), m_eSimThreshold(eSimThreshold), m_binSizeOfTriDir(binSizeOfTriDir),
		  m_binSizeOfOriDir(binSizeOfOriDir),m_triMinAngle(triMinAngle), m_eMin(eMin), m_triMaxArea(triMaxArea),
		  m_minProportion(minProportion), m_maxProportion(maxProportion), m_iMaxComNum(iMaxComNum),
		  m_iMontCarloDensity(iMontCarloDensity), m_minNN(minNN), m_maxNN(maxNN), m_maxReTri(maxReTri)
		{

		}

		float m_alta;
		float m_sigma;
		float m_eSimThreshold;
		float m_binSizeOfTriDir;
		float m_binSizeOfOriDir;
		float m_triMinAngle;
		float m_eMin;
		float m_triMaxArea;
		float m_minProportion;
		float m_maxProportion;
		int m_iMaxComNum;
		int m_iMontCarloDensity;
		float m_minNN;
		float m_maxNN;
		float m_maxReTri;
		//int m_iDimOfDescriptor;
	};

	struct LogoSLRPoint
	{
		LogoSLRPoint()
		{
		}
		LogoSLRPoint(int x, int y)
		: m_ix(x), m_iy(y)
		{
		}

		bool operator == (LogoSLRPoint &olp)
		{
			if (olp.m_ix == m_ix && olp.m_iy == m_iy)
			{
				return true;
			}
			return false;
		}

		void operator = (const LogoSLRPoint &lp)
		{
			m_ix = lp.m_ix;
			m_iy = lp.m_iy;
			m_laplacian = lp.m_laplacian;
			m_size = lp.m_size;
			m_dir = lp.m_dir;
			m_hessian = lp.m_hessian;
			m_uVisualWord = lp.m_uVisualWord;
		}

		void operator = (CvSURFPoint &surfp)
		{
			//最接近的转换
			CvPoint cp = cvPointFrom32f(surfp.pt);
			m_ix = cp.x;
			m_iy = cp.y;
			m_laplacian = surfp.laplacian;
			m_size = surfp.size;
			m_dir = surfp.dir;
			m_hessian = surfp.hessian;
		}
		//add 2.18
		void operator = (KeyPoint &keypt)
		{
			CvPoint cp = cvPointFrom32f(keypt.pt);
			m_ix = cp.x;
			m_iy = cp.y;
			//m_ix = keypt.pt.x;
			//m_iy = keypt.pt.y;
			m_laplacian = 0;//keypt.octave;//?? need to be revised!
			m_size = keypt.size;
			m_dir = keypt.angle;
			m_hessian = 0;//keypt.class_id;
		}

	public:
		int m_ix;
		int m_iy;
	  int m_laplacian;
	  int m_size;
	  float m_dir;
	  float m_hessian;
		unsigned short m_uVisualWord;
	};
	
	struct LogoSLREdge
	{
		LogoSLREdge()
		{
		}

		LogoSLREdge(LogoSLRPoint PA, LogoSLRPoint PB)
		: m_slrPointA(PA), m_slrPointB(PB)
		{
		}

		bool operator < (LogoSLREdge &oslre)
		{
			return oslre.m_slrPointA.m_size < m_slrPointA.m_size;
		}

		bool operator == (LogoSLREdge &oslre)
		{
			return ((oslre.m_slrPointA == m_slrPointA && oslre.m_slrPointB == m_slrPointB)
				|| (oslre.m_slrPointA == m_slrPointB && oslre.m_slrPointB == m_slrPointA));
		}

		void operator = (const LogoSLREdge &oslre)
		{
			m_slrPointA = oslre.m_slrPointA;
			m_slrPointB = oslre.m_slrPointB;
			m_rAngleA = oslre.m_rAngleA;
			m_rAngleB = oslre.m_rAngleB;
			m_edgeSig = oslre.m_edgeSig;
		}

	public:
		LogoSLRPoint m_slrPointA;
		LogoSLRPoint m_slrPointB;
		float m_rAngleA;
		float m_rAngleB;
		long m_edgeSig;
	};

	struct LogoTri
	{
		LogoTri()
		{
		}
		LogoTri(LogoSLREdge &oEdge,LogoSLRPoint &oPoint)
		: m_sEdge(oEdge), m_sPoint(oPoint)
		{
		}
		bool operator = (const LogoTri &lsr)
		{
			m_sEdge = lsr.m_sEdge;
			m_sPoint = lsr.m_sPoint;
			m_thetaA = lsr.m_thetaA;
			m_thetaB = lsr.m_thetaB;
			m_thetaC = lsr.m_thetaC;
			m_rapha = lsr.m_rapha;
			m_rbeta = lsr.m_rbeta;
			m_rgama = lsr.m_rgama;
			m_signature = lsr.m_signature;	
			return true;	
		}
	public:
		LogoSLREdge  m_sEdge;
		LogoSLRPoint m_sPoint;
		float m_thetaA;
		float m_thetaB;
		float m_thetaC;
		float m_rapha;
		float m_rbeta;
		float m_rgama;
		long long m_signature;
	};

	struct LogoCodeBookFormat
	{
		float m_fDescriptor[SIFT_DIMENSION];//DIMENSION_LEN dimension descriptor of a single feature
		int m_iSig;//4 digit (1000,...,9999)
	};

	LogoSLRPoint logoSLRPoint(CvSURFPoint &surfp,unsigned short visualWord);

	LogoSLRPoint logoSLRPoint(KeyPoint &keypt,unsigned short visualWord);

	CvSURFPoint  KeyPointToSurfPoint(KeyPoint &keypt);
	
}

#endif

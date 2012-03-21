#ifndef	__LOGO_COMMON_H__
#define	__LOGO_COMMON_H__

#include "Logo_Base.hpp"

namespace LogRec
{

	template <typename T>
	std::string LogoToString(T e);

	template <typename T>
	T LogoFromString(std::string &s,T yum);

	bool LogoCutBlank(std::string &str);

	bool LogoCutBlankBE(std::string &str);

	bool LogoIsAllAlpha(const std::string &str);

	bool LogoIsDigit(const std::string &str);

	float LogoEuclidean(float fx[], float fy[], int n);

	float LogoManhaton(float fx[], float fy[], int n);
	
	//浮点数转换为整数，按最接近的方式转换
	int fToi(float elem);
	//获取两个图像上两条边的相似性
	
	//获取三角形面积
	float LogoGetTriangleArea(LogRec::LogoTri &slrTri);
	
	float LogoGetSimEdgeScore(LogRec::LogoSLREdge &slreA,LogRec::LogoSLREdge &slreB,float alta,float sigma);

	float LogoSimPointScore(float delta,float alta,float sigma);

	float LogoSimEdgeScore(float deltaI, float deltaJ,float alta,float sigma);
	
	//得到边上特征点的相对方向
	bool LogoGetEdgeRelativeAngles(LogRec::LogoSLRPoint &ptI, LogRec::LogoSLRPoint &ptJ, float &ralpha, float &rbeta);
	
	//获取边和特征点之间的相对角度
	bool LogoGetEdgeRelativeAngles(LogRec::LogoSLREdge &slre);
	//获取边的签名
	bool LogoGetEdgeSig(LogRec::LogoSLREdge &slre,int binSizeOfFeatureOri);
	//获取两点的距离
	float LogoGetTwoPointsDistance(LogRec::LogoSLRPoint ptA, LogRec::LogoSLRPoint ptB);

	//求三角形每个特征点的相对方向
	bool LogoGetTriRelaOri(LogRec::LogoTri &slrTri);

	//获取三角形的三个内角
	bool LogoGetTtriAngles(LogRec::LogoTri & slrTri);
	
	//判断三角形中最短边是否大于sLen个像素
	bool IsLargerThanShortestLen(LogRec::LogoTri & slrTri, float sLen);

	//判断三角形中三个特征点是否有相同的label	
	bool IsTriHasSameLabel(LogRec::LogoTri & slrTri);

	//判断三边相互之比是否符合(1/3,3的范围)
	bool IsInProportion(LogRec::LogoTri & slrTri);

	//判断三角形中最小角度是否大于给定的sAngle
	bool IsLargeThanSmallestAngle(LogRec::LogoTri & slrTri, float sAngle);
	
	//把表征local features 的三角形相关信息pack到64 bit 的signature中
	bool LogoPackInfoPackInSignature(LogRec::LogoTri & slrTri,int binSizeOfShapeAngle,int binSizeOfFeatureOri);

	//严格按论文给的格式进行pack
	bool LogoPackTriInfoToSignature(LogRec::LogoTri & slrTri,int binSizeOfShapeAngle,int binSizeOfFeatureOri);

}//end of LogRec namespace


#endif

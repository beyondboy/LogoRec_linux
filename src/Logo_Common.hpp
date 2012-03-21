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
	
	//������ת��Ϊ����������ӽ��ķ�ʽת��
	int fToi(float elem);
	//��ȡ����ͼ���������ߵ�������
	
	//��ȡ���������
	float LogoGetTriangleArea(LogRec::LogoTri &slrTri);
	
	float LogoGetSimEdgeScore(LogRec::LogoSLREdge &slreA,LogRec::LogoSLREdge &slreB,float alta,float sigma);

	float LogoSimPointScore(float delta,float alta,float sigma);

	float LogoSimEdgeScore(float deltaI, float deltaJ,float alta,float sigma);
	
	//�õ��������������Է���
	bool LogoGetEdgeRelativeAngles(LogRec::LogoSLRPoint &ptI, LogRec::LogoSLRPoint &ptJ, float &ralpha, float &rbeta);
	
	//��ȡ�ߺ�������֮�����ԽǶ�
	bool LogoGetEdgeRelativeAngles(LogRec::LogoSLREdge &slre);
	//��ȡ�ߵ�ǩ��
	bool LogoGetEdgeSig(LogRec::LogoSLREdge &slre,int binSizeOfFeatureOri);
	//��ȡ����ľ���
	float LogoGetTwoPointsDistance(LogRec::LogoSLRPoint ptA, LogRec::LogoSLRPoint ptB);

	//��������ÿ�����������Է���
	bool LogoGetTriRelaOri(LogRec::LogoTri &slrTri);

	//��ȡ�����ε������ڽ�
	bool LogoGetTtriAngles(LogRec::LogoTri & slrTri);
	
	//�ж�����������̱��Ƿ����sLen������
	bool IsLargerThanShortestLen(LogRec::LogoTri & slrTri, float sLen);

	//�ж��������������������Ƿ�����ͬ��label	
	bool IsTriHasSameLabel(LogRec::LogoTri & slrTri);

	//�ж������໥֮���Ƿ����(1/3,3�ķ�Χ)
	bool IsInProportion(LogRec::LogoTri & slrTri);

	//�ж�����������С�Ƕ��Ƿ���ڸ�����sAngle
	bool IsLargeThanSmallestAngle(LogRec::LogoTri & slrTri, float sAngle);
	
	//�ѱ���local features �������������Ϣpack��64 bit ��signature��
	bool LogoPackInfoPackInSignature(LogRec::LogoTri & slrTri,int binSizeOfShapeAngle,int binSizeOfFeatureOri);

	//�ϸ����ĸ��ĸ�ʽ����pack
	bool LogoPackTriInfoToSignature(LogRec::LogoTri & slrTri,int binSizeOfShapeAngle,int binSizeOfFeatureOri);

}//end of LogRec namespace


#endif

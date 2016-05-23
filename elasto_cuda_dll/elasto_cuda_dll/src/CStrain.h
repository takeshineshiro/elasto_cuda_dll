#ifndef CSTRAIN_H_H_H
#define CSTRAIN_H_H_H
//#pragma   once 

#include "opencv\cv.h"
#include "CElasto.h"
#include "CDataProcess.h"
#include <iostream>

//////////////////////////////////////////////////////////////////////////
//��� Radon �����任 �����Ŀ�ʼ������������
//////////////////////////////////////////////////////////////////////////
typedef struct RadonResultPoint
{
	CvPoint startPoint;
	CvPoint endPoint;

} RadonResultPoint;



class CStrain : public CDataProcess{


public:

	CStrain();

	virtual  void Do();

//private:
	float strainAlgorithm(const EInput &input, EOutput &output);

	void  CalcStrain(const EInput &input, EOutput &output);

	// һ�������任
	void  CalcStrain2(const EInput &input, EOutput &output);
	
	// ��������任
	void  CalcStrain3(const EInput &input, EOutput &output);

	void  RadonProcess(CvPoint &s, CvPoint &e, const CRect &rect, const CvMat &matStrain);

	void  RadonProcess2(CvPoint &s, CvPoint &e, const CRect &rect, const CvMat &matStrain);

	void  RadonProcess3(CvPoint &s,
	                    CvPoint &e,
	                    const CRect &sub_rc,
	                    const CvMat &matStrain,
						std::vector<RadonResultPoint>& vecStartEnd);


	//////////////////////////////////////////////////////////////////////////
	// �����任
	// ���룺
	//   pmatDisplacement,   ����-Ӧ�䣻
	// �����
	//   ppmatRadon,   ָ��ĵ�ַ�� ��������һ�����󱣴������任�Ľ���������������ĵ�ַ
	//                 ������ppmatRadon�С��û���ʹ�ñ����ͷ�����
	//////////////////////////////////////////////////////////////////////////

	static void  RadonSum(const CvMat *pmatDisplacement, CvMat **ppmatRadan);


private:

	// ͼ�����

	void  ImagePostProc(IplImage *pImg, const char *filename, const CvPoint &s, const CvPoint &e);


	//////////////////////////////////////////////////////////////////////////
	// ����Ӧ��ֵ��Ӧ��ͼ�ĻҶ�ֵ
	// ���룺
	//    count�� ��ϵĵ���
	//    pmat��  Ӧ��ľ���
	//    pimg��  Ӧ��ͼ
	//////////////////////////////////////////////////////////////////////////
	void  ComputeStrainValueAndImage(int count, CvMat *pmat, IplImage *pimg);

};



typedef struct 
{
	CRect   rc;

	CvPoint pt;

	float xWidth;//�������࣬ԽС����б��Խ��

} RadonParam;



struct MyLessThan
{
	bool operator() (RadonParam &lhs, RadonParam &rhs)
	{
		return (lhs.pt.x - lhs.pt.y) < (rhs.pt.x - lhs.pt.y);
	}
};



//creator wangxiaomeng 
//date    20160214
// /*���ս�������*/

struct MyLessThan2
{
	bool operator()(const RadonParam &x, const RadonParam &y)
	{
		return x.xWidth > y.xWidth;
	}
};




#endif //define CSTRAIN_H_H_H
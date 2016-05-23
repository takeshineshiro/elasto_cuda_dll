/*/////////////////////////////////////////////////////////////////////////
//filename: CStrain.cpp
//creator author: yangge

modify date: 20160214
author     : wxm
content    : RadonProcess2()�����򲿷ִ����Ż�
version 
******************************************************************************/


#include "stdafx.h"
#include "CStrain.h"
#include "CDisplacement.h"
#include "opencv/highgui.h"
#include "opencv/cv.h"
#include "ImageFunc.h"

#include <iostream>
#include <vector>


using namespace std;


extern ConfigParam g_paramConfig;

void  SaveDataFile(const char *filename, CvMat *pmat);


CStrain::CStrain()
{

}


void CStrain::Do()
{
	//strainAlgorithm(); 
}



//////////////////////////////////////////////////////////////////////////
// �����任
// pmatDisplacement,   rows: disp;  cols: time-extent( lines)
//     ��,��ʾһ����, Ҳ����ʱ�� ��
//     ��,��ʾӦ���ֵ
//////////////////////////////////////////////////////////////////////////
void  CStrain::RadonSum(const CvMat *pmatDisplacement, CvMat **ppmatRodan)
{
	int xstart = 0;

	int xend = pmatDisplacement->rows;

	int t = pmatDisplacement->cols;// time extent

	CvMat *pmatRodan = cvCreateMat(t - 1, t, pmatDisplacement->type);

	cvZero(pmatRodan);

	int tstart = 0;

	int tend = 0;

	int dx = 0;

	float dt = 0.0f;

	float c = 0.0f;

	for (tstart = 0; tstart < t - 1; tstart ++)
	{
		for (tend = tstart + 1; tend < t; tend ++)
		{

			c = (float)(xend - xstart) / (tend - tstart);

			for (dx = xstart; dx < xend; dx ++)
			{

				dt = tstart + (dx - xstart) / c;

				CV_MAT_ELEM(*pmatRodan, float, tstart, tend) = CV_MAT_ELEM(*pmatRodan, float, tstart, tend)
					+ CV_MAT_ELEM(*pmatDisplacement, float, dx, (int)dt);

			}
		}
	}


	*ppmatRodan = pmatRodan;


}


static void PopFirstVector(std::vector<cv::Point2f> &vec)
{
	std::vector<cv::Point2f> swap_vec(vec);

	std::vector<cv::Point2f>::size_type size = swap_vec.size();

	vec.clear();

	if (size > 1)
	{

		int n = size - 1;

		int i;

		for (i = 0; i < n; i++)
		{

			vec.push_back(swap_vec[i + 1]);

		}

	}

}



static void PushBackVector(std::vector<CvPoint2D32f> & vec, CvPoint2D32f &pt)
{
	vec.push_back(pt);
}



static void PopFirstVector(CvPoint2D32f *pVec, int size)
{
	CvPoint2D32f *pvecSwap = new CvPoint2D32f[size];

	ZeroMemory(pvecSwap, sizeof(CvPoint2D32f) * size);

	memcpy(pvecSwap, pVec + 1, sizeof(CvPoint2D32f) * (size - 1));

	memcpy(pVec, pvecSwap, sizeof(CvPoint2D32f) * size);

	delete [] pvecSwap;
}




void  CStrain::ComputeStrainValueAndImage(int count, CvMat *strainMat, IplImage *strImage)
{
	
	float *  tmp;            //��ʱ������ָ��Ӧ��ͼ��ĳһ��

	float   coeff_a1;        //�洢ֱ��б�ʣ�һ���

	int    deltadepth = 5;   //��λ���

	float  result[4] = {0.0, 0.0, 0.0, 0.0}; //�洢ֱ����Ͻ��

	int    i, j;

	CvPoint2D32f pt;


#if 0
	// ԭ������ƣ�ʹ��CvSeq, cvFitLine.

	CvMemStorage *storage = cvCreateMemStorage(0);
	for(i = 0; i < strImage->width; i++)// srtImage����
	{
		CvSeq* point_seq = cvCreateSeq(CV_32FC2, sizeof(CvSeq), sizeof(CvPoint2D32f), storage);
		for(j = 0; j < count - 1; ++j)	//��ѹ��count - 1����
		{
			pt = cvPoint2D32f(j * deltadepth, CV_MAT_ELEM(*outDataMat, float, i, j));
			cvSeqPush(point_seq, &pt);
		}

		for(j = 0; j < strImage->height; j++)// strImage����
		{
			int k = j + count -1; // ǰ���Ѿ�ѹ����count - ��
			tmp = static_cast<float*>(static_cast<void*>(strImage->imageData + j * strImage->widthStep + sizeof(float) * i));  //ȡӦ��ͼ���Ӧλ��
			pt = cvPoint2D32f(k * deltadepth, CV_MAT_ELEM(*outDataMat, float, i, k));
			cvSeqPush(point_seq, &pt);  //ѹ�����һ����
			cvFitLine(point_seq, CV_DIST_L2, 0, 0.01, 0.01, result); //��С�������
			coeff_a1 = result[1] / result[0];   //���ֱ��б�ʣ���Ϊ���ĵ�Ӧ��
			CV_MAT_ELEM_PTR(*strainMat, float, i, j) = coeff_a1;  
			*tmp = 100 * coeff_a1;

			cvSeqPopFront(point_seq);
		}
		cvClearSeq(point_seq);
	}
	cvReleaseMemStorage(&storage);    //read violation

#endif 

#if 0

	/* ��C++�ӿ�����ʵ��	*/
	{
		std::vector<cv::Point2f> points_vec;
		
		for(i = 0; i < strImage->width; i++)// srtImage����
		{
			for (j = 0; j < count - 1; j++)	//��ѹ��points - 1����
			{
				pt = cvPoint2D32f(j * deltadepth, CV_MAT_ELEM(*outDataMat, float, i, j));
				points_vec.push_back(pt);
			}

			for(j = 0; j < strImage->height; ++j)// strImage����
			{
				int k = j + count - 1;
				tmp = static_cast<float*>(static_cast<void*>(strImage->imageData + j * strImage->widthStep + sizeof(float) * i));  //ȡӦ��ͼ���Ӧλ��
				pt = cvPoint2D32f(k * deltadepth, CV_MAT_ELEM(*outDataMat, float, i, k));
				points_vec.push_back(pt);  //ѹ�����һ����
				cv::Vec4f line;
				cv::fitLine(cv::Mat(points_vec), line, CV_DIST_L2, 0, 0.01, 0.01);//��С�������
				coeff_a1 = line[1] / line[0];   //���ֱ��б�ʣ���Ϊ���ĵ�Ӧ��
				CV_MAT_ELEM(*strainMat, float, i, j) = coeff_a1;  
				*tmp = 100 * coeff_a1;
				PopFirstVector(points_vec);
			}
			points_vec.clear();
		}
	}

#endif



#if 1
	// ���������CvSeq
	{

		CvPoint2D32f *points = new  CvPoint2D32f[count];

		CvMat ptMat = cvMat(1, count, CV_32FC2, points);

		for(i = 0; i < strImage->width; i++)// srtImage����
		{


			for (j = 0; j < count - 1; j++)	//��ѹ��points - 1����
			{

				pt = cvPoint2D32f(j * deltadepth, CV_MAT_ELEM(*outDataMat, float, i, j));

				points[j] = pt;

			}



			for(j = 0; j < strImage->height; j++)// strImage����
			{


				int k = j + count - 1;

				
				tmp = static_cast<float*>(static_cast<void*>(strImage->imageData + j * strImage->widthStep + sizeof(float) * i));  //ȡӦ��ͼ���Ӧλ��
				
				
				pt = cvPoint2D32f(k * deltadepth, CV_MAT_ELEM(*outDataMat, float, i, k));

				
				points[count- 1] = pt;  //ѹ�����һ����


				cvFitLine(&ptMat, CV_DIST_L2, 0, 0.01, 0.01, result);//��С�������

				
				coeff_a1 = result[1] / result[0];   //���ֱ��б�ʣ���Ϊ���ĵ�Ӧ��

				//cv::Vec4f line;
				//cv::fitLine(cv::Mat(&ptMat), line, CV_DIST_L2, 0, 0.01, 0.01);//��С�������
				//coeff_a1 = line[1] / line[0];   //���ֱ��б�ʣ���Ϊ���ĵ�Ӧ��

				
				CV_MAT_ELEM(*strainMat, float, i, j) = coeff_a1;  

				
				*tmp = 100 * coeff_a1;


				PopFirstVector(points, count);


			}

		}


		delete [] points;

	}

#endif


}


//�����ֶμ���
void  CStrain::RadonProcess(CvPoint &s, CvPoint &e, const CRect &sub_rc, const CvMat &matStrain)
{
	int  radon_num       = g_paramConfig.radon_num;

	int  radon_step      = g_paramConfig.radon_step;

	
	int  intpl_multiple  = 1; // ��ֵ��������������任

	std::vector<RadonParam> array_params;


	for (int i = 0; i < radon_num; i++)
	{

		RadonParam param;

		param.rc.left   = sub_rc.left;

		param.rc.top    = sub_rc.top + i * radon_step;

		param.rc.right  = sub_rc.right;

		param.rc.bottom = sub_rc.bottom + i * radon_step;


		CvMat *pmatSub  = cvCreateMatHeader(param.rc.Height(), param.rc.Width(), matStrain.type);

		cvGetSubRect(&matStrain, pmatSub, cvRect(param.rc.left, param.rc.top, param.rc.Width(), param.rc.Height()));

		CvMat *pmatRadon = 0;

		CvMat *pmatMultiple = cvCreateMat(pmatSub->rows, pmatSub->cols * intpl_multiple, pmatSub->type);

		cvResize(pmatSub, pmatMultiple);

		CStrain::RadonSum(pmatMultiple, &pmatRadon);


		double  min_val;

		double  max_val;

		CvPoint min_loc;

		CvPoint max_loc;

		cvMinMaxLoc(pmatRadon, &min_val, &max_val, &min_loc, &max_loc);

		param.pt = max_loc;

		array_params.push_back(param);


		cvReleaseMat(&pmatRadon);

		cvReleaseMat(&pmatMultiple);

		cvReleaseMatHeader(&pmatSub);

	}

	std::sort(array_params.begin(), array_params.end(), MyLessThan());


	if (g_paramConfig.calc_type.compare("middle") == 0)
	{

		int size = array_params.size();

		s.x = array_params[size / 2].pt.y / intpl_multiple;

		s.y = array_params[size / 2].rc.top;


		e.x = array_params[size / 2].pt.x / intpl_multiple;

		e.y = array_params[size / 2].rc.bottom;

	}
	else if (g_paramConfig.calc_type.compare("max") == 0)
	{

		int size = array_params.size();

		s.x      = array_params[0].pt.y / intpl_multiple;

		s.y      = array_params[0].rc.top;

		e.x      = array_params[0].pt.x / intpl_multiple;

		e.y      = array_params[0].rc.bottom;
	}
	else if (g_paramConfig.calc_type.compare("min") == 0)
	{
		int size = array_params.size();

		s.x      = array_params[size - 1].pt.y / intpl_multiple;

		s.y      = array_params[size - 1].rc.top;

		e.x      = array_params[size - 1].pt.x / intpl_multiple;

		e.y      = array_params[size - 1].rc.bottom;
	}
	else
	{
		//

	}
}



//�����ֶμ���
void  CStrain::RadonProcess2(CvPoint &s, CvPoint &e, const CRect &sub_rc, const CvMat &matStrain)
{
	int  radon_num      = g_paramConfig.radon_num;

	int  radon_step     = g_paramConfig.radon_step;


	int  intpl_multiple = 1; // ��ֵ��������������任

	std::vector<RadonParam> array_params;

	for (int i = 0; i < radon_num; i++)
	{

		RadonParam param;

		param.rc.left   = sub_rc.left;

		param.rc.top    = sub_rc.top + i * radon_step;

		param.rc.right  = sub_rc.right;

		param.rc.bottom = sub_rc.bottom + i * radon_step;

		CvMat *pmatSub  = cvCreateMatHeader(param.rc.Height(), param.rc.Width(), matStrain.type);

		cvGetSubRect(&matStrain, pmatSub, cvRect(param.rc.left, param.rc.top, param.rc.Width(), param.rc.Height()));

		CvMat *pmatRadon = 0;

		CvMat *pmatMultiple = cvCreateMat(pmatSub->rows, pmatSub->cols * intpl_multiple, pmatSub->type);

		cvResize(pmatSub, pmatMultiple);

		CStrain::RadonSum(pmatMultiple, &pmatRadon);

		double  min_val;

		double  max_val;

		CvPoint min_loc;

		CvPoint max_loc;

		cvMinMaxLoc(pmatRadon, &min_val, &max_val, &min_loc, &max_loc);

		param.pt = max_loc;

		param.xWidth = param.pt.y - param.pt.x;//add by wxm

		array_params.push_back(param);


		cvReleaseMat(&pmatRadon);

		cvReleaseMat(&pmatMultiple);

		cvReleaseMatHeader(&pmatSub);

	}

	//std::sort(array_params.begin(), array_params.end(), MyLessThan());
	std::sort(array_params.begin(), array_params.end(), MyLessThan2());//modified by wxm


	if (g_paramConfig.calc_type.compare("middle") == 0)
	{
		int size = array_params.size();

		s.x      = array_params[size / 2].pt.y / intpl_multiple;

		s.y      = array_params[size / 2].rc.top;

		e.x      = array_params[size / 2].pt.x / intpl_multiple;

		e.y      = array_params[size / 2].rc.bottom;
	}
	else if (g_paramConfig.calc_type.compare("max") == 0)
	{
		int size = array_params.size();
		s.x      = array_params[0].pt.y / intpl_multiple;
		s.y      = array_params[0].rc.top;

		e.x      = array_params[0].pt.x / intpl_multiple;
		e.y      = array_params[0].rc.bottom;
	}
	else if (g_paramConfig.calc_type.compare("min") == 0)
	{
		int size = array_params.size();
		s.x      = array_params[size - 1].pt.y / intpl_multiple;
		s.y      = array_params[size - 1].rc.top;

		e.x      = array_params[size - 1].pt.x / intpl_multiple;
		e.y      = array_params[size - 1].rc.bottom;
	}
	else
	{
		//

	}

#if 1 //�����ã���������

	int    win_size       = g_paramConfig.windowHW;

	double overlap        = (g_paramConfig.windowHW - g_paramConfig.step) / (float)g_paramConfig.windowHW;  // �غ��ʣ�90%

	double sound_velocity = g_paramConfig.acousVel; // �����ٶ�

	double sample_frq     = g_paramConfig.sampleFreqs;

	double prf            = g_paramConfig.prf;

	std::vector<RadonResultPoint> vecStartEnd;

	FILE *wfile;

	wfile                 = fopen("vecResult.txt", "w");


	for (int i = 0; i < array_params.size(); i++)
	{
		CvPoint startPoint = cvPoint((array_params[i].pt.y / intpl_multiple), array_params[i].rc.top);

		CvPoint endPoint   = cvPoint((array_params[i].pt.x / intpl_multiple), array_params[i].rc.bottom);

		double v           = ((endPoint.y - startPoint.y) * win_size * (1 - overlap) * sound_velocity / sample_frq / 2)
			/ ((endPoint.x - startPoint.x) / prf);

		double e            = v * v * 3;

		fprintf(wfile, "vecStart(%d,%d),  vecEnd(%d,%d) ;",
							startPoint.x, startPoint.y,
							endPoint.x, endPoint.y);//����

		fprintf(wfile, "v = %f, e = %f \n", v, e);// ����ģ��

	}


	fclose(wfile);/*�ر��ļ�*/

	wfile = NULL;

#endif

}

//////////////////////////////////////////////////////////////////////////
//�����ֶμ��㣬creator ������
// ��������, outDataMat, �����ʾ λ������;��ʽ˵��:  ��,��ʾһ����; ��,��ʾλ��ֵ
// ������ݣ� vecStartEnd ��ʼ������ �� �յ�����
//   
//////////////////////////////////////////////////////////////////////////
void  CStrain::RadonProcess3(CvPoint &s, 
	                         CvPoint &e, 
							 const CRect &sub_rc, 
							 const CvMat &matStrain, 
							 std::vector<RadonResultPoint>& vecStartEnd)
{
	
	int  radon_num = g_paramConfig.radon_num;

	int  radon_step = g_paramConfig.radon_step;

	int  intpl_multiple = 1; // ��ֵ��������������任

	std::vector<RadonParam> array_params;

	for (int i = 0; i < radon_num; i++)
	{

		RadonParam param;

		param.rc.left  = sub_rc.left;

		param.rc.top   = sub_rc.top + i * radon_step;

		param.rc.right = sub_rc.right;

		param.rc.bottom = sub_rc.bottom + i * radon_step;

		CvMat *pmatSub = cvCreateMatHeader(param.rc.Height(), param.rc.Width(), matStrain.type);

		cvGetSubRect(&matStrain, pmatSub, cvRect(param.rc.left, param.rc.top, param.rc.Width(), param.rc.Height()));

		CvMat *pmatRadon = 0;

		CvMat *pmatMultiple = cvCreateMat(pmatSub->rows, pmatSub->cols * intpl_multiple, pmatSub->type);

		cvResize(pmatSub, pmatMultiple);

		CStrain::RadonSum(pmatMultiple, &pmatRadon);

		double  min_val;

		double  max_val;

		CvPoint min_loc;

		CvPoint max_loc;

		cvMinMaxLoc(pmatRadon, &min_val, &max_val, &min_loc, &max_loc);

		param.pt = max_loc;

		array_params.push_back(param);


		cvReleaseMat(&pmatRadon);

		cvReleaseMat(&pmatMultiple);

		cvReleaseMatHeader(&pmatSub);

	}

	std::sort(array_params.begin(), array_params.end(), MyLessThan());



	if (g_paramConfig.calc_type.compare("middle") == 0)
	{

		int size = array_params.size();

		s.x      = array_params[size / 2].pt.y / intpl_multiple;
		s.y      = array_params[size / 2].rc.top;

		e.x      = array_params[size / 2].pt.x / intpl_multiple;
		e.y      = array_params[size / 2].rc.bottom;
	}
	else if (g_paramConfig.calc_type.compare("max") == 0)
	{
		int size = array_params.size();
		s.x      = array_params[0].pt.y / intpl_multiple;
		s.y      = array_params[0].rc.top;

		e.x      = array_params[0].pt.x / intpl_multiple;
		e.y      = array_params[0].rc.bottom;
	}
	else if (g_paramConfig.calc_type.compare("min") == 0)
	{
		int size = array_params.size();
		s.x      = array_params[size - 1].pt.y / intpl_multiple;
		s.y      = array_params[size - 1].rc.top;

		e.x      = array_params[size - 1].pt.x / intpl_multiple;
		e.y      = array_params[size - 1].rc.bottom;
	}
	else
	{
		//

	}

	//���浽λ�õ� vec ��, wxm
	for (int i = 0; i < array_params.size(); i++)
	{
		RadonResultPoint radonResultPoint;

		radonResultPoint.startPoint = cvPoint((array_params[i].pt.y / intpl_multiple), array_params[i].rc.top);

		radonResultPoint.endPoint = cvPoint((array_params[i].pt.x / intpl_multiple), array_params[i].rc.bottom);


		vecStartEnd.push_back(radonResultPoint);
	}
}


//////////////////////////////////////////////////////////////////////////
// ���㵯��ģ����ʹ�������任��һ�Σ�
// ��������, outDataMat, �����ʾ λ������;��ʽ˵��:  ��,��ʾһ����; ��,��ʾλ��ֵ
//
//////////////////////////////////////////////////////////////////////////
void  CStrain::CalcStrain2(const EInput &input, EOutput &output)
{
	
	string filename = input.filepath_s;

	//��С���˷���Ӧ�䣻������ϵĵ㣬�������Ϊ��ȣ�������Ϊλ��

	const int points = g_paramConfig.fitline_pts;	//������ϵĵ���


	//int image_width = outDataMat->cols - 300 / g_paramConfig.step;

	int image_width     = outDataMat->cols;

	IplImage *strImage  = cvCreateImage(cvSize(outDataMat->rows, image_width - points + 1), IPL_DEPTH_32F, 1);//������ʾӦ��, �����outDataMat����ת��,���еߵ�.
	
	CvMat    *strainMat = cvCreateMat(strImage->width, strImage->height, CV_32FC1);  //Ӧ������������strImage����ת�ã���outDataMat��ͬ

	ComputeStrainValueAndImage(points, strainMat, strImage);


	SaveDataFile("strain.dat", strainMat);//���ڱ���Ӧ������
	
	//�����任&����в�&����ģ��
	{
		int    win_size       = g_paramConfig.windowHW;

		double overlap        = (g_paramConfig.windowHW - g_paramConfig.step) / (float)g_paramConfig.windowHW;  // �غ��ʣ�90%

		double sound_velocity = g_paramConfig.acousVel; // �����ٶ�

		double sample_frq     = g_paramConfig.sampleFreqs;

		double prf            = g_paramConfig.prf;


		int    dep_start      = (g_paramConfig.sb_x < 0)  ? 0 : g_paramConfig.sb_x;

		int    dep_size       =  (g_paramConfig.sb_w < 0) ? strainMat->width : g_paramConfig.sb_w;

		int    dep_end        = dep_start + dep_size - 1;

		int    t_start        = (g_paramConfig.sb_y < 0) ? 0 : g_paramConfig.sb_y;

		int    t_size         = (g_paramConfig.sb_h < 0) ? strainMat->rows : g_paramConfig.sb_h;

		int    t_end          = t_start + t_size - 1;

		//printf("dep_start:%d, dep_end:%d, dep_size:%d; t_start:%d, t_end:%d, t_size:%d\n", dep_start, dep_end, dep_size, t_start, t_end, t_size);

		CvMat *pmatStrainTran = cvCreateMat(strainMat->cols, strainMat->rows, strainMat->type);// ��strainMatת��

		cvTranspose(strainMat, pmatStrainTran);


		CvMat *pmatSub       = cvCreateMatHeader(dep_size, t_size, pmatStrainTran->type);

		cvGetSubRect(pmatStrainTran, pmatSub, cvRect(t_start, dep_start, t_size, dep_size));
		
		CvMat *pmatRadon = 0;

		// ��ֵ��������������任

		float  intpl_multiple = 1.0f;//20.0, 

		CvMat *pmatMultiple   = cvCreateMat(pmatSub->rows, (int)(pmatSub->cols * intpl_multiple), pmatSub->type);

		cvResize(pmatSub, pmatMultiple);

		CStrain::RadonSum(pmatMultiple, &pmatRadon);


		double  min_val;

		double  max_val;

		CvPoint min_loc;

		CvPoint max_loc;

		cvMinMaxLoc(pmatRadon, &min_val, &max_val, &min_loc, &max_loc);

		//printf("max_loc:(%d,%d)\n", max_loc.x, max_loc.y);

		double v = ((dep_end - dep_start) * win_size * (1 - overlap) * sound_velocity / sample_frq / 2) 
			/ ((max_loc.x - max_loc.y) / intpl_multiple / prf);


		double e = v * v * 3;

		output.v = (float)v;

		output.e = (float)e;

		cvReleaseMat(&pmatRadon);

		cvReleaseMat(&pmatMultiple);

		cvReleaseMatHeader(&pmatSub);

		cvReleaseMat(&pmatStrainTran);

		// ����б��
		IplImage *pimgStrain = cvCreateImage(cvGetSize(strImage), strImage->depth, 3);

		cvCvtColor(strImage, pimgStrain, CV_GRAY2BGR);

		cvLine(pimgStrain, cvPoint((int)(max_loc.y / intpl_multiple), dep_start), cvPoint((int)(max_loc.x / intpl_multiple), dep_end), CV_RGB(255,0,0), 2, CV_AA, 0);   //����

		cvSaveImage(filename.c_str(), pimgStrain);

		cvReleaseImage(&pimgStrain);

	}


	cvReleaseImage(&strImage);

	cvReleaseMat(&strainMat);

}


//////////////////////////////////////////////////////////////////////////
// ���㵯��ģ����ʹ�ö�������任
// ��������, outDataMat, �����ʾ λ������;��ʽ˵��:  ��,��ʾһ����; ��,��ʾλ��ֵ
//
//////////////////////////////////////////////////////////////////////////
void  CStrain::CalcStrain3(const EInput &input, EOutput &output)
{
	
	string filename     = input.filepath_s;

	//��С���˷���Ӧ�䣻������ϵĵ㣬�������Ϊ��ȣ�������Ϊλ��

	const int points    = g_paramConfig.fitline_pts;	//������ϵĵ���

	//int image_width = outDataMat->cols - 300 / g_paramConfig.step;

	int image_width     = outDataMat->cols;

	IplImage *strImage  = cvCreateImage(cvSize(outDataMat->rows, image_width - points + 1), IPL_DEPTH_32F, 1);//������ʾӦ��, �����outDataMat����ת��,���еߵ�.

	CvMat    *strainMat = cvCreateMat(strImage->width, strImage->height, CV_32FC1);  //Ӧ������������strImage����ת�ã���outDataMat��ͬ

	ComputeStrainValueAndImage(points, strainMat, strImage);


	SaveDataFile("strain.dat", strainMat);//���ڱ���Ӧ������

	
	//�����任&����в�&����ģ��
	{

		int    win_size       = g_paramConfig.windowHW;

		double overlap        = (g_paramConfig.windowHW - g_paramConfig.step) / (float)g_paramConfig.windowHW;  // �غ��ʣ�90%

		double sound_velocity = g_paramConfig.acousVel; // �����ٶ�

		double sample_frq     = g_paramConfig.sampleFreqs;

		double prf            = g_paramConfig.prf;


		int    dep_start      = (g_paramConfig.sb_x < 0)  ? 0 : g_paramConfig.sb_x;

		int    dep_size       =  (g_paramConfig.sb_w < 0) ? strainMat->width : g_paramConfig.sb_w;

		int    dep_end        = dep_start + dep_size - 1;

		int    t_start        = (g_paramConfig.sb_y < 0) ? 0 : g_paramConfig.sb_y;
		int    t_size         = (g_paramConfig.sb_h < 0) ? strainMat->rows : g_paramConfig.sb_h;
		int    t_end          = t_start + t_size - 1;
		//printf("dep_start:%d, dep_end:%d, dep_size:%d; t_start:%d, t_end:%d, t_size:%d\n", dep_start, dep_end, dep_size, t_start, t_end, t_size);
		CvMat *pmatStrainTran = cvCreateMat(strainMat->cols, strainMat->rows, strainMat->type);// ��strainMatת��

		cvTranspose(strainMat, pmatStrainTran);

		CvPoint      start;

		CvPoint      end;

		CRect       rect;

		rect.left  = t_start;

		rect.right = t_end;

		rect.top   = dep_start;

		rect.bottom= dep_end;

		//printf("rect:%d, %d, %d, %d\n", rect.left, rect.right, rect.top, rect.bottom);
		//RadonProcess(start, end, rect, *pmatStrainTran);
#if 1
		RadonProcess2(start, end, rect, *pmatStrainTran);
#endif
#if 0   //����ʱʹ��, ���� RadonProcess2 ���Ѿ�ʵ��
		std::vector<RadonResultPoint> vecStartEnd;
		FILE *wfile;

		wfile = fopen("vecResult.txt", "w");
		RadonProcess3(start, end, rect, *pmatStrainTran, vecStartEnd);
		for (int i = 0; i < vecStartEnd.size(); i++)
		{
			double v = ((vecStartEnd[i].endPoint.y - vecStartEnd[i].startPoint.y) * win_size * (1 - overlap) * sound_velocity / sample_frq / 2)
				/ ((vecStartEnd[i].endPoint.x - vecStartEnd[i].startPoint.x) / prf);
			double e = v * v * 3;

			fprintf(wfile, "vecStart(%d,%d),  vecEnd(%d,%d) ;", 
				vecStartEnd[i].startPoint.x, vecStartEnd[i].startPoint.y,
				vecStartEnd[i].endPoint.x, vecStartEnd[i].endPoint.y);//����
			fprintf(wfile, "v = %f, e = %f \n", v, e);// ����ģ��
		}

		fclose(wfile);/*�ر��ļ�*/
		wfile = NULL;
#endif
		//printf("s_pt:(%d,%d); e_pt(%d,%d)\n", start.x, start.y, end.x, end.y);

		double v = ((end.y - start.y) * win_size * (1 - overlap) * sound_velocity / sample_frq / 2) 
			       / ((end.x - start.x) / prf);



		double e = v * v * 3;


		output.v = (float)v;


		output.e = (float)e;

		cvReleaseMat(&pmatStrainTran);


		// ����б��
		ImagePostProc(strImage, filename.c_str(), start, end);


	}


	cvReleaseImage(&strImage);


	cvReleaseMat(&strainMat);



}




void  CStrain::ImagePostProc(IplImage *strImage, const char *filename, const CvPoint &start, const CvPoint &end)
{

	const char * gray_file = "strain_gray.bmp";


	{
		IplImage *pimgStrain = cvCreateImage(cvGetSize(strImage), strImage->depth, 3);

		cvCvtColor(strImage, pimgStrain, CV_GRAY2BGR);

		cvSaveImage(gray_file, pimgStrain);

		cvReleaseImage(&pimgStrain);

	}

	{

		IplImage *pImage = cvLoadImage(gray_file, 0);

		IplImage *pimgStrain = cvCreateImage(cvGetSize(pImage), pImage->depth, 3);

		pimgStrain = cvCreateImage(cvGetSize(pImage), pImage->depth, 3);


		//ͼ����ǿ ��1
		// ������� [0,0.5] �� [0.5,1], gamma=1  ͼ����ǿ
		ImageAdjust(pImage, pImage,	0, 0.5, 0, 0.5, 0.6);// Y����mapped to bottom and top of dst	

		//cvSaveImage("res\\ImageAdjust.bmp", image);//������ǿЧ��ͼ

		//ͼ����ǿ ��2 Ч������
		//ImageStretchByHistogram(image, image);//ͼ����ǿ: �����ȫ��������
		//cvSaveImage("res\\ImageStretchByHistogram.bmp", image);

		//ͼ����ǿ ��3 Ч������
		//ImageStretchByHistogram2(image, image);//ͼ����ǿ: �����ȫ��������
		//cvSaveImage("res\\ImageStretchByHistogram2.bmp", image);

		cvNot(pImage, pImage);//�ڰ���ɫ��ת

		//cvSaveImage("res\\cvNot.bmp", image);//�ڰ�ͼ
		cvCvtColor(pImage, pimgStrain, CV_GRAY2BGR);//ͼ��ת����BGR


		ChangeImgColor(pimgStrain);


		cvLine(pimgStrain, start, end, CV_RGB(255,0,0), 2, CV_AA, 0);   //����


		cvSaveImage(filename, pimgStrain);


		//�ͷ���Դ
		cvReleaseImage(&pImage);

		cvReleaseImage(&pimgStrain);

	}

}

//////////////////////////////////////////////////////////////////////////
// Ϊ�˽��ʹ���ʱ�䡣�ر������ƣ�
// ���������任��
// ��һ��ȷ����ɨ���ߵķ�Χ���ڶ��β�ֵ20�����������任��
//////////////////////////////////////////////////////////////////////////
void  CStrain::CalcStrain(const EInput &input, EOutput &output)
{

	string filename = input.filepath_s;


	//��С���˷���Ӧ�䣻������ϵĵ㣬�������Ϊ��ȣ�������Ϊλ��

	int deltadepth = 5;   //��λ���

	float result1[4] = {0.0, 0.0, 0.0, 0.0}; //�洢ֱ����Ͻ��

	float coeff_a1; //�洢ֱ��б�ʣ�һ���

	float *tmp;  //��ʱ������ָ��Ӧ��ͼ��ĳһ��

	int points = g_paramConfig.fitline_pts;	//������ϵĵ���

	float minstrain = 0;

	float maxstrain = 0;

	int image_width = outDataMat->cols - 300 / g_paramConfig.step;

	//int image_width = outDataMat->cols;

	IplImage *strImage  = cvCreateImage(cvSize(outDataMat->rows, image_width - points + 1), IPL_DEPTH_32F, 1);//������ʾӦ��, �����outDataMat����ת��,���еߵ�.
	
	CvMat    *strainMat = cvCreateMat(strImage->width, strImage->height, CV_32FC1);  //Ӧ������������strImage����ת�ã���outDataMat��ͬ

	CvMemStorage* storage = cvCreateMemStorage(0);

	for(int i = 0; i < strImage->width; ++i)// srtImage����
	{
		CvSeq* point_seq = cvCreateSeq(CV_32FC2, sizeof(CvSeq), sizeof(CvPoint2D32f), storage);

		for(int j = 0; j < points - 1; ++j)	//��ѹ��points - 1����
		{

			cvSeqPush(point_seq, &cvPoint2D32f(j * deltadepth, *(static_cast<float*>(static_cast<void*>(CV_MAT_ELEM_PTR(*outDataMat, i,j))))));
		
		}

		for(int j = points - 1; j < image_width; ++j)// strImage����
		{
			int k = j - points + 1;

			tmp = static_cast<float*>(static_cast<void*>(strImage->imageData + (j - points + 1) * strImage->widthStep + sizeof(float) * i));  //ȡӦ��ͼ���Ӧλ��
			
			cvSeqPush(point_seq, &cvPoint2D32f((j)*deltadepth, *(static_cast<float*>(static_cast<void*>(CV_MAT_ELEM_PTR(*outDataMat, i, j))))));  //ѹ�����һ����
			
			cvFitLine(point_seq, CV_DIST_L2, 0, 0.01, 0.01, result1); //��С�������

			coeff_a1 = result1[1] / result1[0];   //���ֱ��б�ʣ���Ϊ���ĵ�Ӧ��

			*(static_cast<float*>(static_cast<void*>(CV_MAT_ELEM_PTR(*strainMat, i, k)))) = coeff_a1;  

			*tmp = 100 * coeff_a1;

			cvSeqPopFront(point_seq);

		}

		cvClearSeq(point_seq);

	}

	cvReleaseMemStorage(&storage);    //read violation

	SaveDataFile("strain.dat", strainMat);

	//cvSaveImage("strain_gray.bmp", strImage);

	{
		int    dep_start = 500 / g_paramConfig.step; // 150-3.5cm, 70-2.5cm, 110-3cm

		//int    dep_start = 800 / g_paramConfig.step; // 150-3.5cm, 70-2.5cm, 110-3cm
		int    dep_size  = 1600 / g_paramConfig.step ;

		int    dep_end   = dep_start + dep_size - 1;

		int    win_size  = 100;

		double overlap   = (g_paramConfig.windowHW - g_paramConfig.step) / (float)g_paramConfig.windowHW;  // �غ��ʣ�90%

		double sound_velocity = 1500.0f; // �����ٶ�

		double sample_frq = 60e6;

		double prf = 1/300e-6;

		// Ϊ����߼���׼ȷ�ȣ���Ϊ�Ѳ�����Χʱ����������150~240��֮�䡣
		// 150, 90
		// 200, 70
		// 180, 70
		int    t_base  = 0;

		int    t_size  = strainMat->rows;

		int    t_start = t_base;

		int    t_end   = t_base + t_size - 1;


		CvMat *pmatStrainTran = cvCreateMat(strainMat->cols, strainMat->rows, strainMat->type);// ��strainMatת��

		cvTranspose(strainMat, pmatStrainTran);

		CvMat *pmatSub = cvCreateMatHeader(dep_size, t_size, pmatStrainTran->type);

		cvGetSubRect(pmatStrainTran, pmatSub, cvRect(t_start, dep_start, t_size, dep_size));

		//��һ�������任��Ŀ���Ǽ��ٴ���ɨ���ߵ�����
		CvMat *pmatRadon = 0;

		CStrain::RadonSum(pmatSub, &pmatRadon);

		double  min_val;

		double  max_val;

		CvPoint min_loc;

		CvPoint max_loc;

		cvMinMaxLoc(pmatRadon, &min_val, &max_val, &min_loc, &max_loc);

		// ǰ����Ŵ�һЩɨ���ߣ�ע�ⲻҪ����ԭʼ�ķ�Χ

		int  t_shift = 2;

		// ���ݱ����pmatStrainTranȡ�ã�����t_start,t_end������ֵ�����pmatSub������pmatStrainTran�е�һ���Ӽ���ӳ�䵽pmatStrainTran������
		
		t_start = max_loc.y - t_shift + t_base;// ��ǰ���޶���Χʱ�����������ʼֵ
		
		t_end   = max_loc.x + t_shift + t_base;

		t_start = t_start > -1 ? t_start : 0;

		t_end = (t_end < pmatStrainTran->width) ? t_end : pmatStrainTran->width - 1;

		t_size  = t_end - t_start + 1;


		cvReleaseMatHeader(&pmatSub);

		cvReleaseMat(&pmatRadon);

		float  intpl_multiple = 20.0f;//��ֵ�ı���
		{
			//�ڶ��������任
		
			pmatSub = cvCreateMatHeader(dep_size, t_size, pmatStrainTran->type);

			cvGetSubRect(pmatStrainTran, pmatSub, cvRect(t_start, dep_start, pmatSub->width, pmatSub->height));


			CvMat *pmatMultiple = cvCreateMat(pmatSub->rows, (int)(pmatSub->cols * intpl_multiple), pmatSub->type);

		    cvResize(pmatSub, pmatMultiple);

			//cvResize(pmatSub, pmatMultiple, CV_INTER_CUBIC);

			CStrain::RadonSum(pmatMultiple, &pmatRadon);

		
			ASSERT(max_loc.x != max_loc.y);

			cvMinMaxLoc(pmatRadon, &min_val, &max_val, &min_loc, &max_loc);

			cvReleaseMat(&pmatMultiple);

		}
		
		double v = ((dep_end - dep_start) * win_size * (1 - overlap) * sound_velocity / sample_frq / 2) / ((max_loc.x - max_loc.y) / intpl_multiple / prf);
	
		double e = v * v * 3;

		output.v = (float)v;

		output.e = (float)e;

		cvReleaseMatHeader(&pmatSub);

		cvReleaseMat(&pmatStrainTran);

		cvReleaseMat(&pmatRadon);

		IplImage *pimgStrain = cvCreateImage(cvGetSize(strImage), strImage->depth, 3);

		cvCvtColor(strImage, pimgStrain, CV_GRAY2BGR);

		cvLine(pimgStrain, cvPoint((int)(max_loc.y / intpl_multiple + t_start), dep_start), cvPoint((int)(max_loc.x / intpl_multiple + t_start), dep_end), CV_RGB(255,0,0), 2, CV_AA, 0);   //����
		
#if 0
		IplImage *pimgResize = cvCreateImage(cvSize(strImage->width, 355), strImage->depth, 3);
		cvResize(pimgStrain, pimgResize);
		cvSaveImage(filename.c_str(), pimgResize);
		cvReleaseImage(&pimgResize);
		cvReleaseImage(&pimgStrain);
#else

		cvSaveImage(filename.c_str(), pimgStrain);

		cvReleaseImage(&pimgStrain);

#endif
	}

	cvReleaseImage(&strImage);

	cvReleaseMat(&strainMat);



}


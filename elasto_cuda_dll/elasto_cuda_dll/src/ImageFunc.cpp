//////////////////////////////////////////////////////////////////////////
//

#include "stdafx.h"
#include "ImageFunc.h"
#include "opencv/highgui.h"

//////////////////////////////////////////////////////////////////////////
// �ڲ���psrc������ת�ò���
//////////////////////////////////////////////////////////////////////////
void MakeImage(const CvMat *psrc, const char *filename)
{
	CvMat *pmat = cvCreateMat(psrc->cols, psrc->rows, psrc->type);

	cvTranspose(psrc, pmat);
	IplImage *pimage = cvCreateImage(cvGetSize(pmat), IPL_DEPTH_32F, 3);
	cvCvtColor(pmat, pimage, CV_GRAY2BGR);

	cvSaveImage(filename, pimage);

	cvReleaseImage(&pimage);
	cvReleaseMat(&pmat);
}


/*************************************************
Function:      ͨ��ֱ��ͼ�任����ͼ����ǿ����ͼ��Ҷȵ���ֵ���쵽0-255
src1:               ��ͨ���Ҷ�ͼ��
dst1:              ͬ����С�ĵ�ͨ���Ҷ�ͼ��
*************************************************/
int ImageStretchByHistogram(IplImage *src1, IplImage *dst1)
{
	//p[]���ͼ������Ҷȼ��ĳ��ָ��ʣ�
	//p1[]��Ÿ����Ҷȼ�֮ǰ�ĸ��ʺͣ�����ֱ��ͼ�任��
	//num[]���ͼ������Ҷȼ����ֵĴ���;
	assert(src1->width == dst1->width);
	double p[256], p1[256], num[256];

	//�����������
	memset(p, 0, sizeof(p));
	memset(p1, 0, sizeof(p1));
	memset(num, 0, sizeof(num));
	int height = src1->height;
	int width = src1->width;
	long wMulh = height * width;

	//����ͼ������Ҷȼ����ֵĴ���
	//statistics   
	for (int x = 0; x < src1->width; x++)
	{
		for (int y = 0; y < src1->height; y++){
			uchar v = ((uchar*)(src1->imageData + src1->widthStep*y))[x];
			num[v]++;
		}
	}

	//����ͼ������Ҷȼ��ĳ��ָ���
	//calculate probability   
	for (int i = 0; i < 256; i++)
	{
		p[i] = num[i] / wMulh;
	}

	//���Ÿ����Ҷȼ�֮ǰ�ĸ��ʺ�
	//p1[i]=sum(p[j]);  j<=i;   
	for (int i = 0; i < 256; i++)
	{
		for (int k = 0; k <= i; k++)
			p1[i] += p[k];
	}

	//ֱ��ͼ�任
	// histogram transformation   
	for (int x = 0; x < src1->width; x++)
	{
		for (int y = 0; y < src1->height; y++){
			uchar v = ((uchar*)(src1->imageData + src1->widthStep*y))[x];
			((uchar*)(dst1->imageData + dst1->widthStep*y))[x] = p1[v] * 255 + 0.5;
		}
	}
	return 0;
}

//ͼ�����ȱ任
int ImageAdjust(IplImage* src, IplImage* dst,
	double low, double high,   // X����low and high are the intensities of src
	double bottom, double top, // Y����mapped to bottom and top of dst
	double gamma)
{
	if (low < 0 && low>1 && high < 0 && high>1 &&
		bottom < 0 && bottom>1 && top < 0 && top>1 && low > high)
		return -1;
	double low2 = low * 255;
	double high2 = high * 255;
	double bottom2 = bottom * 255;
	double top2 = top * 255;
	double err_in = high2 - low2;
	double err_out = top2 - bottom2;

	int x, y;
	double val;

	// intensity transform
	for (y = 0; y < src->height; y++)
	{
		for (x = 0; x < src->width; x++)
		{
			val = ((uchar*)(src->imageData + src->widthStep*y))[x];
			val = pow((val - low2) / err_in, gamma) * err_out + bottom2;
			if (val > 255) val = 255; if (val < 0) val = 0; // Make sure src is in the range [low,high]
			((uchar*)(dst->imageData + dst->widthStep*y))[x] = (uchar)val;
		}
	}
	return 0;
}



/*************************************************
Function:
Description:     ��Ϊ����ͷͼ���������Ҫ����ֱ��ͼ����ͼ����ǿ��
��ͼ��Ҷȵ���ֵ���쵽0-255
Calls:
Called By:
Input:           ��ͨ���Ҷ�ͼ��
Output:          ͬ����С�ĵ�ͨ���Ҷ�ͼ��
Return:
Others:           http://www.xiaozhou.net/ReadNews.asp?NewsID=771
DATE:               2007-1-5
*************************************************/
int ImageStretchByHistogram2(IplImage *src, IplImage *dst)
{
	//p[]���ͼ������Ҷȼ��ĳ��ָ��ʣ�
	//p1[]��Ÿ����Ҷȼ�֮ǰ�ĸ��ʺͣ�����ֱ��ͼ�任��
	//num[]���ͼ������Ҷȼ����ֵĴ���; 

	assert(src->width == dst->width);
	float p[256], p1[256], num[256];
	//�����������
	memset(p, 0, sizeof(p));
	memset(p1, 0, sizeof(p1));
	memset(num, 0, sizeof(num));

	int height = src->height;
	int width = src->width;
	long wMulh = height * width;

	//����ͼ������Ҷȼ����ֵĴ���
	// to do use openmp
	for (int x = 0; x < width; x++)
	{
		for (int y = 0; y < height; y++)
		{
			uchar v = ((uchar*)(src->imageData + src->widthStep*y))[x];
			num[v]++;
		}
	}

	//����ͼ������Ҷȼ��ĳ��ָ���
	for (int i = 0; i<256; i++)
	{
		p[i] = num[i] / wMulh;
	}

	//���Ÿ����Ҷȼ�֮ǰ�ĸ��ʺ�
	for (int i = 0; i<256; i++)
	{
		for (int k = 0; k <= i; k++)
			p1[i] += p[k];
	}

	//ֱ��ͼ�任
	// to do use openmp
	for (int x = 0; x < width; x++)
	{
		for (int y = 0; y < height; y++)
		{
			uchar v = ((uchar*)(src->imageData + src->widthStep*y))[x];
			((uchar*)(dst->imageData + dst->widthStep*y))[x] = p1[v] * 255 + 0.5;
		}
	}

	return 0;
}

int ChangeImgColor(IplImage *scr)
{
	CvScalar avgChannels = cvAvg(scr);
	double avgB = avgChannels.val[0];//��ȡ��һͨ��ƽ��ֵ
	double avgG = avgChannels.val[1];//��ȡ�ڶ�ͨ��ƽ��ֵ
	double avgR = avgChannels.val[2];//��ȡ����ͨ��ƽ��ֵ

	CvScalar idensity;
	int i = 0, j = 0;
	for (; i < scr->height; i++)
	{
		for (j = 0; j < scr->width; j++)
		{
			idensity = cvGet2D(scr, i, j);
			idensity.val[0] = idensity.val[0] - avgB + 19;//�޸�ɫ��ֵ
			idensity.val[1] = idensity.val[1] - avgG + 79;
			idensity.val[2] = idensity.val[2] - avgR + 168;
			cvSet2D(scr, i, j, idensity);
		}
	}

	return 0;
}

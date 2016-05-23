
#include "stdafx.h"
#include "CDisplacement.h"
#include "CData.h"
#include "CStrain.h"
#include "CFilter.h"
#include "CElasto.h"
#include "opencv/highgui.h"
#include "FileUtility.h"

#include <iostream>
#include <time.h>
#include <stdio.h>
#include "Log.h"
#include "TestTime.h"
#include "SysConfig.h"
#include "ImageFunc.h"



// δ�Ӵ����壨���������ģ��ʱRF���ݵķ�ֵ�����ֵ�����������ã�ע�⣺�˲��Ժ�
#define  RF_NO_BODY_THRESHOLD  100 

CData* test;
CDataProcess* datatest;
CDisplacement* distest;
CStrain* strtest;
CFilter* bpfilt;
CFilter* lpfilt;

EPHandler  g_lpEpHander = NULL;
void *g_lpParam = NULL;
ConfigParam  g_paramConfig;


// ��ȡ�������ļ�
 template<class T> void ReadBinFile(const char *filepath, T *pBuffer, int nElems)
{
	FILE *fid;
	int  nCount = 0;
	int  nBytes = 0;
	T *p = pBuffer;
	if (fopen_s(&fid, filepath, "rb") == 0)
	{
		T t;
		while (!feof(fid))
		{
			nBytes = fread(&t, sizeof(T), 1, fid);
			if (nBytes == 0)   break;
			*p ++ = t;
			nCount ++;
		}

		fclose(fid);
	}
}

// ��ȡӦ������,���������任���㵯��ģ��
//  strain_NG5.dat,   v=3.31,  e =33.008
void TestCalcStrain()
{
	double *pDatas = new double[299 * 355];
	
	ReadBinFile("strain_NG5.dat", pDatas, 299 * 355);

	CvMat *strainMat = cvCreateMat(355, 299, CV_32FC1);

	for (int i = 0; i < 299; i++) // cols
	{
		for (int j = 0; j < 355; j++) // rows
		{
			float f =  (float)(* (pDatas + i * 355 + j));
			CV_MAT_ELEM(*strainMat, float, j, i) = f;
		}
	}
	delete [] pDatas;

	{
		int    dep_start = 51;
		int    dep_end   = 250;
		int    win_size  = 100;
		double overlap   = 0.9f;  // �غ��ʣ�90%
		double sound_velocity = 1500.0f; // �����ٶ�
		double sample_frq = 60e6;
		double prf = 1/300e-6;

		//CvMat *pmatSub = cvCreateMat(dep_end - dep_start + 1, strainMat->cols, strainMat->type);
		CvMat *pmatSub = cvCreateMatHeader(dep_end - dep_start + 1, strainMat->cols, strainMat->type);

		cvGetSubRect(strainMat, pmatSub, cvRect(0, 50, 299, 200));
		CvMat *pmatRadon = 0;
		CStrain::RadonSum(pmatSub, &pmatRadon);

		double  min_val;
		double  max_val;
		CvPoint min_loc;
		CvPoint max_loc;
		cvMinMaxLoc(pmatRadon, &min_val, &max_val, &min_loc, &max_loc);
		double v = ((dep_end - dep_start) * win_size * (1 - overlap) * sound_velocity / sample_frq / 2) 
			/ ((max_loc.x - max_loc.y) / prf);
		double e = v * v * 3;

		cvReleaseMat(&pmatRadon);
		cvReleaseMat(&pmatSub);

		TRACE("V=%f; E=%f;\n", v, e);
	}

	cvReleaseMat(&strainMat);
}


ELASTO_DLL_API int ElastoInit(const char *configfile)
{	
	g_lpEpHander = NULL;
	g_lpParam    = NULL;
	//TestCalcStrain();
	ReadSysConfig(configfile, g_paramConfig);
	g_paramConfig.prf = 1 / 300e-6;
	return 0;
}


ELASTO_DLL_API void ElastoRelease()
{	

}


ELASTO_DLL_API EPHandler ElastoRegisterHandler(EPHandler hander, void *lpParam)
{
	EPHandler old_hander = g_lpEpHander;
	g_lpEpHander = hander;
	g_lpParam    = lpParam;
	return old_hander;
}


ELASTO_DLL_API  void  ElastoGetConfig(ConfigParam &param)
{
	// Ϊ�˼�ʱ��Ӧ���õ��޸ģ�ÿ���ⲿ����ǰ����һ�������ļ���
	ReadSysConfig(DefaultElastoConfFile, g_paramConfig);
    param = g_paramConfig;
}



ELASTO_DLL_API int ElastoProcess(const EInput &in, EOutput &out)
{
	                                                        
	ReadSysConfig(DefaultElastoConfFile, g_paramConfig);             // ���ȶ�ȡ���ã���ʱ��Ӧ���õ��޸ġ�

	int  error = EE_OK;
	std::string filename;
	filename = in.filepath_s;
	test = new CData(in.rows, in.cols);                       
	test->readData(in.pDatas);                                       //��ȡԭʼRF����          

	//distest = new CDisplacement(test->getData(), g_paramConfig.windowHW, g_paramConfig.step);
	distest = new CDisplacement(test->getSubData(g_paramConfig.box_x, g_paramConfig.box_y, g_paramConfig.box_w, g_paramConfig.box_h), g_paramConfig.windowHW, g_paramConfig.step);

	datatest = new CDataProcess();

	strtest  = new CStrain();

	bpfilt   = new CFilter(g_paramConfig.bpfilt_file.c_str());      //��ȡ��ͨ�˲�����ͷ

	lpfilt   = new CFilter(g_paramConfig.lpfilt_file.c_str());      //��ȡ��ͨ�˲�����ͷ

	//CTestTime ttime;
	//CString info;
    //long timeout;

	                                                               // ���RF����ȫ��С��ĳ����ֵ��˵��̽ͷδ�Ӵ����壻�ɴ�ȷ����������㷨����ֱ�ӷ�����Чֵ

#if 1
	{
		double min_v;
		double max_v;
		cvMinMaxLoc(bpfilt->outDataMat, &min_v, &max_v);

		TRACE("RF-min=%f, max=%f\n", min_v, max_v);
		if (max_v < RF_NO_BODY_THRESHOLD)
		{
			error = EE_NO_BODY;
			out.e = -1.0f;
			out.v = -1.0f;
			goto exit;
		}
	}
#endif



	//ttime.run();
	datatest->doProcess(bpfilt);               //��ͨ�˲�
	//timeout = ttime.stop();
	//info.Format(_T("bandpass-filter-timeout=%dms\n"), timeout);
	//CLog::Instance()->Write(info.GetString(), info.GetLength());

	// ֪ͨ�۲���
	if (g_lpEpHander)
	{
		(*g_lpEpHander)(EP_POST_FILTER, bpfilt->outDataMat, g_lpParam);
	}
	//MakeImage(bpfilt->outDataMat, in.filepath_d);
	SaveDataFile("bpfilt.dat", bpfilt->outDataMat);

	//ttime.run();
	datatest->doProcess(distest);            //��λ��

	//timeout = ttime.stop();
	//info.Format(_T("displace-timeout=%dms\n"), timeout);
	//CLog::Instance()->Write(info.GetString(), info.GetLength());

	//ttime.run();
	datatest->doProcess(lpfilt);  //��ͨ�˲�
	//timeout = ttime.stop();
	//info.Format("lowpass-timeout=%dms\n", timeout);
	//CLog::Instance()->Write(info.GetString(), info.GetLength());

	//MakeImage(lpfilt->outDataMat, "disp_lp.bmp");
	//SaveDataFile("disp_lp.dat", lpfilt->outDataMat);
	
	MakeImage(distest->outDataMat, in.filepath_d);

	SaveDataFile("displace.dat", distest->outDataMat);

	if (g_lpEpHander)// ֪ͨ�۲���
	{
		(*g_lpEpHander)(EP_POST_DISPLACEMENT, distest->outDataMat, g_lpParam);
	}

	//ttime.run();

	strtest->CalcStrain3(in, out);//��Ӧ��ͼ������ģ��

	//timeout = ttime.stop();
	//info.Format("calc-strain-timeout=%dms\n", timeout);
	//CLog::Instance()->Write(info.GetString(), info.GetLength());

exit:
	CvMat *pmat = test->getData();
	cvReleaseMat(&pmat);
	cvReleaseMat(&strtest->outDataMat);
	delete strtest;
	delete lpfilt;
	delete distest;
	delete bpfilt;
	delete datatest;
	delete test;

	return error;
}

#if 0


ELASTO_DLL_API int   GetStrainImageAndElasto(const char *file_image, float &e, float *input, int rows, int cols)
{
	std::string filename;
	filename = file_image;
	test = new CData(rows, cols);
	test->readData(input);
	datatest = new CDataProcess();
	distest = new CDisplacement(test->getData(), 100, 10);
	strtest = new CStrain();
	bpfilt = new CFilter("bandpass30.txt");
	//lpfilt = new CFilter("lowpass50.txt");
	lpfilt = new CFilter("lowpass30.txt");
	float YoungModulus;

	datatest->doProcess(bpfilt);   //��ͨ�˲�

	datatest->doProcess(distest);  //��λ��

	datatest->doProcess(lpfilt);  //��ͨ�˲�
	  
	YoungModulus = strtest->strainAlgorithm(in, out);  //��Ӧ��ͼ������ģ��
	
	delete strtest;
	delete lpfilt;
	delete distest;
	delete bpfilt;
	delete datatest;
	delete test;
	
	e = YoungModulus;

	return 0;

}

#endif


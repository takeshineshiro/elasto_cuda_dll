#ifndef CELASTO_H_H_H
#define CELASTO_H_H_H
#pragma   once 

#if 1

//#ifdef ELASTO_DLL_EXPORTS
#ifdef ELASTO_CUDA_DLL_EXPORTS
#define ELASTO_DLL_API __declspec(dllexport)
#else
#define ELASTO_DLL_API __declspec(dllimport)   
#endif

#else

#define ELASTO_DLL_API

#endif

#include <iostream>
#include "SysConfig.h"

#ifdef __cplusplus
extern "C"{
#endif

struct CvMat;


//////////////////////////////////////////////////////////////////////////
// ��ʼ����
// ����ֵ��
//  0�� ��ʾ�ɹ���
//  ��0�� ��ʾʧ��
//////////////////////////////////////////////////////////////////////////
ELASTO_DLL_API int ElastoInit(const char *configfile = DefaultElastoConfFile);

ELASTO_DLL_API void ElastoRelease();


// ����
typedef struct  EInput
{
	float *pDatas;
	int    rows;
	int    cols;
	const char * filepath_d; // filepath of displacement image
	const char * filepath_s; // filepath of strain image

	EInput()
	{
		pDatas = 0;
		rows = -1;
		cols = -1;
		filepath_s = "";
		filepath_d = "";
	}

	~EInput()
	{
		if (pDatas)   delete [] pDatas;
	}

	void CreateDatas(int size)
	{
		if (pDatas)
		{
			delete [] pDatas;
			pDatas = 0;
		}
		pDatas = new float[size];
	}

} EInput, *PEInput;

// ���
typedef struct  
{
	float  v;  // velocity
	float  e;  // modulus
} EOutput, *PEOutput;

// ���Բ����¼�����
enum EProcessEvent
{
	EP_POST_FILTER,             // �˲����
	EP_POST_DISPLACEMENT,       // λ�Ƽ������
	EP_POST_STRAIN,             // Ӧ��������
};

// Elasto Prcoess Error Code
enum
{
	EE_OK,
	EE_FAIL,
	EE_NO_BODY,  // ����δ�Ӵ�����
};

typedef void (* EPHandler)(EProcessEvent, CvMat *, void *);

//////////////////////////////////////////////////////////////////////////
// ȡ��Ӧ��ͼ�͵���ֵ
// �㷨�⴦�������RF���ݣ����Ӧ��ͼ�͵���ֵkPa
// Ӧ��ͼ��bmp��ʽ������file_imageָ�����ļ��У�����ģ����ֵ������e�����У�
// ������
//     file_image, ���룬 Ӧ��ͼ�ļ�������
//     e��         ����� ���浯��ģ��
//     input,      ���룬 RF���ݣ�float��ʽ
//     rows��      ���룬 ���ݵ��У�ɨ���ߵ�����
//     cols;       ���룬 ���ݵ��У�����һ��ɨ���ߵĲ�����
//  ���أ�
//     0��  �ɹ�
//    ������ ʧ��
//////////////////////////////////////////////////////////////////////////
//ELASTO_DLL_API  int   GetStrainImageAndElasto(const char *file_image, float &e, float *input, int rows, int cols);

ELASTO_DLL_API  int   ElastoProcess(const EInput &in, EOutput &out);

//////////////////////////////////////////////////////////////////////////
// ע��ص�����
// �����߿������㷨�ڲ�������̵��ض��׶εõ��ص�����Ļ���
//////////////////////////////////////////////////////////////////////////
ELASTO_DLL_API  EPHandler  ElastoRegisterHandler(EPHandler, void *lpParam);

#ifdef __cplusplus
}
#endif 

#endif
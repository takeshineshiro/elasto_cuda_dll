// test_cuda_dll.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdio.h>     //����C������
#include <stdlib.h>
#include <Windows.h>
//��ʾ/��̬�������Ӳ���
typedef void(*DLLFUNC)(void);//������Ҫ��dll�е��õĺ���ԭ�͵ĺ���ָ��

int _tmain(int argc, _TCHAR* argv[])
{
	HINSTANCE hcudaDll = LoadLibrary(__T("cuda_dll.dll"));//��̬�ؼ���CUDAdlltest.dll
	if (hcudaDll)
	{
		DLLFUNC dllFun = (DLLFUNC)GetProcAddress(hcudaDll, "showHelloCuda");//��ú���ָ��
		if (dllFun)
		{
			dllFun();//ִ��showHelloCuda����
		}
		else
		{
			printf("Can not find the function in dll!");//�������ں���������
		}
		FreeLibrary(hcudaDll);//��̬��ж��CUDAdlltest.dll

	}
	else
	{
		printf("Load dll fail!");
	}
	return 0;
}
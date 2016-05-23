// test_cuda_dll.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdio.h>     //引入C函数库
#include <stdlib.h>
#include <Windows.h>
//显示/动态加载链接测试
typedef void(*DLLFUNC)(void);//声明需要从dll中调用的函数原型的函数指针

int _tmain(int argc, _TCHAR* argv[])
{
	HINSTANCE hcudaDll = LoadLibrary(__T("cuda_dll.dll"));//动态地加载CUDAdlltest.dll
	if (hcudaDll)
	{
		DLLFUNC dllFun = (DLLFUNC)GetProcAddress(hcudaDll, "showHelloCuda");//获得函数指针
		if (dllFun)
		{
			dllFun();//执行showHelloCuda函数
		}
		else
		{
			printf("Can not find the function in dll!");//可能由于函数名错误
		}
		FreeLibrary(hcudaDll);//动态地卸载CUDAdlltest.dll

	}
	else
	{
		printf("Load dll fail!");
	}
	return 0;
}
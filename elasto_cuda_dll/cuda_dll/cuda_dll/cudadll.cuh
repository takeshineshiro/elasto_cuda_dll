#include <stdio.h>       //����C������-ʵ���ϱ��������Ӧ����C�ķ�ʽ���룬�������׺Ϊcpp����
#include <stdlib.h>
#include <cuda_runtime.h> //����CUDA����ʱ��ͷ�ļ�

#ifdef __cplusplus //ָ�������ı��뷽ʽ���Եõ�û���κ����εĺ�����
extern "C"
{
#endif

#ifdef CUDADLLTEST_EXPORTS
#define CUDADLLTEST_API __declspec(dllexport) //�������ź궨��
#else
#define CUDADLLTEST_API __declspec(dllimport)
#endif

	extern CUDADLLTEST_API int count;       //Ҫ������ȫ�ֱ���

	CUDADLLTEST_API bool InitCUDA(void);    //Ҫ������CUDA��ʼ������

	CUDADLLTEST_API void showHelloCuda(void); //Ҫ�����Ĳ��Ժ���

#ifdef __cplusplus
}
#endif
#include "stdafx.h" //����Ԥ����ͷ�ļ�

#include "CUDAdll.cuh" //���뵼����������ͷ�ļ�



//��ʼ��CUDA
extern int count = 0;


bool InitCUDA(void)//CUDA��ʼ������
{

	printf("Start to detecte devices.........\n");//��ʾ��⵽���豸��

	cudaGetDeviceCount(&count);//�������������ڵ���1.0���豸��

	if (count == 0){

		fprintf(stderr, "There is no device.\n");

		return false;

	}


	printf("%d device/s detected.\n", count);//��ʾ��⵽���豸��


	int i;

	for (i = 0; i < count; i++){//������֤��⵽���豸�Ƿ�֧��CUDA

		cudaDeviceProp prop;

		if (cudaGetDeviceProperties(&prop, i) == cudaSuccess) {//����豸���Բ���֤�Ƿ���ȷ

			if (prop.major >= 1)//��֤�����������������������ĵ�һλ���Ƿ����1

			{
				printf("Device %d: %s supports CUDA %d.%d.\n", i + 1, prop.name, prop.major, prop.minor);//��ʾ��⵽���豸֧�ֵ�CUDA�汾
				break;


			}
		}
	}

	if (i == count) {//û��֧��CUDA1.x���豸
		fprintf(stderr, "There is no device supporting CUDA 1.x.\n");
		return false;
	}
	cudaSetDevice(i);//�����豸Ϊ�����̵߳ĵ�ǰ�豸
	return true;
}


void showHelloCuda(void)//����CUDA��ʼ������
{
	if (!InitCUDA()) //��ʼ��ʧ��
	{
		printf("Sorry,CUDA has not been initialized.\n");
		return;
	}
	printf("Hello GPU! CUDA has been initialized.\n");
}
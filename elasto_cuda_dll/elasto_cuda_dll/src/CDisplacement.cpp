#include "stdafx.h"

#include "CDisplacement.h"
#include "CElasto.h"

#include <stdio.h>
#include <iostream>

#include "CFilter.h"

#define SIGN(x) (((x)>=0)?(1):(0))

const float PI = 3.141592627f;

extern ConfigParam g_paramConfig;

//////////////////////////////////////////////////////////////////////////
//	�����ṹ
//////////////////////////////////////////////////////////////////////////
typedef struct plural
{
	float real;
	float img;
} plural;


CDisplacement::CDisplacement(CvMat *dataMat, int win, int step)
{
	outDataMat = dataMat;
	winSize = win;
	stepSize = step;
}

void CDisplacement::Do()
{
#if 1

	displacementAlgorithm();

#else
	
	ComputeDispalcement();

#endif
}



//////////////////////////////////////////////////////////////////////////
// opencv��C�ӿں�������಻����ĵط�������������ڴ�й©��
// ����Ĵ���templateMat, objectMat, �Ҹĳ���cvCreateMatHeader, Ӧ�ø�����
// ԭ���Ĵ����ƺ��п����ڴ�й¶�� data.ptrָ���ֵ�Ѿ���д�ˡ���ֵ���releaseʱȴû�б���
// ������opencv�ڲ������ڷ�������Ͽ��ǲ��ܣ�������û���㹻���opencv�����
//     ���
//////////////////////////////////////////////////////////////////////////
void CDisplacement::displacementAlgorithm()
{
	int    multiWin = 2;   //�󴰿ڶ�С���ڵı���
	int    winNum   = static_cast<int>(outDataMat->cols - multiWin * winSize) / stepSize;  //������Ҫƥ��Ķ���
	CvMat *disMat      = cvCreateMat(outDataMat->rows - 1, winNum, CV_32FC1);     //���ڱ���λ�Ƶľ���ƥ��ó�������λ�ƾ�����ֵ��Ӧ��Ϊ������
	
	CvMat *templateMat = cvCreateMatHeader(1, winSize, CV_32FC1);  //����ƥ���ģ�� 

	CvMat *objectMat   = cvCreateMatHeader(1, multiWin * winSize, CV_32FC1);  //����ƥ���Ŀ��

	CvMat *resultMat   = cvCreateMat(1, (multiWin - 1) * winSize + 1, CV_32FC1);  //���ڴ��ƥ����

	//CvMat *maxMat      = cvCreateMat(outDataMat->rows - 1, winNum, CV_32FC1);    //���ڴ洢ÿ��ƥ������ƥ����

	double min, max;  //ƥ�����е����ֵ
	CvPoint max_loc;  //ƥ�������ֵ����λ��
	double pmax, nmax; //���ֵ��ǰ������ֵ
	double displacement; //�洢λ��ֵ

	//#pragma  omp  parallel for schedule(dynamic)

	for (int i = 0; i < disMat->rows; ++i)   //��λ��
	{
		for (int j = 0; j < disMat->cols; ++j)
		{         /* ->data.ptr��ʾ���ݵ���ʼλ�� */
			templateMat->data.ptr = outDataMat->data.ptr + i * outDataMat->step 
				                     + static_cast<int>(sizeof(float) * ((multiWin - 1) * winSize / 2 + j * stepSize)); 
			objectMat->data.ptr   = outDataMat->data.ptr + (i + 1) * outDataMat->step + static_cast<int>(sizeof(float) * (j * stepSize)/* + 0.5*/);
			
			cvMatchTemplate(objectMat, templateMat, resultMat, CV_TM_CCORR_NORMED); //ƥ��

			cvMinMaxLoc(resultMat, &min, &max, NULL, &max_loc);

			pmax         = *static_cast<float*>(static_cast<void*>(resultMat->data.ptr + sizeof(float)*max_loc.x - sizeof(float)));  //ȡ���ֵ��ǰһ��ֵ

			nmax         = *static_cast<float*>(static_cast<void*>(resultMat->data.ptr + sizeof(float)*max_loc.x + sizeof(float)));  //ȡ���ֵ�ĺ�һ��ֵ

			displacement = (multiWin - 1) * winSize / 2 - max_loc.x - (pmax - nmax) / (2 * (pmax - 2 * max + nmax));                 //��ֵ�õ�λ��

			CV_MAT_ELEM(*disMat,float, i, j) = static_cast<float>(displacement); //�����ŵ�λ�ƾ�����
		}
	}

	//��ɾ�� 2009-10-29 ³���Բ���
	for (int i = 1; i < disMat->rows - 1; ++i)   //λ��ƽ����ȥ����㣬�Ի����ϵ��Ϊ���ݡ�
	{
		for (int j = 0; j < disMat->cols - 1; ++j)
		{
			if (abs(CV_MAT_ELEM(*disMat, float, i, j)) > 12)
			{
				CV_MAT_ELEM(*disMat, float, i, j) = CV_MAT_ELEM(*disMat, float, i - 1, j);  
			}
		}
	}




	//λ�Ƶ���
	for (int i = 0; i < disMat->cols; ++i)
	{
		for(int j = 0; j < disMat->rows - 1; ++j)
		{
			CV_MAT_ELEM(*disMat, float, j + 1, i) = CV_MAT_ELEM(*disMat, float, j + 1, i) + CV_MAT_ELEM(*disMat, float, j, i);
		}
	}





	//��ֵ�˲�
	int N = 100;

	float sum = 0;

	CvMat *tempMat = cvCreateMat(disMat->rows, disMat->cols + N - 1, disMat->type);

	for (int j = 0; j < N - 1; j++)
	{
		for (int i = 0; i < disMat->rows; i++)                                            //ǰN-1�в�0 
		{
			CV_MAT_ELEM(*tempMat, float, i, j) = CV_MAT_ELEM(*disMat, float, i, 0);
		}
	}



	for (int j = N - 1; j < disMat->cols + N - 1; j++)                                   //�м䲹��
	{
		for (int i = 0; i < disMat->rows; i++)
		{
			CV_MAT_ELEM(*tempMat, float, i, j) = CV_MAT_ELEM(*disMat, float, i, j - N + 1);
		}
	}



	for (int i = 0; i < disMat->rows; i++)
	{
		for (int j = 0; j < disMat->cols; j++)
		{
			for (int k = j; k < j + N; k++)                                                                 
			{
				sum += CV_MAT_ELEM(*tempMat, float, i ,k);                          //���ۼ�ƽ��
			}

			CV_MAT_ELEM(*disMat, float, i, j) = sum / N;

			sum = 0;

		}

	}



	cvReleaseMat(&tempMat);


	cvReleaseMat(&outDataMat);

	outDataMat = cvCreateMat(disMat->rows, disMat->cols, disMat->type);

#if 0
	//����ӳ���match filter,��ǿλ���ź�
	double match[78] = {0,0,0,0,0,0,0.0941,0.1874,0.279,0.3682,0.454,0.5359,0.613,0.6846,0.7502,0.8091,0.8608,0.9049,0.9409,0.9686,0.9877,0.998,0.9995,
		0.9921,0.9759,0.951,0.9177,0.8762,0.827,0.7704,0.7069,0.6372,0.5619,0.4815,0.3969,0.3087,0.2179,0.125,0.0311,-0.0631,-0.1568,
		-0.249,-0.3391,-0.4261,-0.5094,-0.5881,-0.6616,-0.7292,-0.7904,-0.8446,-0.8912,-0.9299,-0.9604,-0.9824,-0.9956,-1,-0.9955,
		-0.9822,-0.9602,-0.9296,-0.8908,-0.844,-0.7898,-0.7286,-0.6609,-0.5873,-0.5085,-0.4253,-0.3382,-0.2481,-0.1558,-0.0622,0,0,0,0,0,0};
	
	double tmp = 0;
	for (int i = 0; i < disMat->cols; ++i)
	{
		for (int j = 0; j < disMat->rows; ++j)
		{
			for (int k = 0; k <= j; ++k)
			{
				tmp += cvmGet(disMat, k, i) * match[(j - k) < 78 ? (j - k) : 0];  /*���*/
			}
			cvmSet(outDataMat, j, i, tmp);
			tmp = 0;
		}
	}

#else

	{

		 CFilter match_filter(g_paramConfig.matchfilt_file.c_str());


		 match_filter.DoTimeFieldFilter(disMat);

	}

#endif

	cvReleaseMat(&disMat);

	cvReleaseMatHeader(&templateMat);

	cvReleaseMatHeader(&objectMat);

	cvReleaseMat(&resultMat);

}


void hilbertTrsf(CvMat* &idat, int fullFlag)
{
	CvMat* iclone;
	int i;

	if (fullFlag != 0)
	{
		iclone = cvCloneMat(idat);
	}
	cvDFT(idat, idat, CV_DXT_FORWARD|CV_DXT_ROWS);
	for (i = 0; i != idat->rows; ++i)
	{
		int j = 0;
		for (j = 1; j != idat->cols/2; ++j)
		{
			CvScalar tmp;
			tmp = cvScalar(cvGet2D(idat, i, j).val[1], -(cvGet2D(idat, i, j).val[0]), 0, 0);
			cvSet2D(idat, i, j, tmp);
		}
		for (j = idat->cols/2 +1; j != idat->cols; ++j)
		{
			CvScalar tmp;
			tmp = cvScalar(-(cvGet2D(idat, i, j).val[1]), cvGet2D(idat, i, j).val[0], 0, 0);
			cvSet2D(idat, i, j, tmp);
		}
		CvScalar tmp;
		tmp = cvScalar(0, 0, 0, 0);
		cvSet2D(idat, i, 0, tmp);
		cvSet2D(idat, i, idat->cols/2, tmp);
	}
	cvDFT(idat, idat, CV_DXT_INVERSE_SCALE|CV_DXT_ROWS);
	for (i = 0; i != idat->rows; ++i)
	{
		for (int j = 0; j != idat->cols; ++j)
		{
			CvScalar tmp;
			tmp = cvScalar((cvGet2D(idat, i, j)).val[1], (cvGet2D(idat, i, j)).val[0], 0, 0);
			cvSet2D(idat, i, j, tmp);
		}
	}
	if (fullFlag != 0)
	{
		cvAdd(idat, iclone, idat);
		cvReleaseMat(&iclone);
		iclone = NULL;
	}
}

CvMat* reltvToAbs(CvMat *relativeDisp)
{
    for (int j = 0; j != relativeDisp->cols; ++j)
    {
        for (int i = 0; i != relativeDisp->rows - 1; ++i) 
        {
            if (i < 5)	// 2012.11.29 ����֪
            {
                *(static_cast<float*>(static_cast<void*>(CV_MAT_ELEM_PTR(*relativeDisp, i + 1, j)))) = 0;				
            }
            else
            {
                *(static_cast<float*>(static_cast<void*>(CV_MAT_ELEM_PTR(*relativeDisp, i + 1, j)))) = 
                    *(static_cast<float*>(static_cast<void*>(CV_MAT_ELEM_PTR(*relativeDisp, i + 1, j)))) + 
                    *(static_cast<float*>(static_cast<void*>(CV_MAT_ELEM_PTR(*relativeDisp, i, j))));
            }
        }
    } //�����м����;
	
	/* ���������Թ����ԣ�������; 
	float saveDisp=0; //ÿһ�����λ��ֵ��д��λ�ƾ���;
	std::fstream fout;
	fout.open("AbsdispMat.dat", std::ios::out); //�洢����;
	for (int i = 0; i != relativeDisp->rows; ++i)
	{
		for (int j = 0; j != relativeDisp->cols; ++j)
		{
			saveDisp=*(static_cast<float*>(static_cast<void*>(CV_MAT_ELEM_PTR(*relativeDisp, i, j))));
			fout<<saveDisp<<std::endl; //�洢����;
		}
	}	
	fout.close();/* ���������Թ����ԣ�������; */
	
	return relativeDisp;
}

// 2012.9.5 ��������нϴ�Ķ� ����֪
void singularFilter(CvMat *inputY, float threshold)		
{
	//std::fstream fout;
	//fout.open("svFiltDisp.dat", std::ios::out); //�洢����;

	int rowCounter=0;
	int columnCounter=0;
	float* tempAddress=inputY->data.fl;
	int frames=inputY->rows;
	int totalpoints=inputY->cols;

	for(rowCounter=0;rowCounter<frames;rowCounter++)	
	{
		for(columnCounter=0;columnCounter<totalpoints;columnCounter++)
		{
			if (fabs(*(tempAddress))>threshold)
			{
					*(tempAddress)=0;//threshold/2;		// 2012.9.5 ������ֵ��ȫ������  ����֪
			}
			tempAddress++; //�ƶ�ָ���ַ��
		}
	}

	tempAddress=inputY->data.fl;

	for(rowCounter=0;rowCounter<frames;rowCounter++)
	{
		for(columnCounter=0;columnCounter<totalpoints;columnCounter++)
		{
			if (rowCounter==0 || rowCounter==frames - 1)
			{
			} 
			else
			{
			    if ((fabs(*(tempAddress)-*(tempAddress-totalpoints))>threshold) && (fabs(*(tempAddress)-*(tempAddress+totalpoints))>threshold))
			    {
					*(tempAddress)=(*(tempAddress-totalpoints) + *(tempAddress+totalpoints))/2;
			    }	// 2012.9.5 ��ǰ�����ݵĲ������ֵ��ȡǰ�����ݵľ�ֵ ����֪
			}
			tempAddress++; //�ƶ�ָ���ַ��
		}
	}


/*	
	//���������Թ����ԣ�������; 
	float saveDisp=0; //ÿһ�����λ��ֵ��д��λ�ƾ���;
	std::fstream fout;
	fout.open("SingdispMat.dat", std::ios::out); //�洢����;
	for (int i = 0; i != inputY->rows; ++i)
	{
		for (int j = 0; j != inputY->cols; ++j)
		{
			saveDisp=*(static_cast<float*>(static_cast<void*>(CV_MAT_ELEM_PTR(*inputY, i, j))));
			fout<<saveDisp<<std::endl; //�洢����;
		}
	}	
	fout.close();/* ���������Թ����ԣ�������; 
	
	*/

}

CvMat* CDisplacement::DoCalcDisp(CvMat *inputMat, int windowHW, int maxLag, int step, float fs, float c)
{
	//plural* inputY,int inputHeight,int inputWidth
	hilbertTrsf(inputMat, 1);

	plural* inputY = (plural*)inputMat->data.fl;
	int inputHeight = inputMat->height;
	int inputWidth = inputMat->width;
	int endIndex = inputWidth-1;
	int idealEndIndex = endIndex-(windowHW-1);
	int idealDistance = idealEndIndex;
	int gapN = idealDistance/step;
	int stepN = gapN + 1;
	int realEndIndex = gapN * step;

	int slopeHeight = maxLag * 2 + 1;
	int slopeWidth  = stepN;
	plural* slopeM = new plural[slopeHeight*slopeWidth];
	plural* slopeMPointer = slopeM;
	plural* slopeLineXPointer = inputY;
	plural* slopeLineYPointer = inputY + inputWidth + maxLag;
	plural* startXPointer = slopeLineXPointer;
	plural* startYPointer = slopeLineYPointer;

	int outputWidth  = stepN;
	int outputHeight = inputHeight - 1;
	CvMat* outputMat = cvCreateMat(outputHeight, outputWidth, CV_32FC1);

	//int outputSize=outputHeight*outputWidth;
	float* outputY = outputMat->data.fl;
	float* outputYPointer = outputY;

	int rowCounter =- maxLag;

	int  shortestLength = windowHW - abs(rowCounter);
	int lineWindowHW = shortestLength;

	int  lineLength = windowHW + (stepN - 1) * step;
	int fullSlopeMWidth  = lineLength;
	int fullSlopeMHeight = slopeHeight;
	int fullSlopeMSize   = fullSlopeMHeight*fullSlopeMWidth;
	plural* fullSlopeM = new plural[fullSlopeMSize];
	plural* fullSlopeMPointer = fullSlopeM;
	plural* fullSlopeMLinePointer = fullSlopeM;

	int stepCounter = 0;
	int lineWindowHWCounter = 0;

	float deltaSlopeMReal = 0;
	float deltaSlopeMImg  = 0;
	float tempSlopeMReal  = 0;
	float tempSlopeMImg   = 0;

	float deltaFrontBackReal = 0;
	float deltaFrontBackImg  = 0;

	int dstHeight = slopeWidth;
	int dstWidth  = slopeHeight;
	int dstSize   = dstHeight*dstWidth;
	plural* dstM = new plural[dstSize];
	plural* srcPointer = slopeM;
	plural* dstPointer = dstM;

	int dstJump = dstSize - 1;

	plural* startPointer = dstM;
	plural* currentPointer;

	int lowerBound =- maxLag + 1;
	int upperBound =  maxLag - 1;
	int minDelay   =- maxLag;
	int maxDelay   =  maxLag;
	int minDelayNext = minDelay + 1;

	int last2MaxDelay = minDelay;
	int lastMaxDelay  = minDelay;
	int currentMaxDelay = minDelay;
	int estimatedMaxDelay = minDelay;
	int delayCounter = minDelay;

	float currentRealAbs;
	float currentImgAbs;
	float currentModulus;

	float currentMaxRealAbs;
	float currentMaxImgAbs;
	float currentMaxModulus;

	float angleLast2;
	float angleLast;
	float angleCurrent;
	float angleNext;
	float angleNext2;

	float deltaLast2;
	float deltaLast;
	float deltaNext;
	float deltaNext2;

	float deltaLast2True;
	float deltaLastTrue;
	float deltaNextTrue;
	float deltaNext2True;

	int reasonableLowerBound =- maxLag + 2;
	int reasonableUpperBound =  maxLag-2;

	plural* last2Pointer = NULL;
	plural* lastPointer  = NULL;
	plural* currentMaxCrossPointer = NULL;
	plural* nextPointer  = NULL;
	plural* next2Pointer = NULL;
	plural* currentMaxDelayPointer = NULL;

	float deltaDisp = 0;

	int columnCounter = 0;

	for (int bigRowCounter = 0; bigRowCounter < outputHeight; bigRowCounter++)
	{
		slopeMPointer = slopeM;
		fullSlopeMLinePointer = fullSlopeM;
		
		slopeLineXPointer = inputY + bigRowCounter*inputWidth;
		slopeLineYPointer = slopeLineXPointer + inputWidth+maxLag;
		
		lineWindowHW = shortestLength;	
		
		for (rowCounter =- maxLag; rowCounter <= maxLag; rowCounter++, fullSlopeMLinePointer += fullSlopeMWidth)
		{
			startXPointer = slopeLineXPointer;
			startYPointer = slopeLineYPointer;
			fullSlopeMPointer = fullSlopeMLinePointer;
			
			tempSlopeMReal = 0;
			tempSlopeMImg  = 0;
			
			columnCounter = 0;
			for (lineWindowHWCounter = 0; lineWindowHWCounter < lineWindowHW; lineWindowHWCounter++, fullSlopeMPointer++, startXPointer++, startYPointer++)
			{
				deltaSlopeMReal = startXPointer->real * startYPointer->real + startXPointer->img  * startYPointer->img;
				deltaSlopeMImg  = startYPointer->real * startXPointer->img  - startXPointer->real * startYPointer->img;
				fullSlopeMPointer->real = deltaSlopeMReal;
				fullSlopeMPointer->img  = deltaSlopeMImg;
				tempSlopeMReal += deltaSlopeMReal;
				tempSlopeMImg  += deltaSlopeMImg;
			}
			slopeMPointer->real = tempSlopeMReal;
			slopeMPointer->img  = tempSlopeMImg;
			
			slopeMPointer++;
			
			plural* frontPointer = fullSlopeMPointer - lineWindowHW;
			
			for (columnCounter = 1; columnCounter < slopeWidth; columnCounter++)
			{			
				deltaFrontBackReal = 0;
				deltaFrontBackImg  = 0;
				
				for (stepCounter = 0; stepCounter < step; stepCounter++, startXPointer++, startYPointer++, fullSlopeMPointer++, frontPointer++)
				{
					deltaSlopeMReal = startXPointer->real * startYPointer->real + startXPointer->img * startYPointer->img;
					deltaSlopeMImg  = startYPointer->real * startXPointer->img-startXPointer->real * startYPointer->img;
					fullSlopeMPointer->real = deltaSlopeMReal;
					fullSlopeMPointer->img  = deltaSlopeMImg;
					deltaFrontBackReal += deltaSlopeMReal-frontPointer->real;
					deltaFrontBackImg  += deltaSlopeMImg-frontPointer->img;
				}
				tempSlopeMReal += deltaFrontBackReal;
				tempSlopeMImg  += deltaFrontBackImg;
				
				slopeMPointer->real = tempSlopeMReal;
				slopeMPointer->img  = tempSlopeMImg;
				slopeMPointer++;
			}
			
			if (rowCounter<0) 
			{
				slopeLineYPointer--;
				lineWindowHW++;
			}
			else
			{
				slopeLineXPointer++;
				lineWindowHW--;
			}
		}
		
		srcPointer = slopeM;
		dstPointer = dstM;
		
		for (rowCounter = 0; rowCounter<slopeHeight; rowCounter++)
		{
			for (columnCounter = 0; columnCounter < slopeWidth; columnCounter++)
			{
				dstPointer->real = srcPointer->real;
				dstPointer->img  = srcPointer->img;
				srcPointer++;
				dstPointer += dstWidth;
			}
			dstPointer -= dstJump;
		}
		
		currentPointer = dstM;
		startPointer   = dstM;
		
		currentMaxRealAbs = fabs(currentPointer->real);
		currentMaxImgAbs  = fabs(currentPointer->img);
		currentMaxModulus = currentMaxRealAbs * currentMaxRealAbs + currentMaxImgAbs * currentMaxImgAbs;
		currentPointer++;
		
		rowCounter = 0;//��ʼ��һ����ȡ���delay
		for (delayCounter = minDelayNext; delayCounter <= maxLag; delayCounter++, currentPointer++)
		{
			currentRealAbs = fabs(currentPointer->real);
			currentImgAbs  = fabs(currentPointer->img);
			if (currentRealAbs > currentMaxRealAbs || currentImgAbs > currentMaxImgAbs)
			{
				currentModulus = currentRealAbs*currentRealAbs + currentImgAbs*currentImgAbs;
				if (currentModulus > currentMaxModulus)
				{
					currentMaxModulus=currentModulus;
					currentMaxRealAbs=currentRealAbs;
					currentMaxImgAbs=currentImgAbs;
					currentMaxDelay=delayCounter;
				}
			}
		}	
		
		if(currentMaxDelay>=reasonableLowerBound&&currentMaxDelay<=reasonableUpperBound)
		{
			currentMaxDelayPointer=startPointer+currentMaxDelay+maxLag;
			last2Pointer=currentMaxDelayPointer-2;
			lastPointer=last2Pointer+1;
			nextPointer=currentMaxDelayPointer+1;
			next2Pointer=nextPointer+1;
			
			angleLast2=atan2(last2Pointer->img,last2Pointer->real)/PI;
			angleLast=atan2(lastPointer->img,lastPointer->real)/PI;
			angleCurrent=atan2(currentMaxDelayPointer->img,currentMaxDelayPointer->real)/PI;
			angleNext=atan2(nextPointer->img,nextPointer->real)/PI;
			angleNext2=atan2(next2Pointer->img,next2Pointer->real)/PI;
			
			deltaLast2=angleLast-angleLast2;
			deltaLast=angleCurrent-angleLast;
			deltaNext=angleNext-angleCurrent;
			deltaNext2=angleNext2-angleNext;
			
			deltaLast2True=deltaLast2-((int)deltaLast2)*2;
			deltaLastTrue=deltaLast-((int)deltaLast)*2;
			deltaNextTrue=deltaNext-((int)deltaNext)*2;
			deltaNext2True=deltaNext2-((int)deltaNext2)*2;
			
			deltaDisp=angleCurrent/(0.3f*(deltaNextTrue+deltaLastTrue)+0.2f*(deltaNext2True+deltaLast2True));
			//*outputYPointer=(currentMaxDelay+deltaDisp)*c/(2*fs);
			if (bigRowCounter == 0) //��һ�е�ʱ��
			{
				*outputYPointer=(currentMaxDelay+deltaDisp)*c/(2*fs);
			}
			else
			{
				outputYPointer++;
				*outputYPointer=(currentMaxDelay+deltaDisp)*c/(2*fs);
			}
		}
		else
		{
			//*outputYPointer=currentMaxDelay*c/(2*fs);
			if (bigRowCounter == 0) //��һ�е�ʱ��,��ʼָ�벻��+1��
			{
				*outputYPointer = currentMaxDelay*c/(2*fs);
			}
			else // ������ʱ����Ϊָ��ָ������Ѿ���ֵ�ĵ�ַ��ָ��������ƣ����򸲸ǵ���ǰ���ݣ�
			{
				outputYPointer++;
				*outputYPointer = currentMaxDelay*c/(2*fs);
			}
		}
		
		for(rowCounter=1;rowCounter<dstHeight;rowCounter++)
		{		
			startPointer+=dstWidth;
			
			last2MaxDelay=lastMaxDelay;
			lastMaxDelay=currentMaxDelay;
			estimatedMaxDelay=2*lastMaxDelay-last2MaxDelay;
			
			if(estimatedMaxDelay>maxDelay)
				estimatedMaxDelay=maxDelay;
			else if(estimatedMaxDelay<minDelay)
				estimatedMaxDelay=minDelay;
			
			currentMaxDelay=estimatedMaxDelay;
			
			currentPointer=startPointer+estimatedMaxDelay+maxLag;
			currentMaxRealAbs=(currentPointer->real);
			currentMaxImgAbs=(currentPointer->img);
			currentMaxModulus=currentMaxRealAbs*currentMaxRealAbs+currentMaxImgAbs*currentMaxImgAbs;
			currentPointer=startPointer;
			
			if(estimatedMaxDelay!=minDelay)
			{
				for(delayCounter=minDelay;delayCounter<currentMaxDelay;delayCounter++, currentPointer++)
				{
					currentRealAbs=fabs(currentPointer->real);
					currentImgAbs=fabs(currentPointer->img);
					if(currentRealAbs>currentMaxRealAbs||currentImgAbs>currentMaxImgAbs)
					{
						currentModulus=currentRealAbs*currentRealAbs+currentImgAbs*currentImgAbs;
						if(currentModulus>currentMaxModulus)
						{
							currentMaxModulus=currentModulus;
							currentMaxRealAbs=currentRealAbs;
							currentMaxImgAbs=currentImgAbs;
							currentMaxDelay=delayCounter;
						}
					}
				}
			}
			else
			{
				currentPointer++;
				delayCounter=minDelayNext;
			}
			
			for(;delayCounter<=maxDelay;delayCounter++,currentPointer++)
			{
				if(delayCounter!=estimatedMaxDelay)
				{
					currentRealAbs=fabs(currentPointer->real);
					currentImgAbs=fabs(currentPointer->img);
					if(currentRealAbs>currentMaxRealAbs||currentImgAbs>currentMaxImgAbs)
					{
						currentModulus=currentRealAbs*currentRealAbs+currentImgAbs*currentImgAbs;
						if(currentModulus>currentMaxModulus)
						{
							currentMaxModulus=currentModulus;
							currentMaxRealAbs=currentRealAbs;
							currentMaxImgAbs=currentImgAbs;
							currentMaxDelay=delayCounter;
						}
					}
				}
			}		
			
			if(currentMaxDelay>=reasonableLowerBound&&currentMaxDelay<=reasonableUpperBound)
			{
				currentMaxDelayPointer=startPointer+currentMaxDelay+maxLag;
				last2Pointer=currentMaxDelayPointer-2;
				lastPointer=last2Pointer+1;
				nextPointer=currentMaxDelayPointer+1;
				next2Pointer=nextPointer+1;
				
				angleLast2=atan2(last2Pointer->img,last2Pointer->real)/PI;
				angleLast=atan2(lastPointer->img,lastPointer->real)/PI;
				angleCurrent=atan2(currentMaxDelayPointer->img,currentMaxDelayPointer->real)/PI;
				angleNext=atan2(nextPointer->img,nextPointer->real)/PI;
				angleNext2=atan2(next2Pointer->img,next2Pointer->real)/PI;
				
				deltaLast2=angleLast-angleLast2;
				deltaLast=angleCurrent-angleLast;
				deltaNext=angleNext-angleCurrent;
				deltaNext2=angleNext2-angleNext;
				
				deltaLast2True=deltaLast2-((int)deltaLast2)*2;
				deltaLastTrue=deltaLast-((int)deltaLast)*2;
				deltaNextTrue=deltaNext-((int)deltaNext)*2;
				deltaNext2True=deltaNext2-((int)deltaNext2)*2;
				
				deltaDisp=angleCurrent/(0.3f*(deltaNextTrue+deltaLastTrue)+0.2f*(deltaNext2True+deltaLast2True));
				*(++outputYPointer)=(currentMaxDelay+deltaDisp)*c/(2*fs);
			}
			else
			{
				*(++outputYPointer)=currentMaxDelay*c/(2*fs);
			}
		}
	}

	delete[] slopeM;
	delete[] fullSlopeM;
	delete[] dstM;

	/*���������Թ����ԣ�������; outputHeight,outputWidth
	float saveDisp=0; 
	std::fstream fout;
	fout.open("dispMat.dat", std::ios::out); //�洢����;
	for (int i = 0; i != outputHeight; ++i)
	{
		for (int j = 0; j != outputWidth; ++j)
		{
			saveDisp=*(static_cast<float*>(static_cast<void*>(CV_MAT_ELEM_PTR(*outputMat, i, j))));
			fout<<saveDisp<<std::endl; //�洢����;
		}
	}	
	fout.close();/* ���������Թ����ԣ�������; */

	return outputMat;
}




void CDisplacement::ComputeDispalcement()
{
	CvMat *pmatIn = cvCreateMat(outDataMat->rows, outDataMat->cols, CV_32FC2);
	for (int i = 0; i < outDataMat->rows; i++)
	{
		for (int j = 0; j < outDataMat->cols; j++)
		{
			CV_MAT_ELEM(*pmatIn, float, i, j) = CV_MAT_ELEM(*outDataMat, float, i, j);
		}
	}

	CvMat * pmatDisp = DoCalcDisp(pmatIn, g_paramConfig.windowHW, g_paramConfig.maxLag, g_paramConfig.step, g_paramConfig.sampleFreqs, g_paramConfig.acousVel);
	singularFilter(pmatDisp, g_paramConfig.threshold);	//2012.9.5  ����ֵ�˲��������������޸� ����֪
	reltvToAbs    (pmatDisp);

	cvReleaseMat(&outDataMat);
#if 1
	outDataMat = pmatDisp;

#else
	outDataMat = cvCloneMat(pmatDisp);
	cvReleaseMat(&pmatDisp);
#endif
}



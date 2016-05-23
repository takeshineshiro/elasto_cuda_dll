// TestTime.h: interface for the CTestTime class.
// ������Ƶ������ڲ��� ���������ʱ��.
// �ı���ʽ��ansi����.��֧��unicode����.
//            
// history:
//     ����,  ���,  2012/3/26,
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TESTTIME_H__BED4EC0C_D99A_4741_AAF9_3A7256E418C2__INCLUDED_)
#define AFX_TESTTIME_H__BED4EC0C_D99A_4741_AAF9_3A7256E418C2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef  __AFX_H__
	#include <windows.h>

#endif

#include <time.h>
#include<string>


//��ȡ��ʱ��д�뵽mfc�ĵ��Դ����У�Ҳ���ǵ�����TRACE
class CTestTime
{
public:
	CTestTime();
	virtual ~CTestTime();

	virtual void run();
	virtual void stop(CString &text);//����ʱ�䣬�ַ���ʾ
	virtual long stop();// ����ʱ�䣬ms

protected:
    virtual long getTimeTicks();

    long  getTimeTicksOnTimeCount();

    long  getTimeTicksOnThreadTime();

    long  getTimeTicksOnClock();

//private:
	int     m_startTickCount;//ms
	int     m_stopTickCount; //ms
};

#endif // !defined(AFX_TESTTIME_H__BED4EC0C_D99A_4741_AAF9_3A7256E418C2__INCLUDED_)

//////////////////////////////////////////////////////////////////////////
//
//

#pragma once

#include <string>

const  char DefaultElastoConfFile[] = ".\\config.ini";

typedef struct ConfigParam
{
	//////////////////////////////////////////////////////////////////////////
	// �㷨�õ�����&&Start
	int     sampleFreqs;    //������;
	float   acousVel;       //����, m/s;
	float   prf;

	float   threshold;      // 2012.9.5 ����ֵ�˲�����ֵ ����֪
	int     windowHW;       // 2012.9.5 ����ش��ڳ��� ����֪
	int     maxLag;         // 2012.9.5 ����ؼ�������ƫ�ƣ�Խ�������Խ�� ����֪
	int     step;           // 2012.9.5 ����ؼ���Ĳ�����ÿ�����ӵ������ݵ�������˴����д󲿷ֵ㶼overlap�� ����֪

	int     fitline_pts;    // ��С���˷�����ֱ����ϵĵ������

	std::string  lpfilt_file;
	std::string  bpfilt_file;
	std::string  matchfilt_file;

	int     box_x; // input data mat
	int     box_y;
	int     box_w;
	int     box_h;
	int     sb_x;  // strain data sub mat for ladong transform
	int     sb_y;
	int     sb_w;
	int     sb_h;

	int     radon_step;
	int     radon_num;
	std::string  calc_type;

	//  �㷨�õ�����&&End
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// ͨѶЭ�����&&Start
	int   lps;              // lines per second,ÿ�����ϴ����ߵ�����
	int   sampleNumPerLine; // ÿ���߰����������������
	int   elmSize;          // ÿ����������ֽڳ���
	int   shearFrameLineNum;// ���в�����֡������ɨ���ߵ�����

	// ͨѶЭ�����&&End
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// UI���&&Start
	int  mm_fr;

	int  m_rect_x;
	int  m_rect_y;
	int  m_rect_w;
	int  m_rect_h;

	int  e_rect_x;
	int  e_rect_y;
	int  e_rect_w;
	int  e_rect_h;

	int  s_rect_x;
	int  s_rect_y;
	int  s_rect_w;
	int  s_rect_h;

	// UI���&&End
	//////////////////////////////////////////////////////////////////////////

	int nDyn;

} ConfigParam, SysConfig, *PSysConfig;

bool ReadSysConfig(const char *ini_file, ConfigParam &param);



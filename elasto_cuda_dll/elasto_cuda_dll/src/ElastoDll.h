// ���� ifdef ���Ǵ���ʹ�� DLL �������򵥵�
// ��ı�׼�������� DLL �е������ļ��������������϶���� ELASTODLL_EXPORTS
// ���ű���ġ���ʹ�ô� DLL ��
// �κ�������Ŀ�ϲ�Ӧ����˷��š�������Դ�ļ��а������ļ����κ�������Ŀ���Ὣ
// ELASTODLL_API ������Ϊ�Ǵ� DLL ����ģ����� DLL ���ô˺궨���
// ������Ϊ�Ǳ������ġ�
#ifdef ELASTODLL_EXPORTS
#define ELASTODLL_API __declspec(dllexport)
#else
#define ELASTODLL_API __declspec(dllimport)
#endif

// �����Ǵ� ElastoDll.dll ������
class ELASTODLL_API CElastoDll {
public:
	CElastoDll(void);
	// TODO: �ڴ�������ķ�����
};

extern ELASTODLL_API int nElastoDll;

ELASTODLL_API int fnElastoDll(void);

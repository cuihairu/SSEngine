/*!
!module SDSystemInfoϵͳ��Ϣ�ռ����
!moduletype common
������ȡ�ͻ������л�����Ӳ����Ϣ�������
���������Ҫ�����У�
.���CPUӲ�������Ϣ��
.����Կ�Ӳ�������Ϣ��
.����ڴ������Ϣ��
.��ò���ϵͳ�����Ϣ��
.���DirectX�汾��Ϣ��
.���ɴ�������ΨһID��
.�����ύ��������������ϵͳ��Ϣ�ַ�����

����ʹ��ʾ����!!!link \ʾ��\���ʾ��\SDSystemInfo@Demo1@ϵͳ��Ϣ�ռ�.htm SDSystemInfo Demo1 ϵͳ��Ϣ�ռ�!!!��

!inc SDSysteminfo.h
*/ 
#ifndef __SSCP_SDSYSTEMINFO_ISYSTEMINFO_H__
#define __SSCP_SDSYSTEMINFO_ISYSTEMINFO_H__

#include "sdbase.h"
// Compatibility: alias SSSCPVersion to SSSVersion for this SDK
typedef SSSVersion SSSCPVersion;



namespace SSCP
{

	// ģ������
	const char SDSYSTEMINFO_MODULENAME[] = "SSSystemInfo";

	// �汾��
	const SSSCPVersion	SDSYSTEMINFO_VERSION = {0,3,0,1};


	//=============================================================================================================================
	/*!
	!macro SDSYSTEMINFO_CPU_
	CPU�������ơ�
	!inc SDSystemInfo.h
	*/
	#define SDSYSTEMINFO_CPU_OTHER			0		//! δ֪����
	#define SDSYSTEMINFO_CPU_INTEL			1		//! Intel
	#define SDSYSTEMINFO_CPU_AMD			2		//! AMD
	#define SDSYSTEMINFO_CPU_UMC			3		//! UMC
	#define SDSYSTEMINFO_CPU_CYRIX			4		//! Cyrix
	#define SDSYSTEMINFO_CPU_NEXGEN			5		//! NexGen
	#define SDSYSTEMINFO_CPU_CENTAUR		6		//! Centaur
	#define SDSYSTEMINFO_CPU_RISE			7		//! Rise
	#define SDSYSTEMINFO_CPU_TMX86			8		//! TMx86
	#define SDSYSTEMINFO_CPU_NSC			9		//! NSC

	/*!
	!macro SDSYSTEMINFO_GPU_
	GPU�������ơ�
	!inc SDSystemInfo.h
	*/
	#define SDSYSTEMINFO_GPU_UNKONW			0		//! δ֪����
	#define SDSYSTEMINFO_GPU_ATI			1		//! ATI
	#define SDSYSTEMINFO_GPU_NVIDIA			2		//! nVidia
	#define SDSYSTEMINFO_GPU_POWERVR		3		//! PowerVR
	#define SDSYSTEMINFO_GPU_SIS			4		//! SIS
	#define SDSYSTEMINFO_GPU_MATROX			5		//! Matrox
	#define SDSYSTEMINFO_GPU_XGI			6		//! XGI
	#define SDSYSTEMINFO_GPU_S3				7		//! S3/VIA
	#define SDSYSTEMINFO_GPU_3DFX			8		//! 3dfx
	#define SDSYSTEMINFO_GPU_INTEL			9		//! Intel
	#define SDSYSTEMINFO_GPU_3DLABS			10		//! 3DLabs
	#define SDSYSTEMINFO_GPU_CIRRUS			11		//! Cirrus
	#define SDSYSTEMINFO_GPU_TRIDENT		12		//! Trident


	/*!cls
	�����ṩϵͳ��Ӳ��������Ϣ���ࡣ
	!inc SDSystemInfo.h
	*/
	class ISSSystemInfo : public ISSBase
	{
	public:
		/************************************************************************************/
		//!group CPU��غ���
		/*!func
		���CPU���ơ�
		!param [out] CPU�����ƣ�����128���ַ�����
		*/
		virtual void SSAPI GetCpuName(char* strResult) = 0;

		/*!func
		���CPU��������
		!param [out] CPU�������̵����֣�����32���ֽڳ���
		!return CPU����ID����SDSYSTEMINFO_CPU_��
		*/
		virtual UINT32 SSAPI GetCpuVendor(char* str) = 0;

		/*!func
		���CPU�ͺ�
		!param [out] CPU�ͺţ�����64�ֽڳ��ȡ�
		*/
		virtual void SSAPI GetCpuModel(char* str) = 0;

		/*!func
		�ж��Ƿ�֧��MMX��
		!return CPU�Ƿ�֧�ִ����ԡ�
		*/
		virtual bool SSAPI IsSupportMMX() = 0;

		/*!func
		�ж��Ƿ�֧��SSE��
		!return CPU�Ƿ�֧�ִ����ԡ�
		*/
		virtual bool SSAPI IsSupportSSE() = 0;

		/*!func
		�ж��Ƿ�֧��SSE2��
		!return CPU�Ƿ�֧�ִ����ԡ�
		*/
		virtual bool SSAPI IsSupportSSE2() = 0;

		/*!func
		�ж��Ƿ�֧��3DNOW��
		!return CPU�Ƿ�֧�ִ����ԡ�
		*/
		virtual bool SSAPI IsSupport3DNow() = 0;

		/*!func
		�ж��Ƿ�֧��HT��
		!return CPU�Ƿ�֧�ִ����ԡ�
		*/
		virtual bool SSAPI IsSupportHT() = 0;

		/*!func
		��ע�����CPU��Ƶ����һ��׼ȷ��
		!return CPU��Ƶ����ע���õ�������Ϊ0��
		*/
		virtual UINT32 SSAPI GetCpuSpeedReg() = 0;

		/*!func
		����CPU��Ƶ��
		!return ͨ��CPU�ڲ�����������CPU����Ƶ���ڴ��������CPU����Ч�����޷�׼ȷ����֧��StepSpeed�������ƶ�CPU��Ƶ��
		*/
		virtual UINT32 SSAPI GetCpuSpeed() = 0;


		/************************************************************************************/
		//!group GPU��غ���
		/*!func
		����Կ����ơ�
		!param [out] �Կ����ƣ�����256�����ַ�����
		!return �Կ�֧�ֵ�PixelShader�汾�ţ�0x0000����0.0��0x0100����1.0��0x0101����1.1��0x0102����1.2��0x0103����1.3��0x0104����1.4��0xFFFF�����޷���ȡPS֧�����
		*/
		virtual UINT32 SSAPI GetVideoCardName(char* str) = 0;

		/*!func
		����Դ�������
		!return �Դ���������λΪM��
		*/
		virtual UINT32 SSAPI GetVideoMemory() = 0;

		/*!func
		���GPU�������ơ�
		!param [in,out] ����ֵΪ�Կ����ƣ����Ϊ�Կ����������̣�����128���ַ����ȡ�
		!param [out] �Կ���������ID����SDSYSTEMINFO_GPU_��
		!return �Կ��ȼ���ţ�1��8��ʾ�����Կ�������Խ���ٶ�Խ�죬1,2Ϊ��̭�Կ���3,4Ϊ�Ͷ��Կ���5,6Ϊ�����Կ���7,8Ϊ�߶��Կ���10��ʾΪרҵ�Կ���
		*/
		virtual UINT32 SSAPI GetVideoCardModel(char* str,INT32* pVideoCardID) = 0;


		/************************************************************************************/
		//!group ����ϵͳ��غ���
		/*!func
		���Windows�汾�š�
		!return Windows�汾�ţ�Ϊ ���汾�š�1000 + ���汾��
		*/
		virtual UINT32 SSAPI GetWindowsVersion() = 0;

		/*!func
		���ϵͳ�ڴ�������
		!param [out] ϵͳ���ڴ棬���������ڴ棬����ΪNULL��
		!return �����ڴ���
		*/
		virtual UINT32 SSAPI GetWindowsMemory(UINT32* pTotal) = 0;

		/*!func
		���DirectX�汾�š�
		DirectX�汾�Ŷ��壺
		00000000 = �޷����DirectX�汾��
		00010000 = DirectX 1.0
		00020000 = DirectX 2.0
		00030000 = DirectX 3.0
		00030001 = DirectX 3.0a
		00050000 = DirectX 5.0
		00060000 = DirectX 6.0
		00060100 = DirectX 6.1
		00060101 = DirectX 6.1a
		00070000 = DirectX 7.0
		00070001 = DirectX 7.0a
		00080000 = DirectX 8.0
		00080100 = DirectX 8.1
		00080101 = DirectX 8.1a
		00080102 = DirectX 8.1b
		00080200 = DirectX 8.2
		00090000 = DirectX 9.0
		!return ����ֵΪ10�������֣��������ϡ�
		*/
		virtual UINT32 SSAPI GetDXVersion() = 0;


		/************************************************************************************/
		//!group ��������ΨһID
		/*!func
		��ô˿ͻ��˵�Ψһ��ʶID��
		!param [out] 16���ֽڵĿͻ��˱�ʶID
		*/
		virtual void SSAPI GetMachineID(char* str) = 0;


		/************************************************************************************/
		//!group ����ϵͳ��Ϣ�ַ���
		/*!func
		����ϵͳ��Ϣ�ַ�����
		!param [out] ����Ҫ��4k�ֽڵĳ��ȣ����ϵͳ��Ϣ��
		!return �����Ƿ�ɹ���
		*/
		virtual bool SSAPI GetSystemInfo(char* strOutput) = 0;
	};


	/*!func
	���ʵ����ȡ������
	�˺�������SDSystemInfo�����ʵ����
	!inc SDSystemInfo.h
	!param [in] �û�Ҫ��������Ͱ汾�ţ���SSSCPVersion��
	!return ����ɹ������ش������ʵ�������ʧ�ܷ��ؿ�ָ�롣
	*/
	ISSSystemInfo* SSAPI SDSystemInfoGetModule(const SSSCPVersion* pstVersion);

	/*!typedef
	SDSystemInfoGetModule �������Ͷ��塣
	��SDSystemInfoGetModule��
	!inc SDSystemInfo.h
	*/
	typedef ISSSystemInfo* (SSAPI *PFN_SDSystemInfoGetModule)(const SSSCPVersion* pstVersion);

} // End of SSCP namespace section 

#endif




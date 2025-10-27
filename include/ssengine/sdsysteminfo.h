/*!
!module SDSystemInfo系统信息收集组件
!moduletype common
用来获取客户端运行环境软硬件信息的组件。
此组件的主要功能有：
.获得CPU硬件相关信息；
.获得显卡硬件相关信息；
.获得内存相关信息；
.获得操作系统相关信息；
.获得DirectX版本信息；
.生成此主机的唯一ID；
.生成提交给服务器的完整系统信息字符串；

具体使用示例见!!!link \示例\组件示例\SDSystemInfo@Demo1@系统信息收集.htm SDSystemInfo Demo1 系统信息收集!!!。

!inc SDSysteminfo.h
*/ 
#ifndef __SSCP_SDSYSTEMINFO_ISYSTEMINFO_H__
#define __SSCP_SDSYSTEMINFO_ISYSTEMINFO_H__

#include "sdbase.h"
// Compatibility: alias SSSCPVersion to SSSVersion for this SDK
typedef SSSVersion SSSCPVersion;



namespace SSCP
{

	// 模块名称
	const char SDSYSTEMINFO_MODULENAME[] = "SSSystemInfo";

	// 版本号
	const SSSCPVersion	SDSYSTEMINFO_VERSION = {0,3,0,1};


	//=============================================================================================================================
	/*!
	!macro SDSYSTEMINFO_CPU_
	CPU厂商名称。
	!inc SDSystemInfo.h
	*/
	#define SDSYSTEMINFO_CPU_OTHER			0		//! 未知厂商
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
	GPU厂商名称。
	!inc SDSystemInfo.h
	*/
	#define SDSYSTEMINFO_GPU_UNKONW			0		//! 未知厂商
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
	用来提供系统软硬件环境信息的类。
	!inc SDSystemInfo.h
	*/
	class ISSSystemInfo : public ISSBase
	{
	public:
		/************************************************************************************/
		//!group CPU相关函数
		/*!func
		获得CPU名称。
		!param [out] CPU的名称，至少128个字符长度
		*/
		virtual void SSAPI GetCpuName(char* strResult) = 0;

		/*!func
		获得CPU厂商名。
		!param [out] CPU生产厂商的名字，至少32个字节长度
		!return CPU厂商ID，见SDSYSTEMINFO_CPU_。
		*/
		virtual UINT32 SSAPI GetCpuVendor(char* str) = 0;

		/*!func
		获得CPU型号
		!param [out] CPU型号，至少64字节长度。
		*/
		virtual void SSAPI GetCpuModel(char* str) = 0;

		/*!func
		判断是否支持MMX。
		!return CPU是否支持此特性。
		*/
		virtual bool SSAPI IsSupportMMX() = 0;

		/*!func
		判断是否支持SSE。
		!return CPU是否支持此特性。
		*/
		virtual bool SSAPI IsSupportSSE() = 0;

		/*!func
		判断是否支持SSE2。
		!return CPU是否支持此特性。
		*/
		virtual bool SSAPI IsSupportSSE2() = 0;

		/*!func
		判断是否支持3DNOW。
		!return CPU是否支持此特性。
		*/
		virtual bool SSAPI IsSupport3DNow() = 0;

		/*!func
		判断是否支持HT。
		!return CPU是否支持此特性。
		*/
		virtual bool SSAPI IsSupportHT() = 0;

		/*!func
		从注册表获得CPU主频，不一定准确。
		!return CPU主频，从注册表得到，可能为0。
		*/
		virtual UINT32 SSAPI GetCpuSpeedReg() = 0;

		/*!func
		计算CPU主频。
		!return 通过CPU内部计数器计算CPU的主频，在大多数主流CPU上有效，但无法准确计算支持StepSpeed技术的移动CPU主频。
		*/
		virtual UINT32 SSAPI GetCpuSpeed() = 0;


		/************************************************************************************/
		//!group GPU相关函数
		/*!func
		获得显卡名称。
		!param [out] 显卡名称，至少256个字字符长度
		!return 显卡支持的PixelShader版本号，0x0000代表0.0，0x0100代表1.0，0x0101代表1.1，0x0102代表1.2，0x0103代表1.3，0x0104代表1.4，0xFFFF代表无法获取PS支持情况
		*/
		virtual UINT32 SSAPI GetVideoCardName(char* str) = 0;

		/*!func
		获得显存容量。
		!return 显存容量，单位为M。
		*/
		virtual UINT32 SSAPI GetVideoMemory() = 0;

		/*!func
		获得GPU厂商名称。
		!param [in,out] 输入值为显卡名称，输出为显卡的生产厂商，至少128个字符长度。
		!param [out] 显卡生产厂商ID，见SDSYSTEMINFO_GPU_。
		!return 显卡等级编号：1～8表示民用显卡，数字越大速度越快，1,2为淘汰显卡，3,4为低端显卡，5,6为主流显卡，7,8为高端显卡；10表示为专业显卡。
		*/
		virtual UINT32 SSAPI GetVideoCardModel(char* str,INT32* pVideoCardID) = 0;


		/************************************************************************************/
		//!group 操作系统相关函数
		/*!func
		获得Windows版本号。
		!return Windows版本号，为 主版本号×1000 + 附版本号
		*/
		virtual UINT32 SSAPI GetWindowsVersion() = 0;

		/*!func
		获得系统内存容量。
		!param [out] 系统总内存，包括虚拟内存，可以为NULL。
		!return 物理内存量
		*/
		virtual UINT32 SSAPI GetWindowsMemory(UINT32* pTotal) = 0;

		/*!func
		获得DirectX版本号。
		DirectX版本号定义：
		00000000 = 无法获得DirectX版本号
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
		!return 返回值为10进制数字，定义如上。
		*/
		virtual UINT32 SSAPI GetDXVersion() = 0;


		/************************************************************************************/
		//!group 生成主机唯一ID
		/*!func
		获得此客户端的唯一标识ID。
		!param [out] 16个字节的客户端标识ID
		*/
		virtual void SSAPI GetMachineID(char* str) = 0;


		/************************************************************************************/
		//!group 生成系统信息字符串
		/*!func
		生成系统信息字符串。
		!param [out] 至少要有4k字节的长度，存放系统信息。
		!return 操作是否成功。
		*/
		virtual bool SSAPI GetSystemInfo(char* strOutput) = 0;
	};


	/*!func
	组件实例获取函数。
	此函数返回SDSystemInfo组件的实例。
	!inc SDSystemInfo.h
	!param [in] 用户要求的组件最低版本号，见SSSCPVersion。
	!return 如果成功，返回此组件的实例；如果失败返回空指针。
	*/
	ISSSystemInfo* SSAPI SDSystemInfoGetModule(const SSSCPVersion* pstVersion);

	/*!typedef
	SDSystemInfoGetModule 函数类型定义。
	见SDSystemInfoGetModule。
	!inc SDSystemInfo.h
	*/
	typedef ISSSystemInfo* (SSAPI *PFN_SDSystemInfoGetModule)(const SSSCPVersion* pstVersion);

} // End of SSCP namespace section 

#endif




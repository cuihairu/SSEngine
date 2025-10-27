/******************************************************************************
			Copyright (C) YoFei Corporation. All rights reserved.

	sdtimer.h - ��ʱ�����
******************************************************************************/


#ifndef SDTIMER_H
#define SDTIMER_H
/**
* @file sdtimer.h
* @author lw
* @brief ��ʱ��ϵ��
*
**/
#include "sdtype.h"
#include <string>

namespace SSCP
{
    /**
    * @defgroup grouptimer ��ʱ�����
    * @ingroup  SSCP
    * @{
    */
    class CSDTimerImpl;

    /**
    * @brief The ISSTimer class is an application-defined timer completion
    *       routine. Specify this address when calling the SetTimer function.
    *       then it will be called when the timer was timeout.
    *       Especially this OnTimer function can't be blocked.
    */
    class ISSTimer
    {
    public:

        virtual SSAPI ~ISSTimer() {}

        /**
        * @brief
        * ��ʱ����������ʱʱ�����ô˶�ʱ����
        * @param dwTimerID : ��ʱʱ�Ķ�ʱ��IDTimer
        * @return void
        */
        virtual void SSAPI OnTimer(UINT32 dwTimerID) = 0;
    };

    /**
    * @brief ��ʱ��ģ���࣬��������LINUX�ķֲ�Ķ�ʱ����������ͨ�����̵߳���ѭ������Run������������ʱ��
    */
    class CSDTimer
    {

    public:
        CSDTimer();
        ~CSDTimer();

        /**
        * @brief
        * ����һ��ʱ�Ӳ�����time-outֵ
        * @param poTimerHandler : �û�����ʱ�ӻص��ӿ���
        * @param dwTimerId : ����ʱ��ID
        * @param dwElapse : ����time-outֵ���Ժ���Ϊ��λ
        * @param dwLoop : ÿ������Ķ�ʱ������ִ��һ�Σ�dwLoop��ʾ��ʱ����ʱ�������ʱ�Ĵ��������Ϊ0xFFFFFFFF����ʾ����ѭ����ʱ��
        * @return ����һ��boolֵ,true��ʾ�ɹ�,false��ʾʧ��
        */
        BOOL SSAPI SetTimer(ISSTimer *pTimer, UINT32 dwTimerId, UINT32 dwElapse, UINT32 dwLoop = 0xFFFFFFFF);

        /**
        * @brief
        * ȡ��ʱ��
        * @param dwTimerId : UINT32 ʱ��ID����ʾҪȡ����ʱ��
        * @return ����һ��boolֵ,true��ʾ�ɹ�,false��ʾʧ��
        */
        BOOL SSAPI KillTimer(UINT32 dwTimerId);

        /**
        * @brief
        * ʱ����Ϣ����
        * @param nCount : ��ʾ������֤�¼����������ޣ�-1��ʾ�������е��¼�
        * @return ��������¼�û�����귵��true����������¼����������ˣ�����false
        */
        BOOL SSAPI Run();

        /**
        * @brief
        * ��ӡ��ʱ����Ϣ
        * @return void
        */
		std::string SSAPI DumpTimerInfo();

    protected:
		CSDTimer(const CSDTimer & ){};
        CSDTimer& operator = (const CSDTimer & );
    private:
        CSDTimerImpl *  m_pTimerImpl = nullptr;
    };

    /** @} */
}

#endif

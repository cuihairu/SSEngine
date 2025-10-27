/******************************************************************************
Copyright (C) 2025 Cui Hairu. All rights reserved.

	sdregex.h - ������ʽ����,ʹ����boost��xpressiveͷ�ļ���,�������ӵ�boost��

******************************************************************************/
#ifndef SDREGEX_H
#define SDREGEX_H
/**
* @file sdregex.h
* @author wk
* @brief ������ʽ��
*
**/
#include "sdmacros.h"
#include "sdtype.h"
#include <string>
#include <vector>

#if SDU_WITH_BOOST
namespace SSCP
{
    typedef std::vector<std::string> SMatchResult;
    /**
    * @brief
    * ���ַ����в���ƥ����ַ��� ��  SDRegexMatch("hell world!",match,"(\\w+) (\\w+)!" );
    * @param pzStr��  ԭʼ�ַ���
    * @param SMatchResult �� ƥ���Ľ��
    * @param pattern �� ��Ҫƥ���������ʽ
    * @return �����Ƿ�ƥ��ɹ�
    */
    BOOL SDRegexMatch(const CHAR * pzStr,  SMatchResult & result, const CHAR * pattern);

    /**
    * @brief
    * ���ַ����в���ƥ����ַ��� ��  SDRegexMatch("hell world!",match,"(\\w+) (\\w+)!" );
    * @param pzStr��  ԭʼ�ַ���,��stl�ı�ʾ
    * @param SMatchResult �� ƥ���Ľ��
    * @param pattern �� ��Ҫƥ���������ʽ
    * @return �����Ƿ�ƥ��ɹ�
    */
    BOOL SDRegexMatch(const std::string & pzStr,  SMatchResult & result, const std::string&  pattern);

    /**
    * @brief
    * ͨ��������ʽ�����ַ����е��Ӵ��������ַ��������滻
    *  ��  SDRegexReplace("This is his face","his","her" ); ���Ϊ"Ther is her face";
    * @param pzStr��  ԭʼ�ַ���
    * @param pattern ��ƥ��ԭʼ���е�������ʽ
    * @param format �� ƥ�����Ҫ�滻������
    * @return �����滻����ַ���
    */
    std::string SDRegexReplace(const CHAR * pzStr , const CHAR * pattern ,const CHAR * format);

    /**
    * @brief
    * ͨ��������ʽ�����ַ����е��Ӵ��������ַ��������滻
    *  ��  SDRegexReplace("This is his face","his","her" ); ���Ϊ"Ther is her face";
    * @param pzStr��  ԭʼ�ַ���, ��stl�ı�ʾ
    * @param pattern ��ƥ��ԭʼ���е�������ʽ
    * @param format �� ƥ�����Ҫ�滻������
    * @return �����滻����ַ���
    */
    std::string SDRegexReplace(const std::string &  pzStr , const std::string &  pattern ,const std::string &  format);
}

#endif // SDU_WITH_BOOST


#endif // 


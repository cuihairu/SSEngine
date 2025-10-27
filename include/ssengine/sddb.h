/******************************************************************************
Copyright (C) 2025 Cui Hairu. All rights reserved.

 sddb.h  -  Database operation classes, macros, types and definitions
            Include file for sddb.dll. This library is designed to support
            database operations.

 Class   -  ISSDBCommand
            ISSDBModule
            ISSDBSession
            ISSDBRecordSet
			ISSDBConn

 Remarks -  ISSDBModule can connect to different databases at the same time and
            process the database operation request(ISSDBCommand) asynchronously.
            Users can execute more than one SQL statement in a database request
            And particular informations can be retrieved when error occurs.

******************************************************************************/

#ifndef SSCP_SDDB_H_20100728
#define SSCP_SDDB_H_20100728

#include "sdbase.h"
#include "sddb_ver.h"
#include "sdlogger.h"

/**
* @brief ���ݿ���Ϣ���Ƶ���󳤶�
*/
#define SDDB_MAX_NAME_SIZE		        64

/**
* @brief ���ݿ��ַ������Ƶ���󳤶�
*/
#define SDDB_MAX_CHARACTSET_SIZE		32

namespace SSCP{
	/**
	* @defgroup groupsddb ���ݿ���ʲ�
	* @ingroup  SSCP
	* @{
	*/

/**
* @brief ���ݿ�ģ�������
*/
const CHAR SDDB_MODULENAME[]        = "SSDB";

/**
* @brief SQLִ�н���ķ���ֵ
*/
enum ESDDBCode
{
    SDDB_ERR_UNKNOWN    = -999, // δ֪����
    SDDB_ERR_CONN       = -2,   // ���Ӵ���
    SDDB_DISCONNECT     = -1,   // �Ͽ��������������
    SDDB_SUCCESS        = 0,    // �����ɹ�
    SDDB_NO_RECORDSET   = 1,    // �����ɹ�����û�н����
    SDDB_HAS_RECORDSET  = 2     // �����ɹ����н����
};

/**
* @brief ���ݿ��������½��Ϣ�ṹ��
*/
typedef struct tagSDDBAccount
{
    CHAR m_szHostName[SDDB_MAX_NAME_SIZE];        // ���ݿ������IP��ַ
    CHAR m_szDBName[SDDB_MAX_NAME_SIZE];          // �����ӵ����ݿ�����
    CHAR m_szLoginName[SDDB_MAX_NAME_SIZE];       // ��½�û���
    CHAR m_szLoginPwd[SDDB_MAX_NAME_SIZE];        // ��½����
	CHAR m_szCharactSet[SDDB_MAX_CHARACTSET_SIZE];// ���ݿ���뼯
    UINT16 m_wConnPort;					          // ���ݿ�����˿�
	INT32 m_wDBType;						      // SDDBDataBaseType���ݿ�����
} SDDBAccount, * PSDDBAccount;

/**
* @brief ���ݿ�����
*/
enum SDDBDataBaseType
{
	SDDB_DBTYPE_MOCK = -1, //ģ������ݿ�
	SDDB_DBTYPE_MYSQL = 0,  //MySQL���ݿ�
	SDDB_DBTYPE_ODBC = 1,  //ODBC���ݿ�
	SDDB_DBTYPE_ADO = 2  //ADO���ݿ�
};

/**
*  @brief SQL������ӿ��࣬SQL������Ľӿڣ��ṩ������������������
*/
class ISSDBRecordSet
{
public:
	virtual SSAPI ~ISSDBRecordSet() {};

	/**
	* @brief ��ȡ���������
	*
	* @return ��ȡ���������
	*/
	virtual UINT32 SSAPI GetRecordCount(void) = 0;
    
	/**
	* @brief ��ȡÿ��������������
	*
	* @return ��ȡÿ��������������
	*/
    virtual UINT32 SSAPI GetFieldCount(void) = 0;
    
	/**
	* @brief ��ȡ��һ�����
	*
	* @return �Ƿ�����һ���������Ϊtrue��û��Ϊfalse
	*/
    virtual bool SSAPI GetRecord(void) = 0;
    
	/**
	* @brief ��ȡ��ǰ������ض�index�����ֵ
	*
	* @param dwIndex ���Index
	* @return ����и�Index���򣬷������ֵ��
	* ���û�и�Index���򣬷���NULL��
	*/
    virtual const CHAR * SSAPI GetFieldValue(UINT32 dwIndex) = 0;
    
	/**
	* @brief ��ȡ��ǰ������ض�index����ĳ���
	*
	* @param dwIndex ���Index
	* @return ����и�Index���򣬷�����ĳ��ȣ�
	* ���û�и�Index���򣬷���0
	*/
    virtual INT32 SSAPI GetFieldLength(UINT32 dwIndex) = 0;
    
	/**
	* @brief �ͷŵ��˽����
	*/
    virtual void SSAPI Release(void)=0;

	/**
	* @brief ��ȡ��ǰ������ض����������ֵ
	*
	* @param pszFieldName �������
	* @return ����ҵ����򣬷��ظ����ֵ��
	* �������NULL������ý��ΪNULL������������
	*/
	virtual const CHAR* SSAPI GetFieldValueByName(const CHAR *pszFieldName) = 0;

	/**
	* @brief ��ȡ��ǰ������ض���������ĳ���
	*
	* @param pszFieldName �������
	* @return ����ҵ����򣬷��ظ���ĳ��ȣ�
	* �������0������ý��ΪNULL������������
	*/
	virtual INT32 SSAPI GetFieldLengthByName(const CHAR *pszFieldName) = 0;
};

/**
* @brief DBConnection�ӿ��࣬�������ݿ�����ӣ�ֻ���첽�ص���ʹ�á�
* ע�⣺���������еĺ����������ǡ��̰߳�ȫ��
*/
class ISSDBConnection
{
public:
	virtual SSAPI ~ISSDBConnection() {};

	/**
	* @brief ��������Ƿ�����
	*
	* @return ��������Ƿ�������trueΪ������falseΪ������
	*/
	virtual bool SSAPI CheckConnection() = 0;

	/**
	* @brief ���������ַ���ת��Ϊ���ݿ��ʶ�����0��β���ַ���
	*
	* @param pSrc Դ�ַ����ĵ�ַ
	* @param nSrcSize Դ�ַ����ĳ���
	* @param pDest Ŀ���ַ����ĵ�ַ���õ�ַ�ռ��ɵ������ṩ��Ŀ���ַ��������ݽ�д�뵽�õ�ַ�ռ���
	* @param nDstSize Ŀ���ַ�������󳤶ȣ�һ��Ӧ�ö���Ϊ2��nSrcSize �� 1
	* @return ת������ַ����ĳ��ȣ�ת��ʧ�ܷ���0
	*/
    virtual UINT32 SSAPI EscapeString(const CHAR *pSrc, INT32 nSrcSize, 
                                      CHAR *pDest,    INT32 nDstSize) = 0;
	/**
	* @brief ִ���޷���ֵ��SQL��䣬���Ϊ����������ɴ�pInsertId��ȡ�����е�ID��
	*
	* @param pSQL ��Ҫִ�е�SQL����ַ
	* @param pInsertId ���Ϊ����SQL��䣬����ͨ��pInsertId��ȡ�������ݵ��������õ�ַ�ռ��ɵ������ṩ
	*                  ע�⣺MS SQL Server�ݲ�֧�ִ˲�����������ΪNULL
	* @return ִ��SQL���Ľ��
	* SDDB_ERR_CONN ���Ӵ���
	* SDDB_ERR_UNKNOWN δ֪���ݿ����
	* ���� Ӱ�������
	*/
	virtual INT32 SSAPI ExecuteSql(const CHAR *pSQL, UINT64 *pInsertId = NULL) = 0;

	/**
	* @brief ִ���з��ؽ����SQL���
	*
	* @param pSQL ��Ҫִ�е�SQL���
	* @param ppoRs ���صĽ�����ϣ��õ�ַ�ռ���SDDB�ṩ��
	* @return ִ��SQL���Ľ��
	* SDDB_ERR_CONN ���Ӵ���
	* SDDB_ERR_UNKNOWN δ֪����
	* SDDB_NO_RECORDSET ִ�гɹ���û�н������
	* SDDB_HAS_RECORDSET ִ�гɹ����н�����ϣ�ע�⣬��ɲ�������Ҫ(*ppoRs)->Release();
	*/
	virtual INT32 SSAPI ExecuteSqlRs(const CHAR *pSQL, SSCP::ISSDBRecordSet **ppoRs) = 0;

	/**
	* @brief ����һ������
	*/
	virtual void SSAPI BeginTransaction() = 0;

	/**
	* @brief ���һ������
	*/
	virtual void SSAPI CommitTransaction() = 0;

	/**
	* @brief �ع�һ������
	*/
	virtual void SSAPI RollbackTransaction() = 0;

	/**
	* @brief ����DB
	*
	* @param pcDBName ��Ҫ������DBName
	* @param bForce �Ƿ�ǿ��ִ��
	* @param pcCharSet DB�ı��뼯
	* @return ����DB�Ƿ�ɹ���true����ɹ���false����ʧ�� 
	*/
	virtual bool SSAPI CreateDB(const CHAR *pcDBName, bool bForce, const CHAR *pcCharSet) = 0;

	/**
	* @brief ѡ�������DB
	*
	* @param pcDBName ѡ�������DBName
	* @return ѡ��DB�Ƿ�ɹ���true����ɹ���false����ʧ��
	*/
	virtual bool SSAPI SelectDB(const CHAR *pcDBName) = 0;

	/**
	* @brief �ͷ�ISSDBConn
	*/
	virtual void SSAPI Release() = 0;
};

/**
* @brief DBCommand�ӿ��࣬�����첽ִ��SQL��DB�����������Ϊ��
* ��������->OnExecuteSql(�첽)->(ִ��ISSDBSession::Runʱ)OnExecuted(ͬ��)->Release(ͬ��)
*/
class ISSDBCommand 
{
public:
	virtual SSAPI ~ISSDBCommand() {};

	/**
	* @brief ������ķ���ID��
	* ��Ҫע����ǣ��첽ִ�е�SQL��������ͬһ��Group�У����Ǳ�֤����������ʱ��ʱ�����ִ��SQL����
	* �������Ϊ��1�����ǲ���֤����������ʱ��ʱ�����ִ��SQL����
	* ʹ��QuickAddDBCommand������ͬ������ִ֤��SQL�����ʱ��˳��
	*/
	virtual int SSAPI GetGroupId() {return -1;};

	/**
	* @brief ִ��SQL����
	*
	* @param poDBConn ִ���������õ�����
	* ע�⣺�����ӳ����ж�����ӵ�����£��˺��������ǡ��̰߳�ȫ�ġ�
	*/
	virtual void SSAPI OnExecuteSql(ISSDBConnection *poDBConnection) = 0;

	/**
	* @brief ִ�����SQL��������û��߳���ִ��ISSDBSession::Runʱ��
	* ִ�д˺����е�ҵ���߼������û����ԣ��˺����е�ִ����ͬ���ġ�
	* ע�⣺������һ��Session��ִ�е�����˺����ڴ�Session::Run()��˳�����У�
	* Ҳ����˵�����ȫ��ֻ��һ���ط�ִ��Session::Run()���򣬴˺������̰߳�ȫ�ġ�
	*/
    virtual void SSAPI OnExecuted(void) = 0;
    
	/**
	* @brief ִ�����SQL��������û��߳���ִ��ISSDBSession::Runʱ��
	* ִ����ҵ���߼���ִ�д�Release���������û����ԣ��˺����е�ִ����ͬ����
	* �����������£��Ƽ�����Ϊ��delete this;
	* ע�⣺������һ��Session��ִ�е�����˺����ڴ�Session::Run()��˳�����У�
	* Ҳ����˵�����ȫ��ֻ��һ���ط�ִ��Session::Run()���򣬴˺������̰߳�ȫ�ġ�
	*/
    virtual void SSAPI Release(void) = 0;
};

/**
* @brief DBSession�ӿ��࣬������һ�����ݿ���������ӵĻỰ���ûỰҲ����ά���������
*/
class ISSDBSession
{
public:
	virtual SSAPI ~ISSDBSession() {};

	/**
	* @brief ���������ַ���ת��Ϊ���ݿ��ʶ�����0��β���ַ���
	*
	* @param pSrc Դ�ַ����ĵ�ַ
	* @param nSrcSize Դ�ַ����ĳ���
	* @param pDest Ŀ���ַ����ĵ�ַ���õ�ַ�ռ��ɵ������ṩ��Ŀ���ַ��������ݽ�д�뵽�õ�ַ�ռ���
	* @param nDstSize Ŀ���ַ�������󳤶ȣ�һ��Ӧ�ö���Ϊ2��nSrcSize �� 1
	* @param timeout ִ�еĳ�ʱʱ�䣬����Ϊ����ʱ��
	* @return ת������ַ����ĳ��ȣ�ת��ʧ�ܷ���0
	*/
    virtual UINT32 SSAPI EscapeString(const CHAR *pSrc, INT32 nSrcSize, 
                                      CHAR *pDest,    INT32 nDstSize, INT32 timeout = -1) = 0;

	/**
	* @brief ִ���޷���ֵ��SQL��䣬���Ϊ����������ɴ�pInsertId��ȡ�����е�ID��
	*
	* @param pSQL ��Ҫִ�е�SQL����ַ
	* @param pInsertId ���Ϊ����SQL��䣬����ͨ��pInsertId��ȡ�������ݵ��������õ�ַ�ռ��ɵ������ṩ
	* @param timeout ִ�еĳ�ʱʱ�䣬����Ϊ����ʱ��
	* @return ִ��SQL���Ľ��
	* SDDB_ERR_CONN ���Ӵ���
	* SDDB_ERR_UNKNOWN δ֪���ݿ����
	* ���� Ӱ�������
	*/
	virtual INT32 SSAPI ExecuteSql(const CHAR *pSQL, UINT64 *pInsertId = NULL, INT32 timeout = -1) = 0;

	/**
	* @brief ִ���з��ؽ����SQL���
	*
	* @param pSQL ��Ҫִ�е�SQL���
	* @param ppoRs ���صĽ�����ϣ��õ�ַ�ռ���SDDB�ṩ��
	* @param timeout ִ�еĳ�ʱʱ�䣬����Ϊ����ʱ��
	* @return ִ��SQL���Ľ��
	* SDDB_ERR_CONN ���Ӵ���
	* SDDB_ERR_UNKNOWN δ֪����
	* SDDB_NO_RECORDSET ִ�гɹ���û�н������
	* SDDB_HAS_RECORDSET ִ�гɹ����н�����ϣ�ע�⣬��ɲ�������Ҫ(*ppoRs)->Release();
	*/
    virtual INT32 SSAPI ExecuteSqlRs(const CHAR *pSQL, ISSDBRecordSet **ppoRs, INT32 timeout = -1) = 0;

	/**
	* @brief ����DB
	*
	* @param pcDBName ��Ҫ������DBName
	* @param bForce �Ƿ�ǿ��ִ��
	* @param pcCharSet DB�ı��뼯
	* @param timeout ִ�еĳ�ʱʱ�䣬����Ϊ����ʱ��
	* @return ����DB�Ƿ�ɹ���true����ɹ���false����ʧ�� 
	*/
	virtual bool SSAPI CreateDB(const CHAR *pcDBName, bool bForce, const CHAR *pcCharSet, INT32 timeout = -1) = 0;

	/**
	* @brief ѡ�������DB
	*
	* @param pcDBName ѡ�������DBName
	* @param timeout ִ�еĳ�ʱʱ�䣬����Ϊ����ʱ��
	* @return ѡ��DB�Ƿ�ɹ���true����ɹ���false����ʧ��
	*/
	virtual bool SSAPI SelectDB(const CHAR *pcDBName, INT32 timeout = -1) = 0;

	/**
	* @brief ���DB����
	*
	* @param poDBCommand ��ӵ�DB����
	* @return �Ƿ���ӳɹ���true������ɹ���false����ʧ��
	*/
	virtual bool SSAPI AddDBCommand(ISSDBCommand *poDBCommand) = 0;

	/**
	* @brief �������DB����
	*
	* @param poDBCommand ������ӵ�DB����
	* @return �Ƿ���ӳɹ���true������ɹ���false����ʧ��
	*/
    virtual bool SSAPI QuickAddDBCommand(ISSDBCommand *poDBCommand) = 0;

	/**
	* @brief ִ����ɵ�DB������߼����룬����DBCommand��OnExecuted(ͬ��)->Release(ͬ��)����
	*
	* @param nCount ��Ҫִ��DB������߼����������
	* ��������ִ�е�ǰ���е��Ѿ����SQL��䣬��δִ������߼�������DB����
	* @return �Ƿ�ִ�����nCount������SQL����
	*/
	virtual bool SSAPI Run(INT32 nCount = -1) = 0;

	/**
	* @brief ��ǰִ�����SQL��䣬��δִ������߼�������DB���������
	*
	* @return ���ص�ǰִ�����SQL��䣬��δִ������߼�������DB���������
	*/
	virtual UINT32 SSAPI GetDBCommandCount() = 0;
};

/**
* @brief DBģ����ӿڣ����ڴ����͹ر�DB�Ự
*/
class ISSDBModule : public ISSBase
{
public:
	/**
	* @brief ����ConfigString��Ϣ����DBSession.
	* ʾ��: HostName=IPAddress;LoginName=YourLoginName;LoginPwd=Password;DBName=YourDBName;CharacterSet=latin1;CoreSize=5;MaxSize=10;DBType=0;Port=3306;
	* ����,ȱʡDBTypeΪ0,��MySQL(Ϊ3����MS SQL);MySQL��ȱʡ�˿ں�Ϊ3306,MS SQL��ȱʡ�˿ں�Ϊ1433.
	*
	* @param pszConfigString ���ݿ�������Ϣ
	* @return ������DBSession���������ʧ�ܣ�����NULL
	*/
	virtual ISSDBSession * SSAPI GetDBSession(const CHAR * pszConfigString) = 0;

	/**
	* @brief ����DBAccount��Ϣ����DBSession�����DBSession��ֻ�е���DBConnection����
	*
	* @param pstDBAccount ���ݿ�������Ϣ
	* @return ������DBSession���������ʧ�ܣ�����NULL
	*/
	virtual ISSDBSession * SSAPI GetDBSession(SDDBAccount *pstDBAccount) = 0;

	/**
	* @brief ������DBSession
	*
	* @param pstDBAccount ���ݿ�������Ϣ
	* @param coreSize �������ģ���С��������
	* @param maxSize �������������
	* @return ������DBSession���������ʧ�ܣ�����NULL
	*/
	virtual ISSDBSession * SSAPI GetDBSession(SDDBAccount *pstDBAccount, UINT32 coreSize, UINT32 maxSize) = 0;

	/**
	* @brief �ر�һ���Ự�����б�DBModule������DBSession����Ҫ�رջỰ
	*
	* @param pDBSession һ����DBModule������DBSession
	*/
	virtual void SSAPI Close(ISSDBSession * pDBSession) = 0; 
};

/**
* @brief ���ݰ汾�Ŵ�����Ӧ��DBModule
*/
ISSDBModule* SSAPI SSDBGetModule(const SSSVersion *pstVersion);

/**
* @brief �����ָ��SSDBGetModule��ָ��
*/
typedef ISSDBModule* (SSAPI *PFN_SSDBGetModule)(const SSSVersion *pstVersion);

/**
* @brief ����DBModule�ڲ�ʹ�õ�Logger����־����
*
* @param poLogger ʹ�õ�Logger
* @param dwLevel ��־����
* @return �����Ƿ�ɹ�������ɹ�����true��ʧ�ܷ���false
*/
bool SSAPI SSDBSetLogger(ISSLogger *poLogger, UINT32 dwLevel);

/**
* @brief �����ָ��SSDBSetLogger��ָ��
*/
typedef bool (SSAPI * PFN_SSDBSetLogger)(ISSLogger *poLogger, UINT32 dwLevel);

}// namespace SSCP

#endif

#ifndef __DLP_DERESERCH_H__
#define __DLP_DERESERCH_H__

class CDeReSearch
{

public:

	virtual ~CDeReSearch();
	static CDeReSearch * GetInstance() ;
	int InitSearch();
	void UpdatePolicyVersion( char * pchVersion );
	char * GetPolicyVersion( );
	char * GetFileIncludeName( char * pchFID );
	void clear();
	int InsertFID( char * pchFID,int iLen ,char * pchFileContent ,int iContentLen , char * pchIncludeFileName );
	int InsertResult( char * pchFID,int iLen ,char * conditionID, int iconLen ,char * pchResult ,int iResultLen );
	char * GetResult( char * pchFID,int iLen ,char * conditionID, int iconLen , int & iResultLen);
	char * GetFileContent( char * pchFID,int & iResultLen);
	void DeleteOldData();
	void UpdateFID( char * pchFID );
	void UpdateIncludeNameByFID( char * pchFID ,char * pchIncludeFileName);
	int isExsit( char * pchFID );
	int isInit();
	int isEnable();
	int GetDataRecordCount();
	int GetMaxAcCount();
	char * GetMd5( char * pchBuffer ,int iLen );

private:
	CDeReSearch();
	static CDeReSearch * m_instace ;
	int m_init;
	int m_iMaxCount ;
	void * m_pDB ; 
	int m_iEnable ;
};




#endif

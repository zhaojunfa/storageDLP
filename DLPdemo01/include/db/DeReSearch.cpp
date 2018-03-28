#include "DeReSearch.h"
#include <stdlib.h>
#include "sqlite3.h"
#include <memory.h>
#include <stdio.h>
#include <string>
#include "dlp_sys_conf.h"
#include <iostream>
#include "hash.h"
#include "stdinc.h"
using namespace std ;
CDeReSearch * CDeReSearch::m_instace = NULL  ;

CDeReSearch::CDeReSearch()
{

	m_init = 0 ;
	m_iMaxCount = 0 ;
	m_pDB = NULL;
	m_iEnable = 0 ;
}

CDeReSearch::~CDeReSearch()
{
	if( m_pDB!=NULL )
	{
		sqlite3 * pDB = ( sqlite3 * ) m_pDB ;

		sqlite3_close(pDB);


		m_pDB = NULL;
	}
}

CDeReSearch * CDeReSearch::GetInstance()
{
	if( NULL == m_instace )
	{

		m_instace = new CDeReSearch ;
		
	}

	return m_instace ;
}
int CDeReSearch::isEnable()
{
	return m_iEnable;
}
int CDeReSearch::InitSearch()
{



#ifdef _WINDOWS
	char * chConfig = NULL;

	
	char  fpdir[256];

	memset(fpdir , 0 , 256 );
	chConfig = get_sys_conf("DATA_PATH");	
	if(chConfig != NULL)
	{
		memcpy( fpdir , chConfig,strlen( chConfig ));
		free( chConfig );
	}
	else
	{
		return 0;
	}
#else

	char * fpdir =  getenv("DATA_PATH");

	if(fpdir == NULL){
		return 1;
	}
#endif

	char path[0x400] = {0};


	memset( path , 0 ,0x400);

#if defined(_WINDOWS) || defined(WIN32)
	sprintf(path, "%s\\%s", fpdir, "detectResult\\DetectResult.db" );
#else
	sprintf(path, "%s/%s", fpdir, "detectResult/DetectResult.db" );
#endif

	sqlite3 * pDB = NULL ; 

	int nRes = sqlite3_open(path, &pDB);  
    if (nRes != SQLITE_OK)  
    {  

		return nRes ;
    } 
	else
	{
		char* pFPDB = get_sys_conf("AccelerateDetect");	

		int iEnable = 0 ;

		if( pFPDB != NULL )
		{
			iEnable = atoi(pFPDB);
			free( pFPDB );
			pFPDB = NULL ;
		}
		else
		{
			iEnable = 0 ;
		}

		m_iEnable = iEnable;
		if( 1==iEnable )
		{
			m_pDB = pDB ;

			m_init = 1 ;

			pFPDB = get_sys_conf("MaxDataResultCount");	

			int iMax = 0 ;

			if( pFPDB != NULL )
			{
				iMax = atoi(pFPDB);
				free( pFPDB );
			}
			else
			{
				iMax = 10000 ;
			}

			m_iMaxCount = iMax;
		}


	}

	return 0;

}

int CDeReSearch::GetMaxAcCount()
{
	return m_iMaxCount;
}

char * CDeReSearch::GetMd5( char * pchBuffer ,int iLen )
{
	char * pMd5 = NULL;

	if( NULL==pchBuffer || iLen <=0 )
	{
		return pMd5;
	}

	MD5 md5;
	//md5code_t code = {0};
	md5.Update((byte *)pchBuffer,(uint32)iLen);
	byte md5Byte[32];
	memset( md5Byte ,0,32);
	md5.Final(md5Byte);
	pMd5 = new char [64];
	memset( pMd5 , 0, 64);
	for(int i = 0;i < 16;i++){
		sprintf(pMd5 + strlen(pMd5),"%x",md5Byte[i]);
	}


	return pMd5;


}

void CDeReSearch::UpdatePolicyVersion( char * pchVersion )
{
	if( NULL==pchVersion )
	{
		return ;
	}

	if( 0==isInit() )
	{
		return ;
	}


	sqlite3 * pDB = ( sqlite3 * )m_pDB ;


	string strSql = "";  
    strSql += "update PolicyVersion set version = '";  
    strSql += pchVersion; 
	strSql += "';";
 
  
    char* cErrMsg = NULL;  
    int nRes = sqlite3_exec(pDB , strSql.c_str() ,0 ,0, &cErrMsg);

	 if (nRes != SQLITE_OK)    
    {  
        //cout<<"rpc update PolicyVersion fail: "<<cErrMsg<<endl;  
        return ;  
    } 

}


static int PolicyResult(void * mypara, int argc, char **argv, char **azColName)  
{  

   memcpy( mypara,argv[0], strlen(argv[0]) );  
    
   return 0;  
} 


static int ParseIncludeFileName(void * mypara, int argc, char **argv, char **azColName)  
{  
	
	if( strlen(argv[0]) < 2048 )
	{
		memcpy( mypara,argv[0], strlen(argv[0]) ); 
	}
    
    
   return 0;  
} 

static int Exsit(void * mypara, int argc, char **argv, char **azColName)  
{  

	if( argc >0  )
	{
		*( int * )mypara = 1 ;
	}
   
    
   return 0;  
} 

char * CDeReSearch::GetFileIncludeName( char * pchFID )
{
	if( NULL==pchFID )
	{
		return 0 ;
	}
	int iRet = 0 ;
	sqlite3 * pDB = ( sqlite3 * )m_pDB ;

	string strSql = "";  
    strSql += "select includeFileName from fileID where fileID  = '";  
	strSql += pchFID;
	strSql += "' ;";

 
  
    char* cErrMsg = NULL;
	char * pchIncludeFileName = new char [2048] ;
	memset( pchIncludeFileName, 0 ,2048 );
    int nRes = sqlite3_exec(pDB , strSql.c_str() ,ParseIncludeFileName ,pchIncludeFileName, &cErrMsg);

	if (nRes != SQLITE_OK)    
    {  
        //cout<<"rpc select PolicyVersion fail: "<<cErrMsg<<endl;  
        return NULL;  
    }



	return pchIncludeFileName;

}
char * CDeReSearch::GetPolicyVersion( )
{
	if( 0==isInit() )
	{
		return 0 ;
	}

	sqlite3 * pDB = ( sqlite3 * )m_pDB ;

	string strSql = "";  
    strSql += "select version from PolicyVersion;";  

 
  
    char* cErrMsg = NULL;
	char * chVersion = new char [128] ;
	memset(chVersion, 0 ,128);
    int nRes = sqlite3_exec(pDB , strSql.c_str() ,PolicyResult ,chVersion, &cErrMsg);

	if (nRes != SQLITE_OK)    
    {  
        //cout<<"rpc select PolicyVersion fail: "<<cErrMsg<<endl;  
        return NULL;  
    }

	//cout<<" chVersion = "<<chVersion<<endl;

	return chVersion;

}

static int Getcount(void * mypara, int argc, char **argv, char **azColName)  
{  

	if( argc >0  )
	{
	*( int * )mypara = atoi( argv[0] ) ;
	}
   
    
   return 0;  
}

int CDeReSearch::GetDataRecordCount()
{
	if( 0==isInit() )
	{
		return 0 ;
	}

	int iCount = 0 ;
	sqlite3 * pDB = ( sqlite3 * )m_pDB ;

	string strSql = "";  
    strSql += "select count(*) from fileID;";  
  
  
    char* cErrMsg;  
    int nRes = sqlite3_exec(pDB , strSql.c_str() ,Getcount ,&iCount, &cErrMsg);  
    if (nRes != SQLITE_OK)    
    {  
        //cout<<"add fileID fail: "<<cErrMsg<<endl;  
        return 0;  
    }  

	return iCount ;


}
void CDeReSearch::clear()
{
	if( 0==isInit() )
	{
		return ;
	}
		sqlite3 * pDB = ( sqlite3 * )m_pDB ;
			

		int nRes = 0;

	//select fileID from ( select *  from fileid order by findtime limit 90 ) order by usereferences   limit 10;

		string strSql = "";
		strSql = "delete from fileid ";

		char* cErrMsg;  
		nRes = sqlite3_exec(pDB , strSql.c_str() ,0 ,0, &cErrMsg);

		strSql.clear();
		strSql = "delete from detectResult ";
		nRes = sqlite3_exec(pDB , strSql.c_str() ,0 ,0, &cErrMsg);
}

int CDeReSearch::isExsit( char * pchFID )
{
	if( NULL==pchFID )
	{
		return 0 ;
	}
	int iRet = 0 ;
	sqlite3 * pDB = ( sqlite3 * )m_pDB ;

	string strSql = "";  
    strSql += "select fileID from fileID where fileID  = '";  
	strSql += pchFID;
	strSql += "' ;";

 
  
    char* cErrMsg = NULL;
	
    int nRes = sqlite3_exec(pDB , strSql.c_str() ,Exsit ,&iRet, &cErrMsg);

	if (nRes != SQLITE_OK)    
    {  
        //cout<<"rpc select PolicyVersion fail: "<<cErrMsg<<endl;  
        return iRet;  
    }



	return iRet;
}
int CDeReSearch::InsertFID( char * pchFID,int iLen ,char * pchFileContent ,int iContentLen , char * pchIncludeFileName  )
{
	if( NULL==pchFID || iLen <= 0 )
	{
		return 0 ;
	}

	if( 0==isInit() )
	{
		return 0 ;
	}

	if( isExsit(pchFID) )
	{
		return 0;
	}
	sqlite3_stmt * stat = NULL;  //预编译使用到的一个很重要的数据结构
	sqlite3 * pDB = ( sqlite3 * )m_pDB ;

	string strSql = "";  
    strSql += "insert into fileID(fileID,filecontent,includeFileName)";  
    strSql += "values('";  
    strSql += pchFID;    
    strSql += "',?,'";
	strSql += pchIncludeFileName; 
	strSql += "');";
  
	int nRes = sqlite3_prepare(pDB,strSql.c_str(),-1,&stat,0);  //预编译

	nRes = sqlite3_bind_blob(stat,1,pchFileContent,iContentLen*2,NULL);   //绑定blob类型
    nRes = sqlite3_step(stat);                              //执行sql语句，这样就把数据存到数据库里了

	//char* cErrMsg = NULL;  
    //nRes = sqlite3_exec(pDB , strSql.c_str() ,0 ,0, &cErrMsg);  
    if (nRes != SQLITE_DONE)    
    {  
        
        return 1;  
    }
    //char* cErrMsg;  
    //int nRes = sqlite3_exec(pDB , strSql.c_str() ,0 ,0, &cErrMsg);  
    //if (nRes != SQLITE_OK)    
    //{  
        //cout<<"add fileID fail: "<<cErrMsg<<endl;  
        //return false;  
    //}  


	return 0 ;
  

}
int CDeReSearch::InsertResult( char * pchFID,int iLen ,char * conditionID, int iconLen ,char * pchResult ,int iResultLen )
{
	if( NULL==pchFID || iLen <= 0 || NULL== conditionID || iconLen <=0 || NULL==pchResult || iResultLen <=0 )
	{
		return 0 ;
	}

	if( 0==isInit() )
	{
		return 0 ;
	}
	sqlite3_stmt * stat = NULL;  //预编译使用到的一个很重要的数据结构
	sqlite3 * pDB = ( sqlite3 * )m_pDB ;
	string strSql = "";
	strSql = "insert into detectResult(fileID,conditionID,detectResult) values('";
	strSql += pchFID;
	strSql += 	"','";
	strSql += conditionID;
	strSql += "',?);";
	strSql += "?);";

	int nRes = sqlite3_prepare(pDB,strSql.c_str(),-1,&stat,0);  //预编译

	nRes = sqlite3_bind_blob(stat,1,pchResult,iResultLen,NULL);   //绑定blob类型
    nRes = sqlite3_step(stat);                              //执行sql语句，这样就把数据存到数据库里了

	//char* cErrMsg = NULL;  
    //nRes = sqlite3_exec(pDB , strSql.c_str() ,0 ,0, &cErrMsg);  
    if (nRes != SQLITE_DONE)    
    {  
        
        return 1;  
    }  


	return 0 ;

}

char *  CDeReSearch::GetFileContent( char * pchFID,int & iResultLen)
{
	if( NULL==pchFID )
	{
		return NULL ;
	}

		if( 0==isInit() )
	{
		return NULL ;
	}
	sqlite3_stmt * stat = NULL;  //预编译使用到的一个很重要的数据结构
	sqlite3 * pDB = ( sqlite3 * )m_pDB ;
	int nRes = 0;

	string strSql = "";
	strSql = "select filecontent from fileID where ";
	strSql += "fileID = '";
	strSql += pchFID;
	strSql += "'";
	strSql += ";";


	nRes = sqlite3_prepare(pDB, strSql.c_str() , -1, &stat, 0);
	//nRes = sqlite3_prepare(pDB, "select * from detectResult;" , -1, &stat, 0);
	
    nRes = sqlite3_step(stat);
//得到纪录中的BLOB字段
	const void * test = sqlite3_column_blob(stat,0);
//得到字段中数据的长度
    iResultLen = sqlite3_column_bytes(stat,0);
   
    //拷贝该字段
    char * pchResult = NULL;
	if( iResultLen > 0)
	{
		pchResult = ( char *) malloc ( iResultLen + 10) ;
		memset( pchResult , 0 ,iResultLen + 10 );
		memcpy(pchResult ,test , iResultLen );
	}


	return pchResult ;
}


char * CDeReSearch::GetResult( char * pchFID,int iLen ,char * conditionID, int iconLen , int & iResultLen)
{
	if( NULL==pchFID || iLen <= 0 || NULL== conditionID || iconLen <=0  )
	{
		return NULL ;
	}

	if( 0==isInit() )
	{
		return NULL ;
	}
	sqlite3_stmt * stat = NULL;  //预编译使用到的一个很重要的数据结构
	sqlite3 * pDB = ( sqlite3 * )m_pDB ;
	int nRes = 0;

	string strSql = "";
	strSql = "select detectResult from detectResult where ";
	strSql += "fileID = '";
	strSql += pchFID;
	strSql += "'";
	strSql += " and conditionID = '";
	strSql += conditionID;
	strSql += "'";
	strSql += ";";


	nRes = sqlite3_prepare(pDB, strSql.c_str() , -1, &stat, 0);
	//nRes = sqlite3_prepare(pDB, "select * from detectResult;" , -1, &stat, 0);
	
    nRes = sqlite3_step(stat);
//得到纪录中的BLOB字段
	const void * test = sqlite3_column_blob(stat,0);
//得到字段中数据的长度
    iResultLen = sqlite3_column_bytes(stat,0);
   
    //拷贝该字段
    char * pchResult = NULL;
	if( iResultLen > 0)
	{
		pchResult = new char[iResultLen + 10];
		memset( pchResult , 0 ,iResultLen + 10 );
		memcpy(pchResult ,test , iResultLen );
	}


	return pchResult ;


}

static int GetFileID(void * mypara, int argc, char **argv, char **azColName)  
{  

	if( argc >0  )
	{
		*( int * )mypara = 1 ;
	}
   
    
   return 0;  
}


static int deleteResult(void * mypara, int argc, char **argv, char **azColName)  
{  

	for( int i =0 ; i< argc ;i++ )
	{
		cout<<argv[i]<<endl;

		sqlite3 * pDB = ( sqlite3 * )mypara ;

		int nRes = 0;

	//select fileID from ( select *  from fileid order by findtime limit 90 ) order by usereferences   limit 10;

		string strSql = "";
		strSql = "delete from fileid where fileid =  '";
		strSql += argv[i];
		strSql += "';";




		char* cErrMsg;  
		nRes = sqlite3_exec(pDB , strSql.c_str() ,0 ,0, &cErrMsg);

		strSql.clear();
		strSql = "delete from detectResult where fileid =  '";
		strSql += argv[i];
		strSql += "';";




		cErrMsg;  
		nRes = sqlite3_exec(pDB , strSql.c_str() ,0 ,0, &cErrMsg);


	}
    
	return 0;  
} 

void CDeReSearch::DeleteOldData()
{
	if( 0==isInit() )
	{
		return  ;
	}

	sqlite3 * pDB = ( sqlite3 * )m_pDB ;

	int nRes = 0;

	//select fileID from ( select *  from fileid order by findtime limit 90 ) order by usereferences   limit 10;

	string strSql = "";
	strSql = "select fileID from ( select *  from fileid order by findtime limit ";
	char chNum[32] = "" ;
	int iNum = m_iMaxCount*0.9;
	sprintf(chNum ,"%d" ,iNum );
	strSql += chNum;
	strSql +=  " ) order by usereferences   limit ";
	iNum = m_iMaxCount*0.1;
	sprintf(chNum ,"%d" ,iNum );
	strSql += chNum;
	strSql += ";";


	char* cErrMsg;  
    nRes = sqlite3_exec(pDB , strSql.c_str() ,deleteResult ,m_pDB, &cErrMsg);  
    if (nRes != SQLITE_OK)    
    {  
        //cout<<"add fileID fail: "<<cErrMsg<<endl;  
        return ;  
    }  



}

void CDeReSearch::UpdateIncludeNameByFID( char * pchFID ,char * pchIncludeFileName)
{
	if( NULL==pchFID  )
	{
		return  ;
	}

	if( 0==isInit() )
	{
		return  ;
	}

	sqlite3 * pDB = ( sqlite3 * )m_pDB ;


	string strSql;
	strSql.clear();
    strSql += "update fileID set includeFileName = ";
	strSql += "'";
	strSql += pchIncludeFileName;
	strSql += "'";
    strSql += " where fileID ='";  
    strSql += pchFID;    
    strSql += "';";  
  
    char* cErrMsg;  
    int nRes = sqlite3_exec(pDB , strSql.c_str() ,0 ,0, &cErrMsg);  
    if (nRes != SQLITE_OK)    
    {  
        //cout<<"update fileID fail: "<<cErrMsg<<endl;  
        return ;  
    }  


	return  ;
}


void CDeReSearch::UpdateFID( char * pchFID )
{
	if( NULL==pchFID  )
	{
		return  ;
	}

	if( 0==isInit() )
	{
		return  ;
	}

	sqlite3 * pDB = ( sqlite3 * )m_pDB ;


	string strSql;
	strSql.clear();
    strSql += "update fileID set usereferences = usereferences + 1";
    strSql += " where fileID ='";  
    strSql += pchFID;    
    strSql += "';";  
  
    char* cErrMsg;  
    int nRes = sqlite3_exec(pDB , strSql.c_str() ,0 ,0, &cErrMsg);  
    if (nRes != SQLITE_OK)    
    {  
        //cout<<"update fileID fail: "<<cErrMsg<<endl;  
        return ;  
    }  


	return  ;
  
}

int  CDeReSearch::isInit()
{
	return m_init ;
}
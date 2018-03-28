#pragma once

#include "..\..\..\TEPCode\interface\sqlite\SqliteDataBase.h"
#include "..\..\..\TEPCode\interface\IActiveObject.h"

#ifndef _STORAGE_TABLE_NAME_
#define _STORAGE_TABLE_NAME_	L"Storage.db"
#endif
#define STORAGEFILE_TABLE_NAME	L"StorageFile"

#define  IMAX_DETECT_ROW  256
typedef struct _STORAGE_FILE_
{
	CString		Guid;
	CString		FileName;
	CString		FilePath;
	CString		StorageTime;			// LONGLONG类型的数字

	CString		Remark;

}StorageFile, *PStorageFile;


class CStorageFileTable : public CSqliteDataBase, public CLiMutex
{
public:
	static CStorageFileTable* Instance()
	{
		static CStorageFileTable obj;

		return &obj;
	}
	~CStorageFileTable()
	{
		if (IsOpen_)
		{
			Close();
		}
	}

public:

	BOOL UpdateRecord(StorageFile record)
	{
		Lock();
		BOOL ret = FALSE;

		char** Result;

		CString sql;
		sql.Format(L"select * from '%s' where Guid = '%s'", STORAGEFILE_TABLE_NAME, record.Guid);

		int row = 0, col = 0;

		// 		CString FileName = record.FileName;
		// 		FileName.Replace(L"\'", L"\'\'");
		// 		CString FilePath = record.FilePath;
		// 		FilePath.Replace(L"\'", L"\'\'");
		// 		CString IsolateReason = record.IsolateReason;
		// 		IsolateReason.Replace(L"\'", L"\'\'");
		// 		CString Remark = record.Remark;
		// 		Remark.Replace(L"\'", L"\'\'");

		if (Select(sql, Result, row, col))
		{
			if (row >= 1)
			{
				sql.Format(L"update '%s' set FileName='%s', FilePath='%s',\
							StorageTime='%s',\
							Remark='%s' where Guid='%s'", 
							STORAGEFILE_TABLE_NAME, 
							record.FileName, record.FilePath,
							record.StorageTime,
							record.Remark, record.Guid);
				Exec(sql);
			}
			else
			{

				sql.Format(L"insert into '%s'(Guid, FileName, FilePath, StorageTime, Remark) \
							values('%s','%s','%s','%s','%s')",
							STORAGEFILE_TABLE_NAME, record.Guid, 
							record.FileName, record.FilePath,
							record.StorageTime, record.Remark);
				Exec(sql);
			}
			ret = TRUE;
		}

		if(Result != NULL)
			sqlite3_free_table(Result);

		Unlock();
		return ret;
	}

	BOOL QueryRecord(CAtlList<StorageFile>& recordList, CString guid = L"")
	{
		BOOL ret = FALSE;
		Lock();
		CString sql;
		char** Result = NULL;
		int row, col;
		char strMaxRow[32];
		memset( strMaxRow, 0 ,32);
		itoa(IMAX_DETECT_ROW,strMaxRow,10);
		//_itoa(IMAX_DETECT_ROW,strMaxRow,10);
		if (guid.IsEmpty())
			sql.Format(L"select * from '%s'", STORAGEFILE_TABLE_NAME);
		else
			sql.Format(L"select * from '%s' where guid='%s' LIMIT %s ", STORAGEFILE_TABLE_NAME, guid , strMaxRow );
		if (Select(sql, Result, row, col))
		{
			int j = col;
			for(int i = 0; i < row; i++)
			{
				StorageFile record;
				record.Guid				= *(Result + (j++));
				record.FileName			= *(Result + (j++));
				record.FilePath			= *(Result + (j++));
				record.StorageTime		= *(Result + (j++));
				record.Remark			= *(Result + (j++));
				recordList.AddTail(record);
			}
			ret = TRUE;
		}
		if(Result != NULL)
			sqlite3_free_table(Result);
		Unlock();
		return ret;
	}
	BOOL DeleteRecord(CString guid=L"")
	{
		BOOL ret = TRUE;
		Lock();
		CString sql;
		if (guid.IsEmpty())
			sql.Format(L"delete from '%s'", STORAGEFILE_TABLE_NAME);
		else
			sql.Format(L"delete from '%s' where guid = '%s'", STORAGEFILE_TABLE_NAME, guid);
		ret = Exec(sql);
		Unlock();
		return ret;
	}

	// 	BOOL DeleteRecordByIsolatePath(CString path)
	// 	{
	// 		BOOL ret;
	// 		Lock();
	// 		CString sql;
	// 		sql.Format(L"delete from '%s' where IsolatePath = '%s'", TABLE_NAME, path);
	// 		ret = Exec(sql);
	// 		Unlock();
	// 		return ret;
	// 	}
	// 辅助函数
public:
	CStringW NewGuid()
	{
		// {F05D53A2-BC6B-4C34-9A3D-C169678993F3} // len = 38
		GUID guid;
		CoCreateGuid(&guid);
		TCHAR buff[64] = {0};
		StringFromGUID2(guid, buff, 64);
		return buff;
	}

	CString DBPath()
	{
		CString path = CMiscHelper::RegGetString(L"StorageDLP\\InstallPath", L"E:");
		path.TrimRight(L"\\");
		path += L"\\DB";

		return path;
	}
private:
	BOOL IsOpen_;
	CStorageFileTable()
	{
		IsOpen_ = Open( DBPath() + L"\\" + _STORAGE_TABLE_NAME_);
		if (IsOpen_)
		{
			CString sql;
			// 表基础结构
			sql.Format(L"create table if not exists '%s'(Guid VARCHAR(128),\
						FileName BLOB, FilePath BLOB,\
						StorageTime TEXT,\
						Remark BLOB)", STORAGEFILE_TABLE_NAME);			
			Exec(sql);
		}
		OpenLock(L"StorageFile");
	}
};

#define CStorageFileTableInstance() CStorageFileTable::Instance()
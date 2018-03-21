#pragma once

#include "..\..\..\TEPCode\interface\sqlite\SqliteDataBase.h"
#include "..\..\..\TEPCode\interface\IActiveObject.h"
#include "..\..\..\TEPCode\interface\CMiscHelper.h"

#ifndef _STORAGE_TABLE_NAME_
#define _STORAGE_TABLE_NAME_	L"Storage.db"
#endif

#define STORAGEISOLATE_TABLE_NAME	L"StorageIsolate"

typedef struct _STORAGE_IASOLATE_
{
	CString		Guid;
	CString		FileName;
	CString		FilePath;
	CString		IsolatePath;
	CString		IsolateReason;
	CString		IsolateTime;			// LONGLONG类型的数字

	CString		Remark;

}StorageIsoalte, *PStorageIsolate;


class CStorageIsolateTable : public CSqliteDataBase, public CLiMutex
{
public:
	static CStorageIsolateTable* Instance()
	{
		static CStorageIsolateTable obj;

		return &obj;
	}
	~CStorageIsolateTable()
	{
		if (IsOpen_)
		{
			Close();
		}
	}

public:
	
	BOOL UpdateRecord(StorageIsoalte record)
	{
		Lock();
		BOOL ret = FALSE;

		char** Result;

		CString sql;
		sql.Format(L"select * from '%s' where Guid = '%s'", STORAGEISOLATE_TABLE_NAME, record.Guid);
		
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
				sql.Format(L"update '%s' set FileName='%s', FilePath='%s', IsolatePath='%s',\
							IsolateReason='%s', IsolateTime='%s',\
							Remark='%s' where Guid='%s'", 
							STORAGEISOLATE_TABLE_NAME, 
							record.FileName, record.FilePath, record.IsolatePath
							,  record.IsolateReason, record.IsolateTime,
							record.Remark, record.Guid);
				Exec(sql);
			}
			else
			{

				sql.Format(L"insert into '%s'(Guid, FileName, FilePath, IsolatePath, IsolateReason, IsolateTime, Remark) \
							values('%s','%s','%s','%s','%s','%s','%s')",
							STORAGEISOLATE_TABLE_NAME, record.Guid, 
							record.FileName, record.FilePath, record.IsolatePath
							,record.IsolateReason, record.IsolateTime, record.Remark);
				Exec(sql);
			}
			ret = TRUE;
		}

		if(Result != NULL)
			sqlite3_free_table(Result);

		Unlock();
		return ret;
	}

	BOOL QueryRecord(CAtlList<StorageIsoalte>& recordList, CString guid=L"")
	{
		BOOL ret = FALSE;
		Lock();
		CString sql;
		char** Result = NULL;
		int row, col;
		if (guid.IsEmpty())
			sql.Format(L"select * from '%s'", STORAGEISOLATE_TABLE_NAME);
		else
			sql.Format(L"select * from '%s' where guid='%s'", STORAGEISOLATE_TABLE_NAME, guid);
		if (Select(sql, Result, row, col))
		{
			int j = col;
			for(int i = 0; i < row; i++)
			{
				StorageIsoalte record;
				record.Guid				= *(Result + (j++));
				record.FileName			= *(Result + (j++));
				record.FilePath			= *(Result + (j++));
				record.IsolatePath		= *(Result + (j++));
				record.IsolateReason	= *(Result + (j++));
				record.IsolateTime		= *(Result + (j++));
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
			sql.Format(L"delete from '%s'", STORAGEISOLATE_TABLE_NAME);
		else
			sql.Format(L"delete from '%s' where guid = '%s'", STORAGEISOLATE_TABLE_NAME, guid);
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
		DWORD dw = GetLastError();
		CString path = CMiscHelper::RegGetString(L"StorageDLP\\InstallPath", L"E:");
		dw = GetLastError();
		path.TrimRight(L"\\");
		path += L"\\DB";
		
		return path;
	}
private:
	BOOL IsOpen_;
	CStorageIsolateTable()
	{
		IsOpen_ = Open( DBPath() + L"\\" + _STORAGE_TABLE_NAME_);
		if (IsOpen_)
		{
			CString sql;
			// 表基础结构
			sql.Format(L"create table if not exists '%s'(Guid VARCHAR(128),\
						FileName BLOB, FilePath BLOB,\
						IsolatePath BLOB, IsolateReason BLOB,\
						IsolateTime TEXT,\
						Remark BLOB)", STORAGEISOLATE_TABLE_NAME);			
			Exec(sql);
		}
		OpenLock(L"StorageIsolate");
	}
};

#define CStorageIsolateTableInstance() CStorageIsolateTable::Instance()
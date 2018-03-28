#pragma once

//��ֹ�ظ�Ƕ��,һ������ͷ�ļ���ֹ�ظ�Ƕ��
#if		!defined(_FILE_ISOLATE_H_)
#define	 _FILE_ISOLATE_H_

#include "stdafx.h"
#include <atlbase.h>
#include <atlstr.h>
#include <atlcoll.h>
#include <atlconv.h>
#include <commdlg.h>

//#include "..\..\include\msgbus\MsgDispatcher.h"


// �����ļ�����
typedef struct _IsolateFileInfo
{
	CStringW	fileHead;			// �ṹ��ͷ����Ϣ(������֤�ļ��е������Ƿ�Ϊ�ýṹ����Ϣ)
	CStringW	RcFileName;			// ԭ�ļ�����
	CStringW	IsolatebinFilePath;	// �������ļ�·��
	CStringW	IsolateTime;		// ����ʱ��
	CStringW	Remark;				// ��ע
	CStringW	CheckPolicyId;		// ������ID  ruleid
	CStringW	ScanPolicyId;		// ɨ�����ID
	CStringW	FileMarkGuid;		// �ļ�Ψһ��ʾGUID			�����ݿ���Ϊ��¼��GUID

}ISOLATEINFO, FAR* LPISOLATEINFO;



#define		ISLT_FILE_TYPE			L".dlp"		// ������Ϣ�ļ���׺��
#define		ISLT_FILE_HEAD			L"{A9AAA585-745B-44C8-90D4-E1A0540907ED}"	// �����ļ�ͷ

class CFileIsolatebin
{
public:
	static CFileIsolatebin* Instance()
	{
		static CFileIsolatebin _obj;
		return &_obj;
	}
	~CFileIsolatebin(){DeleteCriticalSection(&m_Lock);}

public:
	CString GetIsolateReg()
	{
		static CString root;
		if (root.IsEmpty())
		{
			root = CMiscHelper::RegGetString(L"StorageDLP\\IsolationDir");
		}
		
// 		if (!root.IsEmpty())
// 		{
// 			root = root + L"Isolatebin\\";
// 			SetPath(root);
// 		}
		
		return root;
	}

	CString GetIsolateApp()
	{
		TCHAR cPath[MAX_PATH];
		GetCurrentDirectory(MAX_PATH, cPath);
		CString root = cPath;
		int index = root.Find(L":\\");
		root = root.Left(index + 2);
		CMiscHelper::RegSetString(L"StorageDLP\\IsolationDir", root);
		//root += L"Isolatebin\\";
		SetPath(root);

		return root;
	}

	void CreateIsolatePath()
	{
		EnterCriticalSection(&m_Lock);
		if(!PathIsDirectory(m_IsolatebinPath) && CreateDirectory(m_IsolatebinPath, NULL))
		{
			SetFileAttributes(m_IsolatebinPath, FILE_ATTRIBUTE_HIDDEN);
		}
		LeaveCriticalSection(&m_Lock);
	}

	CString GetIsolateBin()
	{
		return m_IsolatebinPath;
	}
private:
	CFileIsolatebin(){InitializeCriticalSection(&m_Lock);}
	void SetPath(CString path)
	{
		EnterCriticalSection(&m_Lock);
		m_IsolatebinPath = path;
		LeaveCriticalSection(&m_Lock);
	}
private:
	CString								m_IsolatebinPath;
	CRITICAL_SECTION					m_Lock;
};


class CFileIsolateCommon
{
public:
	CFileIsolateCommon(){}
	~CFileIsolateCommon(){}

public:
	static CStringW GetCurrTime()
	{
		return CTime::GetCurrentTime().Format(L"%Y-%m-%d %H:%M:%S");
	}

	static CStringW NewGUID()
	{
		// {F05D53A2-BC6B-4C34-9A3D-C169678993F3} // len = 38
		GUID guid;
		CoCreateGuid(&guid);
		TCHAR buff[64] = {0};
		StringFromGUID2(guid, buff, 64);
		return buff;
	}
	// ��ȡ������·��������ɹ�·��ĩβ���Դ� \, �磺C:\Isolatebin\     
	static CStringW GetIsolatebinPath()
	{
		CString path = CFileIsolatebin::Instance()->GetIsolateBin();
		if(path.IsEmpty())
		{
			path = CFileIsolatebin::Instance()->GetIsolateReg();
			if(path.IsEmpty())
			{
				path = CFileIsolatebin::Instance()->GetIsolateApp();
			}
			CFileIsolatebin::Instance()->CreateIsolatePath();
		}
		return path;
	}

	// ��ȡ������·��������ɹ�·��ĩβ���Դ� \, �磺C:\Isolatebin\  �ֿ����Ȩ�޵�����
	static CString GetIsolateRegPath()
	{
		CString path;// = CFileIsolatebin::Instance()->GetIsolateBin();
// 		if(path.IsEmpty())
// 		{
 			path = CFileIsolatebin::Instance()->GetIsolateReg();
// 			CFileIsolatebin::Instance()->CreateIsolatePath();
// 		}
		return path;
	}


	static CString CreateIsolateEventLog(ISOLATEINFO info, LPCWSTR explain, CString SourcePath=L"")
	{
		CXmlMessage msg;
		if(S_OK != msg.Create(NULL, L"tdsvc.client", L"tdsvc.dlplocationscanner", L"SendEventLog"))
			return L"";
		if (SourcePath.Right(1) != '\\')
			SourcePath += L"\\";

		msg.AddTail(L".", L"FileName",			SourcePath + info.RcFileName);
		msg.AddTail(L".", L"IsolateTime",		info.IsolateTime);
		msg.AddTail(L".", L"IsolateReason",		info.Remark);
		msg.AddTail(L".", L"CheckPolicyId",		info.CheckPolicyId);
		msg.AddTail(L".", L"ScanPolicyId",		info.ScanPolicyId);
		msg.AddTail(L".", L"Remark",			explain);
		msg.AddTail(L".", L"GuidFileMark",		info.FileMarkGuid);

		CString str;
		msg.Save(str);
		return str;
	}

	static BOOL IsolateFileDialog(CString& fileName, BOOL bRead = TRUE)
	{
		TCHAR chfileName[MAX_PATH * 4] = {0};


		CString fp = chfileName;
		CString dialogTitle = L"�ļ�" + (CString)chfileName + L"δ�ҵ�, ������ѡ��";

		CString strFilter;
		if (bRead)
		{
			strFilter = TEXT("�����ļ�\0*.*\0�����ļ�(*.dlp)\0*.dlp\0");
			wcscpy(chfileName, fileName);
		}
		else
		{
			strFilter = TEXT("�����ļ�\0*.*\0");
			CString fn = PathFindFileName(fileName);
			fn.TrimRight(L".dlp");
			wcscpy(chfileName, fn);
		}

		OPENFILENAME ofn;
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = NULL;
		ofn.hInstance = NULL;

		ofn.lpstrFilter = strFilter;
		ofn.lpstrCustomFilter = NULL;
		ofn.nMaxCustFilter = 1;

		ofn.lpstrFile = chfileName;
		ofn.nMaxFile = MAX_PATH;
		ofn.lpstrFileTitle = NULL;
		ofn.nMaxFileTitle = MAX_PATH;
		ofn.lpstrInitialDir = NULL;
		ofn.lpstrTitle = bRead ? dialogTitle : NULL;

		ofn.Flags = OFN_OVERWRITEPROMPT;
		ofn.nFileOffset = 0;
		ofn.nFileExtension = 0;
		ofn.lpstrDefExt = TEXT("dlp");
		ofn.lCustData = 0;
		ofn.lpfnHook = NULL;
		ofn.lpTemplateName = NULL;

		BOOL ret = FALSE;
		if (bRead)
			ret = GetOpenFileName(&ofn);
		else
			ret = GetSaveFileName(&ofn);

		fileName = chfileName;
		return ret;
	}
};


class CFileIsolateInfo
{
public:
	CFileIsolateInfo(){}
	~CFileIsolateInfo(){}

public:
	
	// �Ӹ�����Ϣ�ļ� ��ȡ������Ϣ
	// pszIsolateInfoFilePath	������Ϣ�ļ�(��������ļ���� .dlp �ļ�)
	// pInfo					���Σ� ������Ϣ
	BOOL GetIsolateInfo(LPCWSTR pszIsolateInfoFilePath, LPISOLATEINFO pInfo)
	{
		BOOL	bRetVal		= FALSE;
		DWORD	dwError		= ERROR_SUCCESS;

		if (pszIsolateInfoFilePath == NULL  || pszIsolateInfoFilePath[0] == 0  || pInfo == NULL)
		{
			SetLastError(ERROR_INVALID_PARAMETER);
			return FALSE;
		}

		do 
		{
			CAtlFile file;
			HRESULT hr = file.Create(pszIsolateInfoFilePath, GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING);
			if (hr != S_OK)
			{
				dwError = hr;
				break;
			}

			if (!ReadIsolateInfo(file, pInfo))
			{
				dwError = GetLastError();
				break;
			}

			bRetVal = TRUE;
		} while (FALSE);

		SetLastError(dwError);
		return bRetVal;
	}
	
	// ���ø�����Ϣ
	// ����������Ϣ�ļ�(���)
	// pszRcFilePath	��������ļ�·��
	// pInfo			������Ϣ
	BOOL CreateIsolateInfoFile(LPCWSTR pszRcFilePath, LPISOLATEINFO pInfo)
	{
		if (pszRcFilePath == NULL  || pInfo == NULL)
		{
			SetLastError(ERROR_INVALID_PARAMETER);
			return FALSE;
		}

		BOOL		bRetValue	= FALSE;
		DWORD		dwError		= ERROR_SUCCESS;
		CAtlFile	file;

		do 
		{
			// �������ļ�������������串��
			HRESULT hr = file.Create(pszRcFilePath, GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ, CREATE_ALWAYS);
			if (hr != S_OK)
			{
				dwError = hr;
				break;
			}

			if (!WriteIsolateInfo(file.m_h, pInfo))
			{
				dwError = GetLastError();
				break;
			}

			bRetValue = TRUE;

		} while (FALSE);

		file.Close();

		if (!bRetValue)
		{
			::DeleteFile(pszRcFilePath);
		}

		SetLastError(dwError);
		return bRetValue;
	}



public:
	
	// ��ȡ ISOLATEINFO �ṹ��
	BOOL ReadIsolateInfo(HANDLE hFile, LPISOLATEINFO pInfo)
	{
		if (hFile == NULL  ||  hFile == INVALID_HANDLE_VALUE  ||  pInfo == NULL)
		{
			SetLastError(ERROR_INVALID_PARAMETER);
			return FALSE;
		}

		BOOL	bRetVal	= FALSE;
		DWORD	dwError	= ERROR_SUCCESS;
		do 
		{
			DWORD	dwRead	= 0;
			int		nLen	= 0;

			if (!ReadFile(hFile, (LPVOID)&nLen, sizeof(nLen), &dwRead, NULL))
				break;
			if (nLen < 0 || nLen > 10000)
				break;
			if (!ReadFile(hFile, (LPVOID)pInfo->fileHead.GetBuffer(nLen), nLen, &dwRead, NULL))
				break;	
			/*if (	!ReadFile(hFile, (LPVOID)&nLen, sizeof(nLen), &dwRead, NULL)
		||	!ReadFile(hFile, (LPVOID)pInfo->fileHead.GetBuffer(nLen), nLen, &dwRead, NULL))
				break;*/
			pInfo->fileHead.ReleaseBuffer();

			// ��֤�ļ�ͷ
			if (pInfo->fileHead.CompareNoCase(ISLT_FILE_HEAD))
			{
				SetLastError(ERROR_INVALID_TARGET_HANDLE);// Ŀ���ڲ��ļ���ʶ������ȷ�� 
				break;
			}

			if (	!ReadFile(hFile, (LPVOID)&nLen, sizeof(nLen), &dwRead, NULL)
				||	!ReadFile(hFile, (LPVOID)pInfo->RcFileName.GetBuffer(nLen), nLen, &dwRead, NULL))
				break;
			pInfo->RcFileName.ReleaseBuffer();


			if (	!ReadFile(hFile, (LPVOID)&nLen, sizeof(nLen), &dwRead, NULL)
				||	!ReadFile(hFile, (LPVOID)pInfo->IsolatebinFilePath.GetBuffer(nLen), nLen, &dwRead, NULL))
				break;
			pInfo->IsolatebinFilePath.ReleaseBuffer();


			if (	!ReadFile(hFile, (LPVOID)&nLen, sizeof(nLen), &dwRead, NULL)
				||	!ReadFile(hFile, (LPVOID)pInfo->IsolateTime.GetBuffer(nLen), nLen, &dwRead, NULL))
				break;
			pInfo->IsolateTime.ReleaseBuffer();


			if (	!ReadFile(hFile, (LPVOID)&nLen, sizeof(nLen), &dwRead, NULL)
				||	!ReadFile(hFile, (LPVOID)pInfo->Remark.GetBuffer(nLen), nLen, &dwRead, NULL))
				break;
			pInfo->Remark.ReleaseBuffer();

			if (	!ReadFile(hFile, (LPVOID)&nLen, sizeof(nLen), &dwRead, NULL)
				||	!ReadFile(hFile, (LPVOID)pInfo->CheckPolicyId.GetBuffer(nLen), nLen, &dwRead, NULL))
				break;
			pInfo->CheckPolicyId.ReleaseBuffer();

			if (	!ReadFile(hFile, (LPVOID)&nLen, sizeof(nLen), &dwRead, NULL)
				||	!ReadFile(hFile, (LPVOID)pInfo->ScanPolicyId.GetBuffer(nLen), nLen, &dwRead, NULL))
				break;
			pInfo->ScanPolicyId.ReleaseBuffer();

			if (	!ReadFile(hFile, (LPVOID)&nLen, sizeof(nLen), &dwRead, NULL)
				||	!ReadFile(hFile, (LPVOID)pInfo->FileMarkGuid.GetBuffer(nLen), nLen, &dwRead, NULL))
				break;
			pInfo->FileMarkGuid.ReleaseBuffer();

			bRetVal = TRUE;
		} while (FALSE);

		SetLastError(GetLastError());
		return bRetVal;
	}

	// д�� ISOLATEINFO �ṹ��
	BOOL WriteIsolateInfo(HANDLE hFile, LPISOLATEINFO pInfo)
	{
		if (hFile == NULL  ||  hFile == INVALID_HANDLE_VALUE  ||  pInfo == NULL)
		{
			SetLastError(ERROR_INVALID_PARAMETER);
			return FALSE;
		}

		BOOL bRetVal = FALSE;
		do 
		{
			int nLen = (pInfo->fileHead.GetLength() + 1) * sizeof(WCHAR);
			if (	!WriteData(hFile, (PVOID)&nLen, sizeof(nLen))
				||	!WriteData(hFile, (PVOID)(LPCWSTR)pInfo->fileHead, nLen))
				break;

			nLen = (pInfo->RcFileName.GetLength() + 1) * sizeof(WCHAR);
			if (	!WriteData(hFile, (PVOID)&nLen, sizeof(nLen))
				||	!WriteData(hFile, (PVOID)(LPCWSTR)pInfo->RcFileName, nLen))
				break;

			nLen = (pInfo->IsolatebinFilePath.GetLength() + 1) * sizeof(WCHAR);
			if (	!WriteData(hFile, (PVOID)&nLen, sizeof(nLen))
				||	!WriteData(hFile, (PVOID)(LPCWSTR)pInfo->IsolatebinFilePath, nLen))
				break;

			nLen = (pInfo->IsolateTime.GetLength() + 1) * sizeof(WCHAR);
			if (	!WriteData(hFile, (PVOID)&nLen, sizeof(nLen))
				||	!WriteData(hFile, (PVOID)(LPCWSTR)pInfo->IsolateTime, nLen))
				break;

			nLen = (pInfo->Remark.GetLength() + 1) * sizeof(WCHAR);
			if (	!WriteData(hFile, (PVOID)&nLen, sizeof(nLen))
				||	!WriteData(hFile, (PVOID)(LPCWSTR)pInfo->Remark, nLen))
				break;

			nLen = (pInfo->CheckPolicyId.GetLength() + 1) * sizeof(WCHAR);
			if (	!WriteData(hFile, (PVOID)&nLen, sizeof(nLen))
				||	!WriteData(hFile, (PVOID)(LPCWSTR)pInfo->CheckPolicyId, nLen))
				break;

			nLen = (pInfo->ScanPolicyId.GetLength() + 1) * sizeof(WCHAR);
			if (	!WriteData(hFile, (PVOID)&nLen, sizeof(nLen))
				||	!WriteData(hFile, (PVOID)(LPCWSTR)pInfo->ScanPolicyId, nLen))
				break;

			nLen = (pInfo->FileMarkGuid.GetLength() + 1) * sizeof(WCHAR);
			if (	!WriteData(hFile, (PVOID)&nLen, sizeof(nLen))
				||	!WriteData(hFile, (PVOID)(LPCWSTR)pInfo->FileMarkGuid, nLen))
				break;

			bRetVal = TRUE;
		} while (FALSE);

		SetLastError(GetLastError());
		return bRetVal;
	}

	// д������
	BOOL WriteData(HANDLE hFile, const PVOID lpBuffer, DWORD dwBuffLen)
	{
		BOOL			bRetValue	= FALSE;
		DWORD			dwError		= ERROR_SUCCESS;

		DWORD			dwWrite		= 0;
		DWORD			dwOffset	= 0;
		DWORD			dwTotal		= dwBuffLen;
		while (dwOffset < dwBuffLen)
		{
			bRetValue = ::WriteFile(hFile, ((PBYTE)lpBuffer)+dwOffset, dwTotal, &dwWrite, NULL);
			if (!bRetValue) 
			{
				dwError = GetLastError();
				break;
			}

			dwOffset	+= dwWrite;
			dwTotal		-= dwWrite;
		}

		SetLastError(dwError);
		return bRetValue;
	}

};


class CFileIsolateEncrypt
{
public:
	CFileIsolateEncrypt()		
		: m_rijndael()
		, m_nEncryptBlockLen(1024*1024)
	{
		// ��ʼ����Կ
		UCHAR KEY[16]= 
		{ 0x25, 0x3, 0xa7, 0x4d, 0x88, 0x05, 0x47, 0x8d, 0x29, 0xf9, 0xc1, 0x9f, 0x71, 0xb2, 0xe0, 0x19};

		m_rijndael.SetKey(KEY, 16);
	}
	~CFileIsolateEncrypt()
	{
		// �����Կ
		m_rijndael.ClearKey();
	}

	// ����
	// ����ָ���ļ�
	// pszRcFilePath	Դ�ļ�·��
	// pszDestFilePath	�����ļ����λ��(�����ļ���)
	BOOL EncryptFile(LPCWSTR pszRcFilePath, LPCWSTR pszDestFilePath)
	{
		if (pszRcFilePath == NULL  || pszRcFilePath[0] == 0  ||  pszDestFilePath == NULL  ||  pszDestFilePath[0] == 0)
		{
			SetLastError(ERROR_INVALID_PARAMETER);
			return FALSE;
		}

		BOOL		bRetVal			= FALSE;
		DWORD		dwError			= ERROR_SUCCESS;
		LPBYTE		pReadBuff		= NULL;
		LPBYTE		pEncryptBuff	= NULL;
		CAtlFile	rcFile;
		CAtlFile	destFile;

		do 
		{
			// ���仺����
			pReadBuff		= new BYTE[m_nEncryptBlockLen];
			pEncryptBuff	= new BYTE[m_nEncryptBlockLen];
			if (pReadBuff == NULL  ||  pEncryptBuff == NULL)
			{
				dwError = ERROR_IPSEC_IKE_OUT_OF_MEMORY;// �ڴ����ʧ��
				break;
			}
			ZeroMemory(pReadBuff, m_nEncryptBlockLen);
			ZeroMemory(pEncryptBuff, m_nEncryptBlockLen);

			// ��Դ�ļ��� ����Ŀ���ļ�
			HRESULT hr1 = rcFile.Create(pszRcFilePath, GENERIC_READ, FILE_SHARE_READ|FILE_SHARE_WRITE, OPEN_EXISTING);
			HRESULT hr2 = destFile.Create(pszDestFilePath, GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ, CREATE_ALWAYS);
			if (hr1 != S_OK  ||  hr2 != S_OK)
			{
				dwError = (hr1 != S_OK? hr1 : hr2);
				break;
			}

			ULONG	ulOffset	= 0;
			DWORD	dwRead		= 0;
			while (::ReadFile(rcFile, pReadBuff, m_nEncryptBlockLen, &dwRead, NULL)  &&  dwRead > 0)
			{
				m_rijndael.Encrypt(pReadBuff, dwRead, pEncryptBuff, m_nEncryptBlockLen, ulOffset);

				if (!FileIsolateInfo_.WriteData(destFile, pEncryptBuff, dwRead))
				{
					dwError = GetLastError();
					break;
				}
				ulOffset += dwRead;
				ZeroMemory(pReadBuff, m_nEncryptBlockLen);
				ZeroMemory(pEncryptBuff, m_nEncryptBlockLen);
				dwRead = 0;
			}

			if (dwError != ERROR_SUCCESS)
			{
				break;
			}

			bRetVal = TRUE;

		} while (FALSE);

		rcFile.Close();
		destFile.Close();

		if (pReadBuff)
		{
			delete [] pReadBuff;
		}
		if (pEncryptBuff)
		{
			delete [] pEncryptBuff;
		}
		if (!bRetVal)
		{
			::DeleteFile(pszDestFilePath);
		}
		SetLastError(dwError);
		return bRetVal;
	}

	// ����
	// �����ļ�
	// pszRcFilePath	�����ļ�·��
	// pszDestFilePath	�����ļ����λ��(�����ļ���)
	BOOL DecryptFile(LPCWSTR pszRcFilePath, LPCWSTR pszDestFilePath)
	{
		if (pszRcFilePath == NULL  || pszRcFilePath[0] == 0  ||  pszDestFilePath == NULL  ||  pszDestFilePath[0] == 0)
		{
			SetLastError(ERROR_INVALID_PARAMETER);
			return FALSE;
		}

		BOOL		bRetVal			= FALSE;
		DWORD		dwError			= ERROR_SUCCESS;
		LPBYTE		pReadBuff		= NULL;
		LPBYTE		pDecryptBuff	= NULL;
		CAtlFile	rcFile;
		CAtlFile	destFile;

		do 
		{
			// ���仺����
			pReadBuff		= new BYTE[m_nEncryptBlockLen];
			pDecryptBuff	= new BYTE[m_nEncryptBlockLen];
			if (pReadBuff == NULL  ||  pDecryptBuff == NULL)
			{
				dwError = ERROR_IPSEC_IKE_OUT_OF_MEMORY;// �ڴ����ʧ��
				break;
			}
			ZeroMemory(pReadBuff, m_nEncryptBlockLen);
			ZeroMemory(pDecryptBuff, m_nEncryptBlockLen);

			// ��Դ�ļ��� ����Ŀ���ļ�
			HRESULT hr1 = rcFile.Create(pszRcFilePath, GENERIC_READ, FILE_SHARE_READ|FILE_SHARE_WRITE, OPEN_EXISTING);
			HRESULT hr2 = destFile.Create(pszDestFilePath, GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ, CREATE_ALWAYS);
			if (hr1 != S_OK  ||  hr2 != S_OK)
			{
				dwError = (hr1 != S_OK? hr1 : hr2);
				break;
			}

			ULONG	ulOffset	= 0;
			DWORD	dwRead		= 0;
			while (::ReadFile(rcFile, pReadBuff, m_nEncryptBlockLen, &dwRead, NULL)  &&  dwRead > 0)
			{
				m_rijndael.Decrypt(pReadBuff, dwRead, pDecryptBuff, m_nEncryptBlockLen, ulOffset);

				if (!FileIsolateInfo_.WriteData(destFile, pDecryptBuff, dwRead))
				{
					dwError = GetLastError();
					break;
				}

				ulOffset += dwRead;
				ZeroMemory(pReadBuff, m_nEncryptBlockLen);
				ZeroMemory(pDecryptBuff, m_nEncryptBlockLen);
			}

			if (dwError != ERROR_SUCCESS)
			{
				break;
			}

			bRetVal = TRUE;

		} while (FALSE);

		rcFile.Close();
		destFile.Close();

		if (pReadBuff)
		{
			delete [] pReadBuff;
		}
		if (pDecryptBuff)
		{
			delete [] pDecryptBuff;
		}
		if (!bRetVal)
		{
			DeleteFile(pszDestFilePath);
		}
		SetLastError(dwError);
		return bRetVal;
	}
private:

	CRijndael	m_rijndael;
	const int	m_nEncryptBlockLen;		// ���ܿ鳤��

	CFileIsolateInfo FileIsolateInfo_;
};


class CFileIsolateOperator
{
public:
	CFileIsolateOperator()
	{

	}
	~CFileIsolateOperator()
	{

	}

	// ����
	// �����ļ�
	// pszRcFilePath	Ҫ��������ļ�����·��
	// pszRemark		����˵��
	BOOL IsolateFile(LPCWSTR pszRcFilePath, CString& stubFilePath, LPCWSTR pszRemark=L"", LPCWSTR pszCheckPolicyId=L"", LPCWSTR pszScanPolicyId=L"")
	{
		BOOL	bRetVal		= FALSE;
		DWORD	dwError		= ERROR_SUCCESS;

		do 
		{
			if (pszRcFilePath == NULL  ||  pszRcFilePath[0] == 0)
			{
				dwError = ERROR_INVALID_PARAMETER;
				break;
			}


			// ��ȡ������·��
			//CStringW	isobinPath	= GetIsolatebinPath();
			CStringW	isobinPath;
			isobinPath = CFileIsolateCommon::GetIsolateRegPath();
// 			if (isobinPath.IsEmpty())
// 				isobinPath = CFileIsolateCommon::GetIsolatebinPath();

			if (isobinPath.IsEmpty())
			{
				SetLastError(GetLastError());
				return FALSE;
			}

			// ���ļ�������������
			CString isoFilePath = CopyFileAutoName(pszRcFilePath, isobinPath);
			if (isoFilePath.IsEmpty())
			{
				dwError = GetLastError();
				break;
			}

			CString stubFile = pszRcFilePath;
			if(_waccess((CString)pszRcFilePath+ISLT_FILE_TYPE, 0) != -1)
			{
				stubFile += CTime::GetCurrentTime().Format(L"%Y%m%d%H%M%S");
			}
			stubFilePath = stubFile + ISLT_FILE_TYPE;

			// ��Դ�ļ�ɾ������д�����������ļ�
			ISOLATEINFO info;
			info.fileHead			= ISLT_FILE_HEAD;
			info.RcFileName			= PathFindFileName(pszRcFilePath);
			info.IsolatebinFilePath	= isoFilePath;
			info.IsolateTime		= CFileIsolateCommon::GetCurrTime();
			info.Remark				= pszRemark;
			info.CheckPolicyId      = pszCheckPolicyId;
			info.ScanPolicyId		= pszScanPolicyId;
			info.FileMarkGuid		= CFileIsolateCommon::NewGUID();

			if (!::DeleteFile(pszRcFilePath)  ||  !FileIsolateInfo_.CreateIsolateInfoFile(stubFile+ISLT_FILE_TYPE, &info))
			{
				dwError = GetLastError();
				// ���ʧ����Ҫ��ԭ�ļ�����ɾ�����������ļ�
				FileIsolateEncrypt_.DecryptFile(isoFilePath, pszRcFilePath);
				// ɾ���������ļ�
				::DeleteFile(isoFilePath);
				break;
			}

			bRetVal = TRUE;

		} while (FALSE);


		SetLastError(dwError);
		return bRetVal;
	}

	// �ָ�
	// ��ԭ��������ļ�
	// pszIsolateInfoFilePath	������Ϣ�ļ�(��������ļ���� .dlp �ļ�, ��C:\A.txt.dlp)
	// pszRemark				��ԭ˵��
	BOOL RestoreFile(LPCWSTR pszIsolateInfoFilePath,  ISOLATEINFO& info, CString IsolatePath = L"")
	{
		BOOL	bRetVal		= FALSE;
		DWORD	dwError		= ERROR_SUCCESS;
		CString tempFilePath = pszIsolateInfoFilePath;
		do 
		{
			if(!PathFileExists(tempFilePath))
			{
				return RestoreFileNoStub(IsolatePath, tempFilePath);
			}

			CAtlFile file;
			HRESULT hr = file.Create(pszIsolateInfoFilePath, GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING);
			if (hr != S_OK)
			{
				dwError = hr;
				break;
			}

			//ISOLATEINFO info;
			if (!FileIsolateInfo_.ReadIsolateInfo(file, &info))
			{
				dwError = GetLastError();
				break;
			}
			file.Close();

			// �õ�ǰԴ�ļ�·��ƴ��ԭʼ�ļ�����
			CString restorePath = pszIsolateInfoFilePath;
			PathRemoveFileSpec(restorePath.GetBuffer());	//ȥ���ļ�����ʣ��·������
			PathAppend(restorePath.GetBuffer(), info.RcFileName);
			restorePath.ReleaseBuffer();

			if (PathFileExists(restorePath))
			{
				if(!CFileIsolateCommon::IsolateFileDialog(restorePath, FALSE))
				{
					dwError = SCARD_W_CANCELLED_BY_USER;//�û�ȡ���˲���
					break;
				}
			}

			// �������ļ���������
			if (FileIsolateEncrypt_.DecryptFile(info.IsolatebinFilePath, restorePath))
			{
				// ɾ���������ļ�
				::DeleteFile(info.IsolatebinFilePath);
				// ɾ��������Ϣ�ļ�
				::DeleteFile(pszIsolateInfoFilePath);
			}
			else
			{
				dwError = GetLastError();
				if (dwError == E_ACCESSDENIED)
					return RestoreFileNoStub(IsolatePath, restorePath);
				
				break;
			}

			bRetVal = TRUE;
		} while (FALSE);


		SetLastError(dwError);
		return bRetVal;
	}

	
	BOOL RestoreFileNoStub(CString IsolatePath, CString restorePath)
	{
		BOOL	bRetVal		= FALSE;
		DWORD	dwError		= ERROR_SUCCESS;
		do 
		{
			if(!CFileIsolateCommon::IsolateFileDialog(restorePath, FALSE))
			{
				dwError = SCARD_W_CANCELLED_BY_USER;//�û�ȡ���˲���
				break;
			}

			// �������ļ���������
			if (FileIsolateEncrypt_.DecryptFile(IsolatePath, restorePath))
			{
				// ɾ���������ļ�
				::DeleteFile(IsolatePath);
			}
			else
			{
				dwError = GetLastError();
				break;
			}

			bRetVal = TRUE;
		} while (FALSE);

		SetLastError(dwError);
		return bRetVal;
	}
private:
		// ���ļ�������ָ��·���²������������ɹ������ļ�����·�����Ƿ񷵻ؿ�
		// pszRcFilePath	Դ�ļ�·��(�����ļ���)
		// pszIsoBinPath	Ŀ��·��(�������Ѵ��ڵ��ļ���)
		CStringW CopyFileAutoName(LPCWSTR pszRcFilePath, LPCWSTR pszDestPath)
		{
			if (	pszRcFilePath	== NULL	||  pszRcFilePath[0]	== 0 
				||  pszDestPath		== NULL	||  pszDestPath[0]		== 0)
			{
				SetLastError(ERROR_INVALID_PARAMETER);
				return L"";
			}
			if (!PathFileExists(pszRcFilePath)  ||  !PathFileExists(pszDestPath))
			{
				SetLastError(GetLastError());
				return L"";
			}

			// �����µ��ļ���
			WCHAR newPath[MAX_PATH] = {0};
			StringCchCopy(newPath, MAX_PATH, pszDestPath);
			PathAppend(newPath, CFileIsolateCommon::NewGUID() + L".isolate");

			// ���ܿ�����������
			if (!FileIsolateEncrypt_.EncryptFile(pszRcFilePath, newPath))
			{
				SetLastError(GetLastError());
				return L"";
			}

			return newPath;
		}
private:
	CFileIsolateInfo FileIsolateInfo_;
	CFileIsolateEncrypt FileIsolateEncrypt_;
};



#endif

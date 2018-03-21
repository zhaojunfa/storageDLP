#pragma once

#ifndef DLPFILEENCRYPTION_H_
#define DLPFILEENCRYPTION_H_


DWORD WINAPI WatchChanges( LPVOID lpParam );
void Drive_File_Watchchange( );
ULONG SetScanDir(CStringW& msg);//un
void SetScanInfo();
void FileWatchRelease();
extern BOOL enumpath(CString lpPath);
#endif
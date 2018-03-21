
#pragma once

DWORD WINAPI WatchChanges( LPVOID lpParam );
extern BOOL OnStart();
extern void SetList();
extern void BeginCheck();
extern BOOL OnStopCheck();
extern ULONG SetInfo(CStringW& msg);
extern unsigned int __stdcall Run(PVOID pM);
extern BOOL FindLocalIp(CString &Info);
extern void StartRead();

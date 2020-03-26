#pragma once

#include <Windows.h>
#include <Tlhelp32.h>
#include <Vector>
#include <SubAuth.h>
#include <stdint.h>
#include <Bits.h>
#ifndef _MUTATION_
#define _MUTATION_

class MUTATION
{
public:
    void    Seed(int iSeed);

private:

    int     m_iSeed;
};

#endif

//===========================================================================
typedef LONG(WINAPI* RealNtFlushInstructionCache)(HANDLE ProcessHandle, PVOID BaseAddress, ULONG NumberOfBytesToFlush);

RealNtFlushInstructionCache NewNtFlushInstructionCache = NULL;
//===========================================================================
typedef struct _PEB_LDR_DATA
{
    ULONG           Length;
    BOOLEAN         Initialized;
    PVOID           SsHandle;
    LIST_ENTRY      InLoadOrderModuleList;
    LIST_ENTRY      InMemoryOrderModuleList;
    LIST_ENTRY      InInitializationOrderModuleList;
} PEB_LDR_DATA, * PPEB_LDR_DATA;
//===========================================================================
typedef struct _LDR_MODULE
{
    LIST_ENTRY      InLoadOrderModuleList;
    LIST_ENTRY      InMemoryOrderModuleList;
    LIST_ENTRY      InInitializationOrderModuleList;
    PVOID           BaseAddress;
    PVOID           EntryPoint;
    ULONG           SizeOfImage;
    UNICODE_STRING  FullDllName;
    UNICODE_STRING  BaseDllName;
    ULONG           Flags;
    SHORT           LoadCount;
    SHORT           TlsIndex;
    LIST_ENTRY      HashTableEntry;
    ULONG           TimeDateStamp;
} LDR_MODULE, * PLDR_MODULE;
//===========================================================================
using namespace std;
#pragma warning(disable :4235)
extern "C" DWORD func1();
void Erase_PEB_Module(char* szModule)
{
    DWORD dwPEB = 0, dwOffset = 0;
    PLIST_ENTRY pUserModuleHead=0, pUserModule = 0;
    PPEB_LDR_DATA pLdrData;
    PLDR_MODULE pLdrModule;
    PUNICODE_STRING lpModule;
    char szModuleName[512];
    int i = 0, n = 0;

    dwPEB = func1();

    pLdrData = (PPEB_LDR_DATA)(PDWORD)(*(PDWORD)(dwPEB + 12));

    for (; i < 3; i++)
    {
        switch (i)
        {
        case 0:
            pUserModuleHead = pUserModule = (PLIST_ENTRY)(&(pLdrData->InLoadOrderModuleList));
            dwOffset = 0;
            break;

        case 1:
            pUserModuleHead = pUserModule = (PLIST_ENTRY)(&(pLdrData->InMemoryOrderModuleList));
            dwOffset = 8;
            break;
        case 2:
            pUserModuleHead = pUserModule = (PLIST_ENTRY)(&(pLdrData->InInitializationOrderModuleList));
            dwOffset = 16;
            break;
        }

        while (pUserModule->Flink != pUserModuleHead)
        {
            pUserModule = pUserModule->Flink;
            lpModule = (PUNICODE_STRING)(((DWORD)(pUserModule)) + (36 - dwOffset));

            for (n = 0; n < (lpModule->Length) / 2 && n < 512; n++)
                szModuleName[n] = (CHAR)(*((lpModule->Buffer) + (n)));

            szModuleName[n] = '\0';
            if (strstr(szModuleName, szModule))
            {
                pUserModule->Blink->Flink = pUserModule->Flink;
                pUserModule->Flink->Blink = pUserModule->Blink;
            }
        }
    }
}
//===========================================================================
void Erase_PE_Header(DWORD GetModuleBase)
{
    PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)GetModuleBase;
    PIMAGE_NT_HEADERS pNTHeader = (PIMAGE_NT_HEADERS)((DWORD)pDosHeader + (DWORD)pDosHeader->e_lfanew);

    if (pNTHeader->Signature != IMAGE_NT_SIGNATURE)
        return;

    if (pNTHeader->FileHeader.SizeOfOptionalHeader)
    {
        DWORD Protect;
        WORD Size = pNTHeader->FileHeader.SizeOfOptionalHeader;
        VirtualProtect((void*)GetModuleBase, Size, PAGE_EXECUTE_READWRITE, &Protect);
        RtlZeroMemory((void*)GetModuleBase, Size);
        VirtualProtect((void*)GetModuleBase, Size, Protect, &Protect);
    }
}
//===========================================================================
void InitializeMutation(void) {};

//===========================================================================
DWORD WINAPI lpMutation(LPVOID lpParams)
{
    int* iSeed = (int*)lpParams;

    if (iSeed == NULL)
    {
        iSeed = new int;
        *iSeed = rand() % 9999;
    }
    else
    {

        if (*iSeed == 0)
        {
            iSeed = new int;
            *iSeed = rand() % 9999;
        }
    }

    while (true)
    {
        MEMORY_BASIC_INFORMATION mbi;
        VirtualQuery(InitializeMutation, &mbi, sizeof(mbi));
        VirtualProtect(mbi.BaseAddress, mbi.RegionSize, PAGE_EXECUTE_READWRITE, &mbi.Protect);

        BYTE* byteArrayInitializeMutation = (BYTE*)InitializeMutation;

        for (int i = 0; i < 5; i++)
        {
            srand(GetTickCount() * (i + i * i) * (*iSeed));

            byteArrayInitializeMutation[i] = rand() % 255;
        }

        VirtualProtect(mbi.BaseAddress, mbi.RegionSize, mbi.Protect, NULL);
        NewNtFlushInstructionCache(GetCurrentProcess(), InitializeMutation, 5);
        Sleep(10);
    }

    return 0;
}
//===========================================================================
void MUTATION::Seed(int iSeed)
{
    m_iSeed = iSeed;
}

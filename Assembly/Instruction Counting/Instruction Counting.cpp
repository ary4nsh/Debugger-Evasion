/*
 * Debugger detection via instruction counting: set hardware breakpoints on
 * each of 4 NOPs. Vectored handler counts single-step (Eax++) and advances
 * Eip. If Eax reaches 4, no debugger; otherwise debugger interfered.
 */

#include "hwbrk.h"
#include <windows.h>
#include <iostream>

static const int m_nInstructionCount = 4;
static HWHBRK m_hHwBps[4] = { nullptr, nullptr, nullptr, nullptr };

static LONG WINAPI InstructionCountingExceptionHandler(PEXCEPTION_POINTERS pExceptionInfo)
{
    if (pExceptionInfo->ExceptionRecord->ExceptionCode == EXCEPTION_SINGLE_STEP)
    {
#if defined(_M_IX86)
        pExceptionInfo->ContextRecord->Eax += 1;
        pExceptionInfo->ContextRecord->Eip += 1;
        return EXCEPTION_CONTINUE_EXECUTION;
#else
        (void)pExceptionInfo;
        return EXCEPTION_CONTINUE_SEARCH;
#endif
    }
    return EXCEPTION_CONTINUE_SEARCH;
}

#if defined(_M_IX86)
__declspec(naked) DWORD WINAPI InstructionCountingFunc(LPVOID lpThreadParameter)
{
    __asm
    {
        xor eax, eax
        nop
        nop
        nop
        nop
        cmp al, 4
        jne being_debugged
    }

    ExitThread(FALSE);

being_debugged:
    ExitThread(TRUE);
}
#else
static DWORD WINAPI InstructionCountingFunc(LPVOID)
{
    ExitThread(FALSE);
    return 0;
}
#endif

bool IsDebugged()
{
    PVOID hVeh = nullptr;
    HANDLE hThread = nullptr;
    bool bDebugged = false;

    __try
    {
        hVeh = AddVectoredExceptionHandler(TRUE, InstructionCountingExceptionHandler);
        if (!hVeh)
            __leave;

        hThread = CreateThread(0, 0, InstructionCountingFunc, NULL, CREATE_SUSPENDED, 0);
        if (!hThread)
            __leave;

        PVOID pThreadAddr = (PVOID)&InstructionCountingFunc;
        // Fix thread entry address if it is a JMP stub (E9 XX XX XX XX)
        if (*(PBYTE)pThreadAddr == 0xE9)
            pThreadAddr = (PVOID)((uintptr_t)pThreadAddr + 5 + *(PDWORD)((PBYTE)pThreadAddr + 1));

        for (auto i = 0; i < m_nInstructionCount; i++)
            m_hHwBps[i] = SetHardwareBreakpoint(
                hThread, HWBRK_TYPE_CODE, HWBRK_SIZE_1, (PVOID)((uintptr_t)pThreadAddr + 2 + i));

        ResumeThread(hThread);
        WaitForSingleObject(hThread, INFINITE);

        DWORD dwThreadExitCode;
        if (GetExitCodeThread(hThread, &dwThreadExitCode) == TRUE)
            bDebugged = (dwThreadExitCode == TRUE);
    }
    __finally
    {
        for (int i = 0; i < 4; i++)
        {
            if (m_hHwBps[i])
            {
                RemoveHardwareBreakpoint(m_hHwBps[i]);
                m_hHwBps[i] = nullptr;
            }
        }

        if (hThread)
            CloseHandle(hThread);

        if (hVeh)
            RemoveVectoredExceptionHandler(hVeh);
    }

    return bDebugged;
}

int main()
{
    std::cout << "Checking for debugger (instruction counting)...\n";

    if (IsDebugged())
        std::cout << "Debugger detected!\n";
    else
        std::cout << "No debugger detected.\n";

    return 0;
}

/*
 * Debugger detection via INT 3 with exception filter.
 * filter(): sets g_bDebugged true only when the exception was NOT a breakpoint
 * (breakpoint = debugger let it through / no debugger = we get breakpoint in SEH).
 * IsDebugged(): triggers INT 3; if we hit the except block, returns g_bDebugged.
 */

#include <windows.h>
#include <iostream>

bool g_bDebugged = false;

int filter(unsigned int code, struct _EXCEPTION_POINTERS* ep)
{
    g_bDebugged = (code != EXCEPTION_BREAKPOINT);
    return EXCEPTION_EXECUTE_HANDLER;
}

bool IsDebugged()
{
    __try
    {
        __asm __emit(0xCD);
        __asm __emit(0x03);
    }
    __except (filter(GetExceptionCode(), GetExceptionInformation()))
    {
        return g_bDebugged;
    }
    /* No exception: debugger handled the breakpoint */
    return true;
}

int main()
{
    std::cout << "Checking for debugger...\n";

    if (IsDebugged())
        std::cout << "Debugger detected!\n";
    else
        std::cout << "No debugger detected.\n";

    return 0;
}

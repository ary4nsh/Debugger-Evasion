/*
 * Debugger detection via instruction prefixes (REP + segment) and ICEBP.
 * 0xF3 0x64 disassembles as PREFIX REP; 0xF1 = ICEBP (INT1). Some debuggers
 * mis-handle the prefix sequence. If a debugger handles the trap we return true.
 */

#include <windows.h>
#include <iostream>

bool IsDebugged()
{
    __try
    {
        __asm __emit 0xF3
        __asm __emit 0x64
        __asm __emit 0xF1
        return true;
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        return false;
    }
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

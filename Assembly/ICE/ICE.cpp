/*
 * Debugger detection via ICEBP (INT 1 / opcode 0xF1).
 * If a debugger is attached, it may handle the trap and execution continues (returns true).
 * If no debugger is present, SEH catches the exception (returns false).
 */

#include <windows.h>
#include <iostream>

bool IsDebugged()
{
    __try
    {
        __asm __emit 0xF1;
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

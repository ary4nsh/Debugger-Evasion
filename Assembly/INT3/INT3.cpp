/*
 * Debugger detection via software breakpoint (INT 3).
 * If a debugger is attached, it handles the breakpoint and execution continues (returns true).
 * If no debugger is present, SEH catches the exception (returns false).
 */

#include <windows.h>
#include <iostream>

bool IsDebugged()
{
    __try
    {
        __asm int 3;
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

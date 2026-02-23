/*
 * Debugger detection via DebugBreak() (raises EXCEPTION_BREAKPOINT).
 * If a debugger is attached, it handles the breakpoint and execution continues (returns true).
 * If no debugger is present, SEH catches the breakpoint exception (returns false).
 */

#include <windows.h>
#include <iostream>

bool IsDebugged()
{
    __try
    {
        DebugBreak();
    }
    __except (GetExceptionCode() == EXCEPTION_BREAKPOINT ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH)
    {
        return false;
    }

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

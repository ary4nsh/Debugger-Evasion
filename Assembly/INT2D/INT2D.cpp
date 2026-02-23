/*
 * Debugger detection via INT 2D (kernel debugger service on Windows).
 * If a debugger is attached, it may handle INT 2D and execution continues (returns true).
 * If no debugger is present, an exception is raised and SEH catches it (returns false).
 */

#include <windows.h>
#include <iostream>

bool IsDebugged()
{
    __try
    {
        __asm xor eax, eax;
        __asm int 0x2d;     // INT 2D instruction
        __asm nop;
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

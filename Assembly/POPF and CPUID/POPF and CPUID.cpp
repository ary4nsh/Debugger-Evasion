/*
 * Debugger detection via pushfd/popfd and CPUID.
 * pushfd/popfd can interact with TF; CPUID is serializing. C7 B2 emitted after.
 * If a debugger handles single-step we return true; else we catch and return false.
 */

#include <windows.h>
#include <iostream>

bool IsDebugged()
{
    __try
    {
        __asm
        {
            pushfd
            popfd
            cpuid
            __emit 0xC7
            __emit 0xB2
        }
        return true;
    }
    __except (GetExceptionCode() == EXCEPTION_SINGLE_STEP
        ? EXCEPTION_EXECUTE_HANDLER
        : EXCEPTION_CONTINUE_SEARCH)
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

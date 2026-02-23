/*
 * Debugger detection via Trap Flag (TF in EFLAGS).
 * pushfd / set TF (0x100) on stack / popfd: next instruction runs in single-step.
 * If a debugger is attached, it may handle EXCEPTION_SINGLE_STEP (returns true).
 * If no debugger, we catch single-step and return false.
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
            mov dword ptr [esp], 0x100
            popfd
            nop
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

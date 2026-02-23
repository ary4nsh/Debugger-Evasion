/*
 * Debugger detection via push SS / pop SS (TF trap).
 * After push ss; pop ss, the next instruction runs with TF set; pushf then
 * checks the Trap Flag. If TF is set we were single-stepped (debugger).
 */

#include <windows.h>
#include <iostream>

bool IsDebugged()
{
    bool bTraced = false;

    __asm
    {
        push ss
        pop ss
        pushf
        test byte ptr [esp+1], 1
        jz movss_not_being_debugged
    }

    bTraced = true;

movss_not_being_debugged:
    __asm popf;

    return bTraced;
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

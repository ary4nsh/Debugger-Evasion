/*
 * Debugger check using Windows API CheckRemoteDebuggerPresent().
 * Exits with -1 if a debugger is attached to the current process.
 */

#include <windows.h>

int main()
{
    // bDebuggerPresent: Pointer to a BOOL that receives TRUE if a debugger is attached, FALSE otherwise
    BOOL bDebuggerPresent = FALSE;

    if (TRUE == CheckRemoteDebuggerPresent(
            GetCurrentProcess(),     // hProcess: Handle to the process to check (current process)
            &bDebuggerPresent        // bDebuggerPresent: Pointer to a variable that receives the result
        ) &&
        TRUE == bDebuggerPresent)
        // ExitProcess: Terminates the process with exit code -1
        ExitProcess((UINT)-1);

    return 0;
}

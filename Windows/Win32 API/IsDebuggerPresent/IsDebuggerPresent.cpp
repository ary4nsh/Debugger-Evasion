/*
 * Debugger check using Windows API IsDebuggerPresent().
 * Exits with -1 if a debugger is attached.
 */

#include <windows.h>

int main()
{
    // IsDebuggerPresent(): Returns nonzero if the current process is being debugged, otherwise 0
    if (IsDebuggerPresent())
        // ExitProcess: Terminates the process. (UINT)-1 is the exit code passed to the system
        ExitProcess((UINT)-1);

    // No debugger; continue normally
    return 0;
}

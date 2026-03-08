/*
 * Debugger check using ptrace(PTRACE_TRACEME) on Linux.
 * Only one tracer can attach per process; if we are already being traced (e.g. by a debugger),
 * PTRACE_TRACEME fails and we detect the debugger.
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/ptrace.h>

int main()
{
    // ptrace(request, pid, addr, data): PTRACE_TRACEME tells the kernel this process is to be traced by its parent
    if (ptrace(
            PTRACE_TRACEME,   // request: PTRACE_TRACEME = request that the parent trace this process; fails if already traced
            0,                 // pid: Ignored for PTRACE_TRACEME (use 0)
            NULL,              // addr: Ignored for PTRACE_TRACEME
            0                  // data: Ignored for PTRACE_TRACEME
        ) == -1)
    {
        printf("Debugger detected, exiting...\n");
        exit(1);
    }
    printf("Debugger is not present");
    return 0;
}

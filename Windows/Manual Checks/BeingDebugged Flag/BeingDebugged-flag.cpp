/*
 * Debugger check using the PEB (Process Environment Block) BeingDebugged flag.
 * On x86 the PEB pointer is at fs:[0x30]; on x64 at gs:[0x60].
 * BeingDebugged is a byte at offset 2 in the PEB; set when a debugger is attached.
 */

#include <windows.h>

// Minimal PEB layout: BeingDebugged is at offset 2 (after two reserved bytes)
typedef struct _PEB {
    BYTE Reserved1[2];   // Reserved1: Padding so BeingDebugged is at offset 0x2
    BYTE BeingDebugged;  // BeingDebugged: 1 if a debugger is attached, 0 otherwise
} PEB, *PPEB;

int main()
{
    PPEB pPeb;  // pPeb: Pointer to the Process Environment Block for the current process

#ifndef _WIN64
    // x86: PEB pointer is stored at fs:[0x30]
    pPeb = (PPEB)__readfsdword(0x30);   // 0x30: Offset in the TEB (Thread Environment Block) where the PEB pointer is stored
#else
    // x64: PEB pointer is stored at gs:[0x60]
    pPeb = (PPEB)__readgsqword(0x60);   // 0x60: Offset in the TEB where the PEB pointer is stored
#endif // _WIN64

    if (pPeb->BeingDebugged)  // BeingDebugged: Non-zero means a debugger is attached to this process
        goto being_debugged;

    return 0;

being_debugged:
    ExitProcess((UINT)-1);   // Exit with -1 when debugger detected
}

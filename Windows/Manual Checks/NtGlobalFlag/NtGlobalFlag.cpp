/*
 * Debugger check using the PEB NtGlobalFlag.
 * When a debugger is attached, Windows sets heap-checking flags in NtGlobalFlag.
 * On x86 NtGlobalFlag is at PEB+0x68; on x64 at PEB+0xBC.
 */

#include <windows.h>

// Heap flags set by the system when a debugger is attached
#define FLG_HEAP_ENABLE_TAIL_CHECK   0x10   // FLG_HEAP_ENABLE_TAIL_CHECK: Enable tail checking on heap blocks
#define FLG_HEAP_ENABLE_FREE_CHECK   0x20   // FLG_HEAP_ENABLE_FREE_CHECK: Enable free checking on heap blocks
#define FLG_HEAP_VALIDATE_PARAMETERS 0x40   // FLG_HEAP_VALIDATE_PARAMETERS: Validate heap parameters
// NT_GLOBAL_FLAG_DEBUGGED: Combination of flags that indicate the process is being debugged
#define NT_GLOBAL_FLAG_DEBUGGED (FLG_HEAP_ENABLE_TAIL_CHECK | FLG_HEAP_ENABLE_FREE_CHECK | FLG_HEAP_VALIDATE_PARAMETERS)

// PEB: Process Environment Block; we only need a pointer to read NtGlobalFlag at a fixed offset
typedef void* PPEB;

int main()
{
    PPEB pPeb;           // pPeb: Pointer to the Process Environment Block for the current process
    DWORD dwNtGlobalFlag; // dwNtGlobalFlag: NtGlobalFlag value; when debugger is attached, debug-related bits are set

#ifndef _WIN64
    // x86: PEB pointer at fs:[0x30]; NtGlobalFlag at PEB+0x68
    pPeb = (PPEB)__readfsdword(0x30);   // 0x30: Offset in the TEB where the PEB pointer is stored
    dwNtGlobalFlag = *(PDWORD)((PBYTE)pPeb + 0x68);   // 0x68: Offset of NtGlobalFlag in the PEB (x86)
#else
    // x64: PEB pointer at gs:[0x60]; NtGlobalFlag at PEB+0xBC
    pPeb = (PPEB)__readgsqword(0x60);   // 0x60: Offset in the TEB where the PEB pointer is stored
    dwNtGlobalFlag = *(PDWORD)((PBYTE)pPeb + 0xBC);   // 0xBC: Offset of NtGlobalFlag in the PEB (x64)
#endif // _WIN64

    if (dwNtGlobalFlag & NT_GLOBAL_FLAG_DEBUGGED)  // If any debugger-related heap flags are set, a debugger is attached
        goto being_debugged;

    return 0;

being_debugged:
    ExitProcess((UINT)-1);   // Exit with -1 when debugger detected
}

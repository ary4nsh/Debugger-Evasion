/*
 * Debugger check using process heap flags.
 * When a debugger is attached, the heap's Flags and ForceFlags may have extra bits set.
 * PEB and heap layout differ for x86/x64 and Vista+ vs older Windows.
 */

#include <windows.h>
#include <VersionHelpers.h>

#ifndef HEAP_GROWABLE
#define HEAP_GROWABLE 0x2
#endif

// PEB: Process Environment Block; we use it only as a pointer to read offsets
typedef void* PPEB;

// Check: Returns true if heap flags indicate a debugger is attached (extra flags or non-zero ForceFlags)
bool Check()
{
    BOOL bIsWow64 = FALSE;  // bIsWow64: TRUE if this process is 32-bit running on 64-bit Windows (WOW64)
    IsWow64Process(
        GetCurrentProcess(),   // hProcess: Handle to the process to query (current process)
        &bIsWow64              // Wow64Process: Pointer to a BOOL that receives TRUE if the process is WOW64
    );

#ifndef _WIN64
    // x86: PEB at fs:[0x30]; ProcessHeap at PEB+0x18 (native) or PEB+0x1030 (WOW64)
    PPEB pPeb = (PPEB)__readfsdword(0x30);   // 0x30: Offset in the TEB where the PEB pointer is stored
    PVOID pHeapBase = !bIsWow64
        ? (PVOID)(*(PDWORD_PTR)((PBYTE)pPeb + 0x18))    // 0x18: Offset of ProcessHeap in the PEB (native 32-bit)
        : (PVOID)(*(PDWORD_PTR)((PBYTE)pPeb + 0x1030)); // 0x1030: Offset of ProcessHeap in the PEB (WOW64)
    DWORD dwHeapFlagsOffset = IsWindowsVistaOrGreater()
        ? 0x40   // Vista+: Heap Flags field offset in the heap segment
        : 0x0C;  // Pre-Vista: Heap Flags field offset
    DWORD dwHeapForceFlagsOffset = IsWindowsVistaOrGreater()
        ? 0x44   // Vista+: Heap ForceFlags field offset
        : 0x10;  // Pre-Vista: Heap ForceFlags field offset
#else
    // x64: PEB at gs:[0x60]; ProcessHeap at PEB+0x30
    PPEB pPeb = (PPEB)__readgsqword(0x60);   // 0x60: Offset in the TEB where the PEB pointer is stored
    PVOID pHeapBase = (PVOID)(*(PDWORD_PTR)((PBYTE)pPeb + 0x30));   // 0x30: Offset of ProcessHeap in the PEB (x64)
    DWORD dwHeapFlagsOffset = IsWindowsVistaOrGreater()
        ? 0x70   // Vista+: Heap Flags field offset in the heap segment (x64)
        : 0x14;  // Pre-Vista: Heap Flags field offset (x64)
    DWORD dwHeapForceFlagsOffset = IsWindowsVistaOrGreater()
        ? 0x74   // Vista+: Heap ForceFlags field offset (x64)
        : 0x18;  // Pre-Vista: Heap ForceFlags field offset (x64)
#endif // _WIN64

    // Pointers into the process heap: Flags and ForceFlags; when debugger is attached, extra bits may be set
    PDWORD pdwHeapFlags = (PDWORD)((PBYTE)pHeapBase + dwHeapFlagsOffset);       // pdwHeapFlags: Pointer to heap Flags (HEAP_GROWABLE is normal)
    PDWORD pdwHeapForceFlags = (PDWORD)((PBYTE)pHeapBase + dwHeapForceFlagsOffset); // pdwHeapForceFlags: Pointer to heap ForceFlags; non-zero when debugger attached
    return (*pdwHeapFlags & ~HEAP_GROWABLE) || (*pdwHeapForceFlags != 0);
}

int main()
{
    if (Check())
        ExitProcess((UINT)-1);
    return 0;
}

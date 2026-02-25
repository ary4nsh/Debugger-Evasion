/*
 * Debugger check using RtlCreateQueryDebugBuffer and RtlQueryProcessHeapInformation.
 * When a debugger is attached, the process heap may have extra flags set (e.g. tail/free checking).
 * If (Flags & ~HEAP_GROWABLE) is non-zero, a debugger is likely present.
 */

#include <windows.h>

#ifndef HEAP_GROWABLE
#define HEAP_GROWABLE 0x2
#endif

// RTL_HEAP_INFORMATION: Heap entry with Flags; debugger sets extra flags (e.g. HEAP_TAIL_CHECKING_ENABLED)
typedef struct _RTL_HEAP_INFORMATION {
    PVOID BaseAddress;       // BaseAddress: Base address of the heap
    ULONG Flags;             // Flags: Heap flags; HEAP_GROWABLE (0x2) is normal; extra bits indicate debugger
    USHORT EntryOverhead;
    USHORT CreatorBackTraceIndex;
    SIZE_T BytesAllocated;
    SIZE_T BytesCommitted;
    ULONG NumberOfTags;
    ULONG NumberOfEntries;
    ULONG NumberOfPseudoTags;
    ULONG PseudoTagGranularity;
    ULONG Reserved[5];
    PVOID Tags;
    PVOID Entries;
} RTL_HEAP_INFORMATION, *PRTL_HEAP_INFORMATION;

// RTL_PROCESS_HEAPS: Contains the number of heaps and an array of heap information
typedef struct _RTL_PROCESS_HEAPS {
    ULONG NumberOfHeaps;                    // NumberOfHeaps: Count of heaps in the process
    RTL_HEAP_INFORMATION Heaps[1];          // Heaps: Array of heap information (variable length)
} RTL_PROCESS_HEAPS, *PRTL_PROCESS_HEAPS;

// RTL_DEBUG_INFORMATION (DEBUG_BUFFER): Buffer for debug queries; HeapInformation receives heap data
typedef struct _RTL_DEBUG_INFORMATION {
    HANDLE SectionHandleClient;
    PVOID ViewBaseClient;
    PVOID ViewBaseTarget;
    ULONG_PTR ViewBaseDelta;
    HANDLE EventPairClient;
    HANDLE EventPairTarget;
    HANDLE TargetProcessId;
    HANDLE TargetThreadHandle;
    ULONG Flags;
    SIZE_T OffsetFree;
    SIZE_T CommitSize;
    SIZE_T ViewSize;
    PVOID Modules;
    PVOID BackTraces;
    PRTL_PROCESS_HEAPS HeapInformation;     // HeapInformation: Filled by RtlQueryProcessHeapInformation with heap data
    PVOID Locks;
    PVOID SpecificHeap;
    HANDLE TargetProcessHandle;
    PVOID VerifierOptions;
    PVOID ProcessHeap;
    HANDLE CriticalSectionHandle;
    HANDLE CriticalSectionOwnerThread;
    PVOID Reserved[4];
} RTL_DEBUG_INFORMATION, *PRTL_DEBUG_INFORMATION;

namespace ntdll {
    typedef PRTL_DEBUG_INFORMATION PDEBUG_BUFFER;
    typedef ::PRTL_PROCESS_HEAPS PRTL_PROCESS_HEAPS;

    // RtlCreateQueryDebugBuffer: Creates a buffer for process debug/heap queries
    typedef PRTL_DEBUG_INFORMATION(NTAPI* pRtlCreateQueryDebugBuffer)(ULONG MaximumCommit, BOOLEAN UseEventPair);
    // RtlQueryProcessHeapInformation: Fills the buffer with process heap information (undocumented)
    typedef LONG(NTAPI* pRtlQueryProcessHeapInformation)(PRTL_DEBUG_INFORMATION DebugBuffer);
    // RtlDestroyQueryDebugBuffer: Frees the buffer created by RtlCreateQueryDebugBuffer
    typedef LONG(NTAPI* pRtlDestroyQueryDebugBuffer)(PRTL_DEBUG_INFORMATION DebugBuffer);
}

// Check: Returns true if no debugger (heap flags look normal), false if query failed or debugger detected
bool Check()
{
    // LoadLibraryA: Loads the ntdll module so we can resolve the Rtl* function addresses
    HMODULE hNtdll = LoadLibraryA("ntdll.dll");  // lpLibFileName: Name of the module to load
    if (!hNtdll)
        return false;

    // GetProcAddress: Retrieves the address of each exported function from ntdll
    auto pRtlCreate = (ntdll::pRtlCreateQueryDebugBuffer)GetProcAddress(
        hNtdll,                         // hModule: Handle to the loaded ntdll module
        "RtlCreateQueryDebugBuffer"     // lpProcName: Name of the exported function
    );
    auto pRtlQueryHeap = (ntdll::pRtlQueryProcessHeapInformation)GetProcAddress(
        hNtdll,                             // hModule: Handle to the loaded ntdll module
        "RtlQueryProcessHeapInformation"   // lpProcName: Name of the exported function
    );
    auto pRtlDestroy = (ntdll::pRtlDestroyQueryDebugBuffer)GetProcAddress(
        hNtdll,                         // hModule: Handle to the loaded ntdll module
        "RtlDestroyQueryDebugBuffer"    // lpProcName: Name of the exported function
    );

    // If any of the three function pointers could not be resolved, unload ntdll and return false
    if (!pRtlCreate || !pRtlQueryHeap || !pRtlDestroy)
    {
        FreeLibrary(hNtdll);    // hLibModule: Handle to the loaded ntdll module to free
        return false;
    }

    // RtlCreateQueryDebugBuffer(MaximumCommit, UseEventPair)
    ntdll::PDEBUG_BUFFER pDebugBuffer = pRtlCreate(
        0,      // MaximumCommit: 0 = use default commit size for the buffer
        FALSE   // UseEventPair: FALSE = do not create an event pair for the query
    );
    if (!pDebugBuffer)  // pDebugBuffer: Pointer to the debug buffer; NULL if allocation failed
    {
        FreeLibrary(hNtdll);    // hLibModule: Unload ntdll before returning
        return false;           // Buffer creation failed; treat as no debugger (or error)
    }

    // RtlQueryProcessHeapInformation(DebugBuffer): Fills DebugBuffer->HeapInformation with heap data; returns NTSTATUS
    if (pRtlQueryHeap(
            (ntdll::PDEBUG_BUFFER)pDebugBuffer   // DebugBuffer: Buffer created by RtlCreateQueryDebugBuffer, receives heap information
        ) < 0)
    {
        pRtlDestroy(pDebugBuffer);   // Buffer: Debug buffer to free (query failed)
        FreeLibrary(hNtdll);         // hLibModule: Unload ntdll before returning
        return false;                // Query failed; treat as no debugger (or error)
    }

    // Heaps[0]: First process heap; Flags: HEAP_GROWABLE is normal; other bits set when debugger attaches
    ULONG dwFlags = ((ntdll::PRTL_PROCESS_HEAPS)pDebugBuffer->HeapInformation)->Heaps[0].Flags;
    // return: true if any bits other than HEAP_GROWABLE are set (debugger present), else false
    pRtlDestroy(pDebugBuffer);   // Buffer: Debug buffer to free after reading heap flags
    FreeLibrary(hNtdll);         // hLibModule: Unload ntdll before returning
    return (dwFlags & ~HEAP_GROWABLE) != 0;
}

int main()
{
    // If Check() returns true (debugger detected), exit with -1
    if (Check())
        ExitProcess((UINT)-1);
    return 0;
}

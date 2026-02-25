/*
 * Debugger check using HeapWalk and heap block tail.
 * When a debugger is attached, the heap may use a fill pattern (e.g. 0xABABABAB) after busy blocks.
 * We walk the heap until we find a busy block, then check the overlapped/tail DWORD.
 */

#include <windows.h>

// Check: Returns true if the first busy block's tail contains the debugger fill pattern (0xABABABAB)
bool Check()
{
    PROCESS_HEAP_ENTRY HeapEntry = { 0 };   // HeapEntry: Receives heap block info from HeapWalk; lpData, cbData, wFlags used

    do
    {
        if (!HeapWalk(
                GetProcessHeap(),   // hHeap: Handle to the process heap to walk
                &HeapEntry          // lpEntry: Pointer to a PROCESS_HEAP_ENTRY that receives info about the next block
            ))
            return false;
    } while (HeapEntry.wFlags != PROCESS_HEAP_ENTRY_BUSY);   // wFlags: PROCESS_HEAP_ENTRY_BUSY = block is in use; keep walking until we find one

    // pOverlapped: Address immediately after the block (lpData + cbData); debugger fill pattern may be written here
    PVOID pOverlapped = (PBYTE)HeapEntry.lpData + HeapEntry.cbData;
    // 0xABABABAB: Fill pattern used by the heap when a debugger is attached; if present, debugger detected
    return ((DWORD)(*(PDWORD)pOverlapped) == 0xABABABAB);
}

int main()
{
    if (Check())
        ExitProcess((UINT)-1);
    return 0;
}

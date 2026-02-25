/*
 * Debugger check using NtQueryInformationProcess(ProcessDebugPort).
 * If the process has a debug port (-1), exits with -1.
 */

#include <windows.h>

// NT_SUCCESS: Macro that evaluates to TRUE if the NTSTATUS value indicates success (>= 0)
#ifndef NT_SUCCESS
#define NT_SUCCESS(s) (((NTSTATUS)(s)) >= 0)
#endif

// NTSTATUS: Long integer status code returned by NT API functions
typedef LONG NTSTATUS;

// PROCESSINFOCLASS: Enumeration of the kinds of process information that NtQueryInformationProcess can return
typedef enum _PROCESSINFOCLASS {
    ProcessDebugPort = 7  // Value 7: query the debug port; nonzero or -1 means a debugger is attached
} PROCESSINFOCLASS;

// TNtQueryInformationProcess: Type for the NtQueryInformationProcess function pointer (undocumented NT API)
typedef NTSTATUS(NTAPI* TNtQueryInformationProcess)(
    IN HANDLE ProcessHandle,              // Handle to the process to query (e.g. current process)
    IN PROCESSINFOCLASS ProcessInformationClass,  // Class of information to retrieve (e.g. ProcessDebugPort)
    OUT PVOID ProcessInformation,        // Buffer that receives the requested information
    IN ULONG ProcessInformationLength,   // Size in bytes of the output buffer
    OUT PULONG ReturnLength               // Pointer to a variable that receives the size of the returned data
);

int main()
{
    // LoadLibraryA: Loads ntdll.dll so we can get the address of NtQueryInformationProcess
    HMODULE hNtdll = LoadLibraryA("ntdll.dll");
    if (!hNtdll)
        return 0;

    // GetProcAddress: Retrieves the address of NtQueryInformationProcess from ntdll
    auto pfnNtQueryInformationProcess = (TNtQueryInformationProcess)GetProcAddress(
        hNtdll,                         // hModule: Handle to the loaded ntdll module
        "NtQueryInformationProcess"     // lpProcName: Name of the exported function
    );

    if (pfnNtQueryInformationProcess)
    {
        // dwProcessDebugPort: Receives the debug port; -1 (0xFFFFFFFF) or nonzero means debugger present
        DWORD dwProcessDebugPort = 0;
        // dwReturned: Receives the number of bytes written to the output buffer
        ULONG dwReturned = 0;
        NTSTATUS status = pfnNtQueryInformationProcess(
            GetCurrentProcess(),     // ProcessHandle: Handle to the current process
            ProcessDebugPort,        // ProcessInformationClass: Request debug port information
            &dwProcessDebugPort,    // ProcessInformation: Buffer that receives the debug port value
            sizeof(DWORD),           // ProcessInformationLength: Size of the buffer (4 bytes)
            &dwReturned              // ReturnLength: Receives the size of the returned data
        );

        // If the call succeeded and the debug port is -1, a debugger is attached
        if (NT_SUCCESS(status) && (DWORD)-1 == dwProcessDebugPort)
            ExitProcess((UINT)-1);
    }

    // FreeLibrary: Unloads ntdll.dll (optional cleanup)
    FreeLibrary(hNtdll);
    return 0;
}

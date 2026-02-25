/*
 * Debugger check using NtQueryInformationProcess(ProcessDebugFlags).
 * If the process debug flags are 0 (NoDebugInherit clear), a debugger is present; exit with -1.
 */

#include <windows.h>

// NT_SUCCESS: Macro that evaluates to TRUE if the NTSTATUS value indicates success (>= 0)
#ifndef NT_SUCCESS
#define NT_SUCCESS(s) (((NTSTATUS)(s)) >= 0)
#endif

// NTSTATUS: Long integer status code returned by NT API functions
typedef LONG NTSTATUS;

// TNtQueryInformationProcess: Type for the NtQueryInformationProcess function pointer (undocumented NT API)
typedef NTSTATUS(NTAPI* TNtQueryInformationProcess)(
    IN HANDLE ProcessHandle,              // ProcessHandle: Handle to the process to query (e.g. current process)
    IN DWORD ProcessInformationClass,     // ProcessInformationClass: Class of information to retrieve (e.g. ProcessDebugFlags = 0x1f)
    OUT PVOID ProcessInformation,          // ProcessInformation: Buffer that receives the requested information
    IN ULONG ProcessInformationLength,    // ProcessInformationLength: Size in bytes of the output buffer
    OUT PULONG ReturnLength               // ReturnLength: Pointer to a variable that receives the size of the returned data
);

int main()
{
    // LoadLibraryA: Loads ntdll.dll so we can get the address of NtQueryInformationProcess
    HMODULE hNtdll = LoadLibraryA("ntdll.dll");
    if (hNtdll)
    {
        // GetProcAddress: Retrieves the address of NtQueryInformationProcess from ntdll
        auto pfnNtQueryInformationProcess = (TNtQueryInformationProcess)GetProcAddress(
            hNtdll,                         // hModule: Handle to the loaded ntdll module
            "NtQueryInformationProcess"     // lpProcName: Name of the exported function
        );

        if (pfnNtQueryInformationProcess)
        {
            // dwProcessDebugFlags: Receives the process debug flags; 0 means NoDebugInherit is not set (debugger present)
            DWORD dwProcessDebugFlags = 0;
            // dwReturned: Receives the number of bytes written to the output buffer
            DWORD dwReturned = 0;
            // ProcessDebugFlags: Information class value 0x1f to query the process debug flags
            const DWORD ProcessDebugFlags = 0x1f;
            NTSTATUS status = pfnNtQueryInformationProcess(
                GetCurrentProcess(),     // ProcessHandle: Handle to the current process
                ProcessDebugFlags,       // ProcessInformationClass: Request process debug flags (0x1f)
                &dwProcessDebugFlags,    // ProcessInformation: Buffer that receives the debug flags value
                sizeof(DWORD),           // ProcessInformationLength: Size of the buffer (4 bytes)
                &dwReturned              // ReturnLength: Receives the size of the returned data
            );

            // If the call succeeded and debug flags are 0, a debugger is attached (NoDebugInherit is clear)
            if (NT_SUCCESS(status) && (0 == dwProcessDebugFlags))
                ExitProcess((UINT)-1);
        }

        // FreeLibrary: Unloads ntdll.dll (optional cleanup)
        FreeLibrary(hNtdll);
    }
    return 0;
}

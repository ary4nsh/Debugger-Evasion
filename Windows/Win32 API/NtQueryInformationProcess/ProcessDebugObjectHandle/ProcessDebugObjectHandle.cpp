/*
 * Debugger check using NtQueryInformationProcess(ProcessDebugObjectHandle).
 * If the process has a non-zero debug object handle, a debugger is attached; exit with -1.
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
    IN DWORD ProcessInformationClass,     // ProcessInformationClass: Class of information to retrieve (e.g. ProcessDebugObjectHandle = 0x1e)
    OUT PVOID ProcessInformation,        // ProcessInformation: Buffer that receives the requested information
    IN ULONG ProcessInformationLength,   // ProcessInformationLength: Size in bytes of the output buffer
    OUT PULONG ReturnLength              // ReturnLength: Pointer to a variable that receives the size of the returned data
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
            // dwReturned: Receives the number of bytes written to the output buffer
            DWORD dwReturned = 0;
            // hProcessDebugObject: Receives the process debug object handle; non-zero means a debugger is attached
            HANDLE hProcessDebugObject = 0;
            // ProcessDebugObjectHandle: Information class value 0x1e to query the process debug object handle
            const DWORD ProcessDebugObjectHandle = 0x1e;
            NTSTATUS status = pfnNtQueryInformationProcess(
                GetCurrentProcess(),      // ProcessHandle: Handle to the current process
                ProcessDebugObjectHandle, // ProcessInformationClass: Request process debug object handle (0x1e)
                &hProcessDebugObject,     // ProcessInformation: Buffer that receives the debug object handle
                sizeof(HANDLE),           // ProcessInformationLength: Size of the buffer (size of HANDLE)
                &dwReturned               // ReturnLength: Receives the size of the returned data
            );

            // If the call succeeded and the debug object handle is non-zero, a debugger is attached
            if (NT_SUCCESS(status) && (0 != hProcessDebugObject))
                ExitProcess((UINT)-1);
        }

        // FreeLibrary: Unloads ntdll.dll (optional cleanup)
        FreeLibrary(hNtdll);
    }
    return 0;
}

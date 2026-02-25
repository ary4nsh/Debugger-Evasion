/*
 * Debugger check using NtQuerySystemInformation(SystemKernelDebuggerInformation).
 * Detects if a kernel debugger is enabled and present.
 */

#include <windows.h>

// NTSTATUS: Long integer status code returned by NT API functions
typedef LONG NTSTATUS;

// SYSTEM_INFORMATION_CLASS: Enumeration of system information classes for NtQuerySystemInformation
enum { SystemKernelDebuggerInformation = 0x23 };  // SystemKernelDebuggerInformation: Query kernel debugger state

// SYSTEM_KERNEL_DEBUGGER_INFORMATION: Receives kernel debugger presence and enabled state
typedef struct _SYSTEM_KERNEL_DEBUGGER_INFORMATION {
    BOOLEAN DebuggerEnabled;    // DebuggerEnabled: TRUE if a kernel debugger is enabled
    BOOLEAN DebuggerNotPresent; // DebuggerNotPresent: TRUE if no kernel debugger is present
} SYSTEM_KERNEL_DEBUGGER_INFORMATION, *PSYSTEM_KERNEL_DEBUGGER_INFORMATION;

// TNtQuerySystemInformation: Type for the NtQuerySystemInformation function pointer (undocumented NT API)
typedef NTSTATUS(NTAPI* TNtQuerySystemInformation)(
    IN ULONG SystemInformationClass,   // SystemInformationClass: Class of information to retrieve (e.g. SystemKernelDebuggerInformation)
    OUT PVOID SystemInformation,      // SystemInformation: Buffer that receives the requested information
    IN ULONG SystemInformationLength,  // SystemInformationLength: Size in bytes of the output buffer
    OUT PULONG ReturnLength           // ReturnLength: Optional; receives the size of the returned data (may be NULL)
);

// Check: Returns true if a kernel debugger is enabled and present, false otherwise
bool Check()
{
    NTSTATUS status;                                    // status: Return value from NtQuerySystemInformation
    SYSTEM_KERNEL_DEBUGGER_INFORMATION SystemInfo;      // SystemInfo: Receives DebuggerEnabled and DebuggerNotPresent

    HMODULE hNtdll = LoadLibraryA("ntdll.dll");  // lpLibFileName: Name of the module to load
    if (!hNtdll)
        return false;

    auto pNtQuerySystemInformation = (TNtQuerySystemInformation)GetProcAddress(
        hNtdll,                         // hModule: Handle to the loaded ntdll module
        "NtQuerySystemInformation"     // lpProcName: Name of the exported function
    );
    if (!pNtQuerySystemInformation)  // Function not found in ntdll
    {
        FreeLibrary(hNtdll);    // hLibModule: Unload ntdll before returning
        return false;
    }

    // NtQuerySystemInformation(SystemInformationClass, SystemInformation, SystemInformationLength, ReturnLength)
    status = pNtQuerySystemInformation(
        (ULONG)SystemKernelDebuggerInformation,  // SystemInformationClass: Request kernel debugger information (0x23)
        &SystemInfo,                              // SystemInformation: Buffer that receives SYSTEM_KERNEL_DEBUGGER_INFORMATION
        sizeof(SystemInfo),                       // SystemInformationLength: Size of the buffer in bytes
        NULL                                     // ReturnLength: NULL = do not return the size of the data written
    );

    FreeLibrary(hNtdll);    // hLibModule: Unload ntdll before returning

    // If the call succeeded, kernel debugger is detected when Enabled and not NotPresent
    return SUCCEEDED(status)
        ? (SystemInfo.DebuggerEnabled && !SystemInfo.DebuggerNotPresent)
        : false;
}

int main()
{
    // If Check() returns true (kernel debugger detected), exit with -1
    if (Check())
        ExitProcess((UINT)-1);
    return 0;
}

# NtQuerySystemInformation

The `ntdll!NtQuerySystemInformation` function can be utilized to query various classes of information related to the system, including undocumented classes such as `SystemKernelDebuggerInformation` (0x23). This class has been a part of the Windows NT architecture since its inception. This function provides critical information for developers focused on kernel-level debugging, allowing them to understand the current debugging state and tailor their applications accordingly.

When querying the `SystemKernelDebuggerInformation` class, the function returns two flags:
- `KdDebuggerEnabled` in the AL register, indicating whether the kernel debugger is enabled.
- `KdDebuggerNotPresent` in the AH register, where a value of 0 indicates that a kernel debugger is currently present.
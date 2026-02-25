# NtQueryInformationProcess: ProcessDebugFlags

The `ntdll!NtQueryInformationProcess` function can also be utilized to retrieve information related to the `NoDebugInherit` field from the `EPROCESS` kernel structure using the undocumented `ProcessDebugFlags` class (0x1f). This field is vital for determining the presence of a debugger. This functionality provides developers with a method to assess debugger attachment and implement appropriate security measures in their applications, enhancing protection against debugging and reverse engineering. The assembly code for this function is:

- x86 Assembly
```asm
    lea eax, [dwReturned]
    push eax ; ReturnLength
    push 4   ; ProcessInformationLength
    lea ecx, [dwProcessDebugPort]
    push ecx ; ProcessInformation
    push 1Fh ; ProcessInformationClass
    push -1  ; ProcessHandle
    call NtQueryInformationProcess
    cmp dword ptr [dwProcessDebugPort], 0
    jz being_debugged
    ...
being_debugged:
    push -1
    call ExitProcess
```

- x86-64 Assembly
```asm
    lea rcx, [dwReturned]
    push rcx     ; ReturnLength
    mov r9d, 4   ; ProcessInformationLength
    lea r8, [dwProcessDebugPort] 
                 ; ProcessInformation
    mov edx, 1Fh ; ProcessInformationClass
    mov rcx, -1  ; ProcessHandle
    call NtQueryInformationProcess
    cmp dword ptr [dwProcessDebugPort], 0
    jz being_debugged
    ...
being_debugged:
    mov ecx, -1
    call ExitProcess
```

When querying with the `ProcessDebugFlags` class, the function operates as follows:
  - A return value of 0: Indicates that a debugger is present.
  - A non-zero return value: Suggests that no debugger is currently attached, as the `NoDebugInherit` field is set accordingly.
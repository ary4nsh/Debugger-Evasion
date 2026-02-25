# NtQueryInformationProcess: ProcessDebugObjectHandle

The `ntdll!NtQueryInformationProcess` function can be used to retrieve the handle of the "debug object" associated with a process when debugging begins. This is accessible via the undocumented `ProcessDebugObjectHandle` class (0x1e). This capability allows developers to identify and interact with the debugging context, facilitating advanced debugging and monitoring strategies within their applications. The assembly code for this function is:

- x86 Assembly
```asm
    lea eax, [dwReturned]
    push eax ; ReturnLength
    push 4   ; ProcessInformationLength
    lea ecx, [hProcessDebugObject]
    push ecx ; ProcessInformation
    push 1Eh ; ProcessInformationClass
    push -1  ; ProcessHandle
    call NtQueryInformationProcess
    cmp dword ptr [hProcessDebugObject], 0
    jnz being_debugged
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
    lea r8, [hProcessDebugObject] 
                 ; ProcessInformation
    mov edx, 1Fh ; ProcessInformationClass
    mov rcx, -1  ; ProcessHandle
    call NtQueryInformationProcess
    cmp dword ptr [hProcessDebugObject], 0
    jnz being_debugged
    ...
being_debugged:
    mov ecx, -1
    call ExitProcess
```

When querying with the `ProcessDebugObjectHandle` class, the function provides the following:
  - The handle value of the debug object associated with the process.
  - If debugging is not active, the return value may indicate that no debug object handle is available.
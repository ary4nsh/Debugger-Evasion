# NtQueryInformationProcess: ProcessDebugPort

The `ntdll!NtQueryInformationProcess` function can be used to retrieve the debug port number associated with a process through the `ProcessDebugPort` class. This functionality is particularly useful for detecting if a process is currently being debugged. This capability allows developers to implement enhanced security measures or program behavior based on the debugger's presence, thereby protecting against reverse engineering attempts. The assembly code for this function is:

- x86 Assembly
```asm
    lea eax, [dwReturned]
    push eax ; ReturnLength
    push 4   ; ProcessInformationLength
    lea ecx, [dwProcessDebugPort]
    push ecx ; ProcessInformation
    push 7   ; ProcessInformationClass
    push -1  ; ProcessHandle
    call NtQueryInformationProcess
    inc dword ptr [dwProcessDebugPort]
    jz being_debugged
    ...
being_debugged:
    push -1
    call ExitProcess 
```

- x86-64 Assembly
```asm
    lea rcx, [dwReturned]
    push rcx    ; ReturnLength
    mov r9d, 4  ; ProcessInformationLength
    lea r8, [dwProcessDebugPort] 
                ; ProcessInformation
    mov edx, 7  ; ProcessInformationClass
    mov rcx, -1 ; ProcessHandle
    call NtQueryInformationProcess
    cmp dword ptr [dwProcessDebugPort], -1
    jz being_debugged
    ...
being_debugged:
    mov ecx, -1
    call ExitProcess
```

When querying with the `ProcessDebugPort` class, the function returns a DWORD value:
  - 0xFFFFFFFF (decimal -1): Indicates that the process is being debugged.
  - Any other value represents the port number of the debugger, if attached.
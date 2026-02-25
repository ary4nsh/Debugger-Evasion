# BeingDebugged Flag

The method described below provides an alternative way to check the `BeingDebugged` flag in the Process Environment Block (PEB) without invoking the `IsDebuggerPresent()` function. This can be crucial for applications that aim to hide their debugging status more effectively. By checking the `BeingDebugged` flag directly, developers can determine if the process is under debugging without relying on standard API calls, facilitating deeper security mechanisms against reverse engineering and debugging attempts. The assembly code is:

- 32Bit Process
```
mov eax, fs:[30h]
cmp byte ptr [eax+2], 0
jne being_debugged
```

- 64Bit Process
```
mov rax, gs:[60h]
cmp byte ptr [rax+2], 0
jne being_debugged
```

- WOW64 Process
```
mov eax, fs:[30h]
cmp byte ptr [eax+1002h], 0
```

In this implementation, the appropriate PEB structure is accessed based on the architecture:
- For 32-bit systems, the `__readfsdword(0x30)` function retrieves the PEB pointer.
- For 64-bit systems, the `__readgsqword(0x60)` function is utilized.
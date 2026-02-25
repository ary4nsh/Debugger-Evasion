# NtGlobalFlag

The `NtGlobalFlag` field of the Process Environment Block (PEB) can be used to detect the presence of a debugger when certain flags are set. By default, `NtGlobalFlag` is 0, but if a process is created by a debugger, specific flags will be enabled. By checking if the `NtGlobalFlag` contains the `NT_GLOBAL_FLAG_DEBUGGED`, developers can ascertain whether a debugger is currently present, allowing for robust security features against reverse engineering. 

Flags related to debugging include:

- **FLG_HEAP_ENABLE_TAIL_CHECK (0x10)**
- **FLG_HEAP_ENABLE_FREE_CHECK (0x20)**
- **FLG_HEAP_VALIDATE_PARAMETERS (0x40)**

The assembly code for this is:

- 32Bit Process
```
mov eax, fs:[30h]
mov al, [eax+68h]
and al, 70h
cmp al, 70h
jz  being_debugged
```

- 64Bit Process
```
mov rax, gs:[60h]
mov al, [rax+BCh]
and al, 70h
cmp al, 70h
jz  being_debugged
```

- WOW64 Process
```
mov eax, fs:[30h]
mov al, [eax+10BCh]
and al, 70h
cmp al, 70h
jz  being_debugged
```

A combination of these flags can be used to identify if a debugger is attached.

In this implementation, the correct PEB pointer is retrieved based on the architecture:
- For 32-bit systems, it accesses the 0x68 offset.
- For 64-bit systems, it accesses the 0xBC offset.
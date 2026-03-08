# Debugger Evasion

In this repository, you will find simple code examples demonstrating the [Debugger Evasion technique](https://attack.mitre.org/techniques/T1622/), which is used for defence evasion and discovery. 

Debuggers are typically used by defenders to trace and/or analyze the execution of potential malware payloads. Attackers may employ various means to detect and avoid debuggers.

Techniques:

[[Assembly]](https://evasions.checkpoint.com/src/Anti-Debug/techniques/assembly.html)
- DebugBreak
- ICE
- Instruction Prefixes
- Instruction Counting
- INT2D
- INT3
- POPF and CPUID
- POPF and Trap Flag
- Stack Segment Register

[[Windows]](https://evasions.checkpoint.com/src/Anti-Debug/techniques/debug-flags.html#manual-checks)
- Win32 API
   - IsDebuggerPresent()
   - CheckRemoteDebuggerPresent()
   - NtQueryInformationProcess()
      - ProcessDebugPort
      - ProcessDebugFlags
      - ProcessDebugObjectHandle
   - RtlQueryProcessHeapInformation()
   - RtlQueryProcessDebugInformation()
   - NtQuerySystemInformation()

- Manual Checks
   - BeingDebugged Flag
   - NtGlobalFlag
   - Heap Flags
   - Heap Protection
   - Check KUSER_SHARED_DATA structure

[Linux]
- ptrace_traceme

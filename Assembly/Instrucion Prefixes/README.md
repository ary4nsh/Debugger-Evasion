# Instruction Prefixes

This trick exploits how certain debuggers handle instruction prefixes, particularly in OllyDbg. When executing a specific byte sequence that begins with the prefix `F3`, the debugger may skip this prefix and directly control the next instruction. This behavior highlights how some debuggers handle prefixes differently than expected, providing a method for detecting if a program is running under a debugging environment.

- If the code is executed in a debugger like OllyDbg, stepping to the first byte (F3) results in immediate control transfer to the subsequent instruction (e.g., an `INT1` instruction), effectively bypassing the expected behavior.
- If the same code is run without a debugger, the prefix will not be skipped, leading to the raising of an exception. Consequently, control will transfer to the exception block as intended.
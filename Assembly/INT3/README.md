# INT3

The INT3 (opcode 0xCC) instruction is a specialized x86 assembly language instruction used as a software breakpoint in debugging. This instrucion can be used to evade traditional debugging methods.

- If the program encounters the INT3 instruction without an active debugger, it generates an exception: `EXCEPTION_BREAKPOINT` (0x80000003). This invokes an exception handler that can be defined in your application to manage the interruption.
- If a debugger is present at the time of the INT3 instruction execution, control will not transfer to the exception handler. Instead, the debugger will take over, allowing developers to inspect the application's state, view stack traces, and manipulate variables.

#INT3 (long form)

The long form of the INT3 instruction (opcode 0xCD 0x03), provides an alternative to the short form (opcode 0xCC). This version is also used for generating breakpoints in debugging contexts but behaves differently when the exception `EXCEPTION_BREAKPOINT` occurs, especially regarding the instruction pointer (EIP).

Upon encountering the CD 03 instruction, an `EXCEPTION_BREAKPOINT` occurs, just like with the short form. However, a key difference is that the EIP register will point to the middle of the instruction (specifically, to the 0x03 byte).

This means that when returning from the exception, the EIP needs to be adjusted manually in the exception handler if the program is intended to resume execution after the INT3 instruction.

If the EIP is not modified, the next instruction execution will most likely result in an `EXCEPTION_ACCESS_VIOLATION` due to attempting to access an invalid instruction address.

- If the program encounters the long form INT3 (CD 03) instruction without an active debugger, it generates an exception: `EXCEPTION_BREAKPOINT` (0x80000003). The EIP register will point to the middle of the instruction (the 0x03 byte), which may lead to an `EXCEPTION_ACCESS_VIOLATION` if not handled properly in the exception handler.

- If a debugger is present at the time of the execution of the long form INT3 instruction, control will not transfer to the exception handler. Instead, the debugger will take over, allowing developers to pause execution, inspect the application's state, view stack traces, and manipulate variables, with the ability to adjust the EIP as necessary for continued execution.

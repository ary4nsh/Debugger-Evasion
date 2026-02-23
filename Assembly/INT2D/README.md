# INT2D

The INT2D x86 instruction is designed to generate a breakpoint in a similar fashion to the INT3 instruction, raising the exception `EXCEPTION_BREAKPOINT` when executed. However, its behavior, especially concerning the instruction pointer (EIP) and the EAX register, introduces unique considerations.

When the INT2D instruction is executed, an `EXCEPTION_BREAKPOINT` occurs. Unlike the INT3 instruction, Windows uses the EIP register as an exception address and increments its value after the exception is raised. Additionally, Windows examines the value of the EAX register during the execution of INT2D. If the EAX value is 1, 3, or 4 across all versions of Windows, or 5 on Windows Vista and later, the exception address will be increased by one.

This behavior can cause complications for some debuggers, as the incremented EIP may allow the execution to skip the byte following the INT2D instruction, potentially resulting in the program continuing from an unintended or corrupted instruction.

To mitigate this issue, it is advisable to place a one-byte `NOP` instruction after the INT2D instruction. This ensures that execution will skip this byte regardless of the context. If the program lacks an active debugger, control will be transferred to the exception handler.

- If the program encounters the INT2D instruction without an active debugger, it generates an exception: `EXCEPTION_BREAKPOINT` (0x80000003). The EIP register will point to the INT2D instruction, and upon returning from the exception, it will have been incremented. Failure to manage this adjustment correctly may lead to an `EXCEPTION_ACCESS_VIOLATION` due to skipping an unintended instruction.

- If a debugger is present when the INT2D instruction is executed, control will not transfer to the exception handler. Instead, the debugger takes over, allowing developers to inspect the application's state, view stack traces, and manipulate variables. The debugger can also manage the EIP as necessary for the program to continue execution appropriately.

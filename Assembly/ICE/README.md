# ICE/ICEBP/INT1 (In-circuit emulation breakpoint)

The ICE instruction is an x86 instruction with an opcode of 0xF1 which  is present on all known Intel x86 processors from the 80386 onwards, and documented in the latest (May 2018, -067) edition of the SDM. It can be used to detect if the program is currently being traced by a debugger.

- If the program encounters the ICE instruction without an active debugger, it generates an exception: `EXCEPTION_SINGLE_STEP` (0x80000004). Control will be transferred to the defined exception handler in the application.
- If a debugger is present when the ICE instruction is executed, control will not transfer to the exception handler. Instead, the debugger takes over, allowing execution to proceed without interruption, which can complicate debugging efforts and detection.
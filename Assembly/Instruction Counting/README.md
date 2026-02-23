# Instruction Counting

This technique takes advantage of how certain debuggers handle `EXCEPTION_SINGLE_STEP` exceptions to detect if a program is being debugged. The method involves setting hardware breakpoints on each instruction in a predefined sequence (e.g., a sequence of NOP instructions).

1. **Setting Hardware Breakpoints**: Each instruction in the selected sequence is monitored by a hardware breakpoint. When an instruction with an active hardware breakpoint is executed, it raises the `EXCEPTION_SINGLE_STEP` exception.

2. **Exception Handling**: This exception can be caught by a vectored exception handler. Inside the exception handler:
   - A register (typically EAX) serves as an instruction counter and is incremented.
   - The instruction pointer (EIP) is adjusted to pass control to the next instruction in the predefined sequence.

3. **Counter Evaluation**: Each time control is transferred to the next instruction in the sequence, the exception is raised, and the counter is incremented. Once the entire sequence is completed, the counter is checked. 
   - If the counter does not equal the length of the sequence, it indicates that execution was interrupted, suggesting the presence of a debugger.

- If the program executes the sequence without any debugging, the counter will accurately reflect the length of the sequence.
- If a debugger is active during this execution, the counter will not match the expected length of the sequence, implying that the program is being debugged.

This technique serves as an effective way to detect debugging environments and can be integrated into programs that require anti-debugging measures.
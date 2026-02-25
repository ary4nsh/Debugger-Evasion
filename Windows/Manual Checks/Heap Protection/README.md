# Heap Protection

The presence of specific flags in the `NtGlobalFlag` affects how memory is managed within the heap, particularly when a debugger is present. 

## Key Behavior

- **HEAP_TAIL_CHECKING_ENABLED:** When this flag is set, the sequence **0xABABABAB** is appended to the end of allocated heap blocks:
  - **32-Bit Windows:** This sequence is appended **twice**.
  - **64-Bit Windows:** This sequence is appended four times.

- **HEAP_FREE_CHECKING_ENABLED:** If this flag is also set, the sequence **0xFEEEFEEE** will be appended in cases where additional bytes are needed to fill empty spaces until the next memory block.

In this implementation, `HeapWalk` is used to iterate through the heap entries. The function checks the last part of the allocated memory block to see if the **0xABABABAB** pattern is present, indicating that `HEAP_TAIL_CHECKING_ENABLED` is active. This method aids developers in detecting debugging activities and ensuring memory integrity.
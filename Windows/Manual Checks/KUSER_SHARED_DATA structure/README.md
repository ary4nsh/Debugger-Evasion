# Check KUSER_SHARED_DATA Structure

The `KUSER_SHARED_DATA` structure is a crucial component in Windows that enables data sharing between user mode and the kernel. This technique was originally highlighted as a concern for [TitanHide](https://github.com/mrexodia/TitanHide/issues/18), a kernel driver designed to conceal debuggers from detection. The detailed documentation for the structure KUSER_SHARED_DATA and its fields is available [here](https://www.geoffchappell.com/studies/windows/km/ntoskrnl/inc/api/ntexapi_x/kuser_shared_data/index.htm).

## Key Features

- The address of the `KUSER_SHARED_DATA` structure is fixed and has remained consistent across all Windows versions since its introduction.
- The user mode address is identical in both 32-bit and 64-bit environments.
- All offsets and sizes within the structure are strictly defined; new fields are only added in place of unused padding space.

This ensures that the program operates seamlessly on both Windows 2000 (32-bit) and Windows 10 (64-bit) without the need for recompilation. 

In this implementation, a byte at the address **0x7ffe02d4** is examined to determine if either of the flags **0x01** or **0x02** is set. This method provides insights into the state of the system and aids in debug detection strategies, making it a valuable tool for developers focused on security and performance in their applications.
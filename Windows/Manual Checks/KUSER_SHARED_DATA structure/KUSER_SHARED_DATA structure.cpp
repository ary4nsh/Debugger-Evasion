/*
 * Debugger check using the KUSER_SHARED_DATA structure.
 * At fixed user-mode address 0x7FFE0000; the byte at 0x7FFE02D4 has flags that may be set when a kernel debugger is present.
 * Bits 0 and 1 (0x01, 0x02) indicate debugger-related state.
 */

#include <windows.h>

// check_kuser_shared_data_structure: Returns true if the byte at 0x7FFE02D4 has bit 0 or bit 1 set (kernel debugger present)
bool check_kuser_shared_data_structure()
{
    unsigned char b = *(unsigned char*)0x7FFE02D4;   // b: Byte at offset 0x2D4 in KUSER_SHARED_DATA (0x7FFE0000 + 0x2D4); contains debugger-related flags
    return ((b & 0x01) || (b & 0x02));              // 0x01: Bit 0; 0x02: Bit 1; either set indicates debugger
}

int main()
{
    if (check_kuser_shared_data_structure())
        ExitProcess((UINT)-1);
    return 0;
}

/*
 * Minimal hardware breakpoint API (x86 DR0-DR7).
 */

#pragma once

#include <windows.h>

#ifdef __cplusplus
extern "C" {
#endif

#define HWBRK_TYPE_CODE  0
#define HWBRK_TYPE_WRITE 1
#define HWBRK_TYPE_READ  3

#define HWBRK_SIZE_1  0
#define HWBRK_SIZE_2  1
#define HWBRK_SIZE_4  3

typedef PVOID HWHBRK;

HWHBRK SetHardwareBreakpoint(HANDLE hThread, int type, int size, PVOID address);
void   RemoveHardwareBreakpoint(HWHBRK hBrk);

#ifdef __cplusplus
}
#endif

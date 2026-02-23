/*
 * Hardware breakpoint implementation (x86 DR0-DR7).
 */

#include "hwbrk.h"

#if defined(_M_IX86)

struct HwBrkHandle {
    HANDLE hThread;
    int    slot;
};

static int dr7_condition(int type)
{
    switch (type) {
        case HWBRK_TYPE_CODE:  return 0;
        case HWBRK_TYPE_WRITE: return 1;
        case HWBRK_TYPE_READ:  return 3;
        default: return 0;
    }
}

static int dr7_size(int size)
{
    switch (size) {
        case HWBRK_SIZE_1: return 0;
        case HWBRK_SIZE_2: return 1;
        case HWBRK_SIZE_4: return 3;
        default: return 0;
    }
}

HWHBRK SetHardwareBreakpoint(HANDLE hThread, int type, int size, PVOID address)
{
    CONTEXT ctx = { 0 };
    ctx.ContextFlags = CONTEXT_DEBUG_REGISTERS;

    if (!GetThreadContext(hThread, &ctx))
        return nullptr;

    int slot = -1;
    for (int i = 0; i < 4; i++) {
        if (!(ctx.Dr7 & (1 << (i * 2)))) {
            slot = i;
            break;
        }
    }
    if (slot < 0)
        return nullptr;

    (&ctx.Dr0)[slot] = (DWORD)(uintptr_t)address;
    ctx.Dr7 |= (1 << (slot * 2));
    ctx.Dr7 |= (dr7_condition(type) << (16 + slot * 4));
    ctx.Dr7 |= (dr7_size(size) << (18 + slot * 4));

    if (!SetThreadContext(hThread, &ctx))
        return nullptr;

    HwBrkHandle* h = (HwBrkHandle*)malloc(sizeof(HwBrkHandle));
    if (!h) return nullptr;
    h->hThread = hThread;
    h->slot = slot;
    return (HWHBRK)h;
}

void RemoveHardwareBreakpoint(HWHBRK hBrk)
{
    if (!hBrk) return;

    HwBrkHandle* h = (HwBrkHandle*)hBrk;
    CONTEXT ctx = { 0 };
    ctx.ContextFlags = CONTEXT_DEBUG_REGISTERS;

    if (!GetThreadContext(h->hThread, &ctx)) {
        free(h);
        return;
    }

    int slot = h->slot;
    (&ctx.Dr0)[slot] = 0;
    ctx.Dr7 &= ~(1 << (slot * 2));
    ctx.Dr7 &= ~(0xF << (16 + slot * 4));
    SetThreadContext(h->hThread, &ctx);
    free(h);
}

#else

HWHBRK SetHardwareBreakpoint(HANDLE, int, int, PVOID) { return nullptr; }
void   RemoveHardwareBreakpoint(HWHBRK) { }

#endif

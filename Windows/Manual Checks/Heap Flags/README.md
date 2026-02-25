# Heap Flags

The heap contains two crucial fields, `Flags` and `ForceFlags`, which are influenced by the presence of a debugger. The impact varies based on the Windows version.

## Flags and ForceFlags Behavior

- Normal Values:
  - **Flags:** Set to **HEAP_GROWABLE** (2)
  - **ForceFlags:** Set to 0
  
- When a Debugger is Present:
  - For Windows NT, Windows 2000, and 32-bit Windows XP:
    - **Flags:** Set to a combination of:
      - **HEAP_GROWABLE (2)**
      - **HEAP_TAIL_CHECKING_ENABLED (0x20)**
      - **HEAP_FREE_CHECKING_ENABLED (0x40)**
      - **HEAP_SKIP_VALIDATION_CHECKS (0x10000000)**
      - **HEAP_VALIDATE_PARAMETERS_ENABLED (0x40000000)**

  - For 64-bit Windows XP, Windows Vista, and higher:
    - **Flags:** Combination of:
      - **HEAP_GROWABLE (2)**
      - **HEAP_TAIL_CHECKING_ENABLED (0x20)**
      - **HEAP_FREE_CHECKING_ENABLED (0x40)**
      - **HEAP_VALIDATE_PARAMETERS_ENABLED (0x40000000)**

- **ForceFlags:** Set to:
  - **HEAP_TAIL_CHECKING_ENABLED (0x20)**
  - **HEAP_FREE_CHECKING_ENABLED (0x40)**
  - **HEAP_VALIDATE_PARAMETERS_ENABLED (0x40000000)**

In this implementation, the appropriate offsets for the `Flags` and `ForceFlags` fields are determined based on the system architecture and Windows version. The function checks for any adjustments in these fields. If a debugger is detected, appropriate actions can be taken to enhance security against reverse engineering and debugging attempts.
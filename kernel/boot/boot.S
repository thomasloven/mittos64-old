.intel_syntax noprefix
#include <mem.h>
#include <cpuid.h>

.section .data
// Some room to store the bootloader return values
MultiBootMagic:
  .quad 0x0
MultiBootData:
  .quad 0x0

// Some info from here: http://os.phil-opp.com/multiboot-kernel.html
.section .text
.global _start
.code32 // GRUB leaves us in 32 bit mode
_start:
  cli

  // Save eax and ebx in memory
  mov V2P(MultiBootMagic), eax
  mov V2P(MultiBootData), ebx

  // Check if long mode is available
  // Otherwise, there's no point in continuing
  call check_longmode


  // Enable long mode through process described in
  // AMD64 manual vol. 2 ch. 14

  // Enable Physical Address Extension
  mov eax, cr4
  or eax, 1<<5
  mov cr4, eax

  // Load page directory into cr3
  mov eax, offset V2P(BootP4)
  mov cr3, eax

  // Enable long mode
  mov ecx, 0xC0000080
  rdmsr
  or eax, 1<<8
  wrmsr

  // Enable paging
  mov eax, cr0
  or eax, 1<<31
  mov cr0, eax


  // We are now in 32 bit long mode
  // Go to 64 bit mode by loading a GDT
  // and perforing a far jump
  lgdt [V2P(BootGDTp)]

  mov eax, 0x10
  mov ss, eax
  mov ds, eax
  mov es, eax

  .extern long_mode_start
  jmp 0x8:V2P(long_mode_start)


.code64
long_mode_start:
  // We are now in 64 bit long mode
  // but we are still running code in the identity mapped low memory kernel mirror
  // Next we jump to code in high memory (> 32 bits)

  movabs rax, offset .upper_memory
  jmp rax


.upper_memory:
  // We are now in high memory

  // Set up a stack and set up a fake return address to stop backtracing
  movabs rsp, offset BootStack
  push 0
  mov rbp, rsp

  // Remove identity mapping
  mov rax, 0x0
  mov [V2P(BootP4)], rax

  // CPUID
#define CPUID(function) mov rax, (function); cpuid;
#define STORE(reg, variable) movabs r8, offset (variable); mov dword ptr [r8], (reg);
  // Store max CPUID function numbers
  CPUID(CPUID_FUNCTION_VENDOR)
  STORE(eax, cpuid_max)
  CPUID(CPUID_FUNCTIONX_VENDOR)
  STORE(eax, cpuid_maxx)
  // Store CPUID features
  CPUID(CPUID_FUNCTION_FEATURES)
  STORE(eax, cpuid_signature)
  STORE(ebx, cpuid_features_b)
  STORE(ecx, cpuid_features_c)
  STORE(edx, cpuid_features_d)
  // ...and extended features
  CPUID(CPUID_FUNCTIONX_FEATURES)
  STORE(ebx, cpuid_featuresx_b)
  STORE(ecx, cpuid_featuresx_c)
  STORE(edx, cpuid_featuresx_d)

  // Get the saved bootloader data and pass to kmain
  movabs rax, MultiBootMagic
  mov rdi, rax
  movabs rax, MultiBootData
  mov rsi, rax

  // Call c kernel code
  .extern kmain
  call kmain
  hlt



.code32

// Check if CPUID is available
// Method described in AMD64 manual vol. 3
check_cpuid:
  pushfd
  pushfd
  xor dword ptr[esp], 0x00200000
  popfd
  pushfd
  pop eax
  xor eax, [esp]
  popfd
  and eax, 1<<21
  jz error
  ret

// Check if long mode is available using cpuid
// AMD manual vol. 3 Appendix E
check_longmode:
  call check_cpuid
  mov eax, 0x80000000
  cpuid
  cmp eax, 0x80000001
  jb error
  mov eax, 0x80000001
  cpuid
  test edx, 1<<29
  jz error
  ret



// Jump here if something went wrong
error:
  // Print ERR! to screen
  mov [0xb8000], dword ptr 0x4f524f45
  mov [0xb8004], dword ptr 0x4f214f52
  jmp .

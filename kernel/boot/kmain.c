#include <debug.h>
#include <multiboot.h>
#include <mem.h>
#include <scheduler.h>
#include <thread.h>
#include <process.h>
#include <cpu.h>
#include <timer.h>
#include <string.h>
#include <cpu.h>
#include <elf.h>
#include <syscall.h>
#include <cpuid.h>
#include <vfs.h>
#include <ata.h>

int kernel_execve(process_t *p, void *image, char *argv[], char *envp[]);
int kmain(uint64_t multiboot_magic, void *multiboot_data)
{
  debug_init();
  debug_ok("MITTOS64 kernel booted\n");
  debug_build_time();
  debug_git_info();

  interrupt_init();
  multiboot_init(multiboot_magic, P2V(multiboot_data));
  vmm_init();
  pmm_init();
  cpu_init();
  pit_init();

  debug_info("Syscall enabled:%d\n", CPUID_FEATURE_SYSCALL);

  fs_mount(0, "/");
  fs_mount(&debug_file, "/dev/debug");

  ata_init();

  fs_write(&debug_file, "TESTING DEBUG FILE", 18, 0);

  process_t *p1 = process_spawn(0);
  char *args[] = {"init", "Hello, arg", "5", 0};
  char *env[] = {"OS=mittos64", 0};
  p1->fp[1].file = fs_namef("/dev/debug");
  thread_t *th = exec_elf(p1, mboot_data.init, args, env);
  scheduler_insert(th);

  asm("sti");
  debug_info("BOOT COMPLETE\n");
  schedule();
  debug_error("PANIC - This line should be unreachable (%s:%d)\n", __FILE__, __LINE__);
  for(;;)asm("hlt");
}

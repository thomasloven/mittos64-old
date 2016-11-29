#include <elf.h>
#include <stdint.h>
#include <debug.h>

thread_t *exec_elf(process_t *p, void *image)
{
  void *entry = load_elf(p, image);
  debug("Address:%x\n", entry);
  procmm_setup(p);
  thread_t *th = new_thread((void *)entry, 1);
  process_attach(p, th);

  return th;
}

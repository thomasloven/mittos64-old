#include <elf.h>
#include <stdint.h>
#include <debug.h>
#include <mem.h>
#include <string.h>
#include <process.h>

thread_t *exec_elf(process_t *p, void *image, char *argv[], char *envp[])
{
  int argc = 0, envc=0;
  // Count strings and save their lengths
  int len = 0;
  while(argv[argc]){
    len += strlen(argv[argc]) + 1;
    argc++;
  }
  len += (argc+1)*sizeof(char *);
  while(envp[envc])
  {
    len += strlen(envp[envc]) + 1;
    envc++;
  }
  len += (envc+1)*sizeof(char *);
  len += sizeof(size_t)*2;
  len += sizeof(char);

  // Replace process memory space with new image
  void *entry = load_elf(p, image);
  procmm_setup(p, len);

  // We will write to process memory, so let's switch to it
  vmm_set_P4(p->mmap->P4);


  // Memory layout of area before BRK
  // argv[]
  // 0
  // envp[]
  // 0
  // auxv[]
  // arguments
  // env strings
  // 0
  // BRK points here
  void *pos = (void *)p->mmap->brk->start;
  char **_argv = pos;
  pos = incptr(pos, (argc+1)*sizeof(char *));
  char **_envp = pos;
  pos = incptr(pos, (envc+1)*sizeof(char *));
  size_t *_auxv = pos;
  pos = incptr(pos, sizeof(size_t)*2);

  // Copy arguments
  for(int i = 0; i < argc; i++)
  {
    size_t len = strlen(argv[i]) + 1;
    _argv[i] = pos;
    memcpy(_argv[i], argv[i], len);
    pos = incptr(pos, len);
  }
  _argv[argc] = 0;
  // Copy environment strings
  for(int i = 0; i < envc; i++)
  {
    size_t len = strlen(envp[i]) + 1;
    _envp[i] = pos;
    memcpy(_envp[i], envp[i], len);
    pos = incptr(pos, len);
  }
  _envp[envc] = 0;
  // No auxiliary vectors for now
  _auxv[0] = 0;

  thread_t *th = new_thread((void *)entry, 1);
  process_attach(p, th);
  th->r.rsi = (uint64_t)_argv;
  th->r.rdi = argc;

  return th;
}

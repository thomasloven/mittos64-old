#include <stdint.h>
#include <string.h>
#include <mem.h>
#include <debug.h>
#include <process.h>

typedef struct
{
  uint8_t identity[16];
  uint16_t type;
  uint16_t machine;
  uint32_t version;
  uint64_t entry;
  uint64_t ph_offset;
  uint64_t sh_offset;
  uint32_t flags;
  uint16_t header_size;
  uint16_t ph_size;
  uint16_t ph_num;
  uint16_t sh_size;
  uint16_t sh_num;
  uint16_t strtab_index;
}__attribute__((packed)) elf_header;

#define ELF_TYPE_EXECUTABLE 2

typedef struct
{
  uint32_t type;
  uint32_t flags;
  uint64_t offset;
  uint64_t virtual_address;
  uint64_t physical_address;
  uint64_t file_size;
  uint64_t mem_size;
  uint64_t align;
}__attribute__((packed)) elf_phead;

#define ELF_PT_LOAD 1

void load_elf_segment(process_t *p, uint8_t *file, elf_phead *phead)
{
  uint64_t memsize = phead->mem_size;
  uint64_t filesize = phead->file_size;
  uint64_t mempos = phead->virtual_address;
  uint64_t filepos = phead->offset;
  debug("  Load segment %x to %x-%x\n", filepos, mempos, mempos+memsize);

  procmm_map(p->mmap, mempos, mempos+memsize, 0);
  vmm_p4_memcpy(p->mmap->P4, (void *)mempos, 0, &file[filepos], filesize);
  vmm_p4_memset(p->mmap->P4, incptr(mempos, filesize), 0, memsize - filesize);
}

void *load_elf(process_t *p, void *data)
{
  elf_header *elf = data;
  if(memcmp(elf->identity, "\x7f""ELF", 4))
  {
    debug_error("ELF - No elf magic at %x\n", elf->identity);
    return 0;
  }
  if(elf->type != ELF_TYPE_EXECUTABLE)
    return 0;

  debug_info("ELF - Loading ELF executable\n");
  elf_phead *phead = incptr(data, elf->ph_offset);
  for(uint32_t i = 0; i < elf->ph_num; i++)
  {
    if(phead[i].type == ELF_PT_LOAD)
    {
      load_elf_segment(p, data, &phead[i]);
    }
  }
  return (void *)elf->entry;
}

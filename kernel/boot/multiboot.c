#include <stdint.h>
#include <multiboot.h>
#include <mem.h>
#include <debug.h>

struct mboot_data_st mboot_data;


void parse_multiboot1()
{
  mboot1_info *data = mboot_data.data;
  if(data->flags & (1<<2))
  {
    debug_ok("MBOOT1 - commandline\n");
    mboot_data.commandline = P2V(data->cmdline);
  }
  if(data->flags & (1<<9))
  {
    debug_ok("MBOOT1 - bootloader\n");
    mboot_data.bootloader = P2V(data->bootloader_name);
  }
  if(data->flags & (1<<6))
  {
    debug_ok("MBOOT1 - memory map\n");
    mboot_data.mmap_size = data->mmap_len/sizeof(mboot1_mmap_entry);
    mboot_data.mmap = P2V(data->mmap_addr);
  }
  if(data->flags & (1<<3))
  {
    if(data->mods_count)
    {
      mboot1_module_entry *mod = P2V(data->mods_addr);
      mboot_data.init = P2V(mod->mod_start);
      mboot_data.init_len = mod->mod_end - mod->mod_start;
      debug_ok("[MBOOT1] init executable: %x (%x bytes)\n", mboot_data.init, mboot_data.init_len);
    }
  }
}

char *mboot_tags_type[] = {
  "", // 0
  "Boot command line", // 1
  "Boot loader name", // 2
  "Module", // 3
  "Basic memory information", // 4
  "BIOS Boot device", // 5
  "Memory map", // 6
  "VBE info", // 7
  "Framebuffer info", // 8
  "ELF-Symbols", //9
  "APM table", // 10
  "EFI 32-bit system table pointer", // 11
  "EFI 64-bit system table pointer", // 12
  "SMBIOS tables", // 13
  "ACPI old RSDP", // 14
  "ACPI new RSDP", // 15
  "Networking information", // 16
  "EFI memory map", // 17
  "EFI boot services not terminated",
};
void parse_multiboot2()
{
  mboot2_tags_head *data = mboot_data.data;
  mboot2_tag_basic *tag = (void *)(data + 1);
  while(tag->type != 0)
  {
    switch(tag->type)
    {
      case MBOOT2_CMDLINE:
        mboot_data.commandline = tag->data;
        debug_ok("MBOOT2 - handle ");
        break;
      case MBOOT2_BOOTLOADER:
        mboot_data.bootloader = tag->data;
        debug_ok("MBOOT2 - handle ");
        break;
      case MBOOT2_MMAP:
        mboot_data.mmap_size = (tag->size - 8)/
          ((mboot2_memory_map*)tag->data)->entry_size;
        mboot_data.mmap = tag->data;
        debug_ok("MBOOT2 - handle ");
        break;
      case MBOOT2_ACPI_V1:
      case MBOOT2_ACPI_V2:
        mboot_data.rsdp = &tag->data;
        debug_ok("[MBOOT2] %x handle ", mboot_data.rsdp);
        break;
      default:
        debug_warning("MBOOT2 - ignore ");
        break;
    }
    if(tag->type <= 18)
      debug("%s (%d bytes)\n", mboot_tags_type[tag->type], tag->size);
    else
      debug("Unknown tag: %d (%d bytes)\n", tag->type, tag->size);
    tag = next_tag(tag);
  }
}

void multiboot_init(uint64_t magic, void *data)
{
  mboot_data.data = data;
  debug_info("MBOOT - Magic:%x\n", magic);
  if(magic == MBOOT1_MAGIC2)
  {
    debug_info("MBOOT - Multiboot version 1\n");
    mboot_data.version = 1;
    parse_multiboot1();
  } else if (magic == MBOOT2_MAGIC2) {
    debug_info("MBOOT - Multiboot version 2\n");
    mboot_data.version = 2;
    parse_multiboot2();
  } else {
    debug_error("PANIC - NOT A MULTIBOOT COMPLIANT BOOTLOADER\n");
    for(;;);
  }
  if(mboot_data.commandline)
    debug_info("MBOOT - Command line: %s\n", mboot_data.commandline);
  if(mboot_data.bootloader)
    debug_info("MBOOT - Bootloader: %s\n", mboot_data.bootloader);
}

#define overlap(start1, end1, start2, end2) \
  (!( \
    ((uintptr_t)(start1) >= (uintptr_t)(end2)) \
      || ((uintptr_t)(end1) <= (uintptr_t)(start2)) \
  ))
#define overlapsz(start1, size1, start2, size2) \
  overlap((start1), (start1)+(size1), (start2), (start2)+(size2))

int multiboot_page_used(uintptr_t addr)
{
  if(overlapsz(addr, PAGE_SIZE, V2P(mboot_data.init), mboot_data.init_len))
    return 1;
  if(mboot_data.version == 1)
  {
    mboot1_info *data = mboot_data.data;
    if(overlapsz(addr, PAGE_SIZE, V2P(data), sizeof(data)))
      return 1;
    if(data->flags & (1<<6))
    {
      if(overlapsz(addr, PAGE_SIZE, data->mmap_addr, data->mmap_len*sizeof(mboot1_mmap_entry)))
        return 1;
    }
  } else {
    mboot2_tags_head *data = mboot_data.data;
    if(overlapsz(addr, PAGE_SIZE, V2P(data), data->total_size))
      return 1;
    if(mboot_data.rsdp && overlapsz(addr, PAGE_SIZE, mboot_data.rsdp, 1))
      return 1;
  }
  return 0;
}

int multiboot_get_memory_area(uintptr_t *start, uintptr_t *end, uint32_t *type)
{
  static uint32_t entry = 0;
  if(entry >= mboot_data.mmap_size)
    return 0;
  if(mboot_data.version == 1)
  {
    mboot1_mmap_entry *map = (void *)mboot_data.mmap;
    *start = ((uintptr_t)map[entry].base_hi << 32) + map[entry].base_lo;
    *end = *start + ((uintptr_t)map[entry].len_hi << 32) + map[entry].len_lo;
    *type = map[entry].type;
  } else {
    mboot2_memory_map *mmap = mboot_data.mmap;
    mboot2_mmap_entry *e = mmap->entries;
    uint32_t i = entry;
    while(i)
    {
      e = incptr(e, mmap->entry_size);
      i--;
    }
    *start = (uintptr_t)e->base_addr;
    *end = *start + (uintptr_t)e->length;
    *type = e->type;
  }
  entry++;
  return 1;
}

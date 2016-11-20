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

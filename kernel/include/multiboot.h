#pragma once
#define MBOOT1_MAGIC 0x1BADB002
#define MBOOT1_MAGIC2 0x2BADB002
#define MBOOT1_FLAG_PAGE_ALIGN 0x01
#define MBOOT1_FLAG_MEM_INFO 0x02
#define MBOOT1_HEADER_FLAGS (MBOOT1_FLAG_PAGE_ALIGN | MBOOT1_FLAG_MEM_INFO)
#define MBOOT1_HEADER_CHECKSUM -(MBOOT1_HEADER_FLAGS + MBOOT1_MAGIC)

#define MBOOT2_MAGIC 0xE85250D6
#define MBOOT2_MAGIC2 0x36D76289
#define MBOOT2_ARCH 0
#define MBOOT2_LENGTH (MultiBootHeaderEnd - MultiBootHeader)
#define MBOOT2_CHECKSUM -(MBOOT2_MAGIC + MBOOT2_ARCH + MBOOT2_LENGTH)

#ifndef __ASSEMBLER__
#include <stdint.h>

// MULTIBOOT 1
  typedef struct {
    uint32_t flags;
    uint32_t mem_lower;
    uint32_t mem_upper;
    uint32_t boot_device;
    uint32_t cmdline;
    uint32_t mods_count;
    uint32_t mods_addr;
    uint32_t symbol_num;
    uint32_t symbol_size;
    uint32_t symbol_addr;
    uint32_t symbol_shndx;
    uint32_t mmap_len;
    uint32_t mmap_addr;
    uint32_t drives_len;
    uint32_t drives_addr;
    uint32_t config_table;
    uint32_t bootloader_name;
    uint32_t apm_table;
    uint32_t vbe_control_info;
    uint32_t vbe_mode_info;
    uint32_t vbe_mode;
    uint32_t vbe_interface_seg;
    uint32_t vbe_interface_off;
    uint32_t vbe_interface_len;
  } mboot1_info;

typedef struct {
  uint32_t size;
  uint32_t base_lo;
  uint32_t base_hi;
  uint32_t len_lo;
  uint32_t len_hi;
  uint32_t type;
} mboot1_mmap_entry;

// MULTIBOOT 2

  typedef struct {
    uint32_t total_size;
    uint32_t reserved;
  } mboot2_tags_head;

  typedef struct {
    uint32_t type;
    uint32_t size;
    char data[];
  } mboot2_tag_basic;

  typedef struct {
    uint64_t base_addr;
    uint64_t length;
    uint32_t type;
    uint32_t reserved;
  } mboot2_mmap_entry;
  typedef struct {
    uint32_t entry_size;
    uint32_t entry_version;
    mboot2_mmap_entry entries[];
  } mboot2_memory_map;

  #define MBOOT2_CMDLINE 1
  #define MBOOT2_BOOTLOADER 2
  #define MBOOT2_MMAP 6
  #define MBOOT2_ACPI_V1 14
  #define MBOOT2_ACPI_V2 15

  // Multiboot tags are padded to a multiple of 8 bytes
  #define next_tag(tag) ((void *)((((uintptr_t)tag) + tag->size + 7) & ~0x7))

  extern char *mboot_tags_type[];
  struct mboot_data_st{
    int version;
    void *data;
    char *commandline;
    char *bootloader;
    void *mmap;
    uint32_t mmap_size;
    void *rsdp;
  };
  extern struct mboot_data_st mboot_data;

#define MBOOT_MMAP_FREE 1

  void multiboot_init(uint64_t magic, void *data);
  int multiboot_page_used(uintptr_t addr);
  int multiboot_get_memory_area(uintptr_t *start, uintptr_t *end, uint32_t *type);

#endif

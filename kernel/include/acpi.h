#pragma once
#include <stdint.h>

typedef struct RSDP_st
{
  unsigned char signature[8];
  uint8_t checksum;
  unsigned char OEMID[6];
  uint8_t revision;
  uint32_t RSDT_p;
  // The following are only available if revision != 0
  uint32_t length;
  uint64_t XSDT_p;
  uint8_t checksum2;
  uint8_t reserved[3];
}__attribute__ ((packed)) RSDP_st;

typedef struct SDT_header
{
  unsigned char signature[4];
  uint32_t length;
  uint8_t revision;
  uint8_t checksum;
  unsigned char OEMID[6];
  unsigned char OEMtable[8];
  uint32_t OEMrevision;
  uint32_t CreatorID;
  uint32_t CreatorRevision;
  uint8_t data[];
}__attribute__ ((packed)) SDT_header;

typedef struct MADT_header
{
  uint32_t LCA;
  uint32_t flags;
  uint8_t fields[];
}__attribute__((packed)) MADT_header;
typedef struct MADT_field
{
  uint8_t type;
  uint8_t length;
  union{
    struct{
      uint8_t proc_ID;
      uint8_t APIC_ID;
      uint32_t flags;
    }__attribute__((packed)) proc;
    struct {
      uint8_t APIC_ID;
      uint8_t reserved;
      uint32_t address;
      uint32_t base;
    }__attribute__((packed)) IOAPIC;
    struct {
      uint8_t bus;
      uint8_t irq;
      uint32_t interrupt;
      uint16_t flags;
    }__attribute__((packed)) ISO;
  };
}__attribute__((packed)) MADT_field;

void acpi_init();

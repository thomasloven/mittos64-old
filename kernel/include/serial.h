#pragma once
#include <stdint.h>

#define PORT_COM1 0x3F8
#define PORT_COM2 0x2F8
#define PORT_COM3 0x3E8
#define PORT_COM4 0x2E8

void serial_init(uint16_t port);
void serial_write(uint16_t port, uint8_t c);

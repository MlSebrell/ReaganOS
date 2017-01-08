#include <stddef.h>
#include <stdint.h>

#include <arch/device.h>
#include <arch/i386/textscreen.h>
#include <drivers/terminal.h>
#include <mm/heap.h>
#include <lib/string.h>

size_t row = 0;
size_t column = 0;
uint8_t color;
uint16_t * buffer;

device_t * device;

void terminal_flush () {
	textscreen_reset (color);
	
	size_t i;
	size_t j;
	for (i = 0; i < VGA_HEIGHT; i++) {
		for (j = 0; j < VGA_WIDTH; j++) {
			textscreen_writeCharAt (buffer [i * VGA_WIDTH + j], color, j, i);
		}
	}
}

void terminal_clsColor (uint8_t color) {
	for (size_t y = 0; y < VGA_HEIGHT; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {
			const size_t index = y * VGA_WIDTH + x;
			buffer[index] = getVgaEntry (' ', color);
		}
	}
}

void terminal_cls () {
	terminal_clsColor (color);
}

void terminal_writeCharAt (char c, size_t x, size_t y) {
	const size_t index = y * VGA_WIDTH + x;
	buffer [index] = c;
}

void terminal_writeChar (char c) {
	switch (c) {
		case '\n':
			row++;
			column = 0;
			break;
		case '\t':
			for (size_t i = 0; i < TAB_LENGTH; i++) terminal_writeChar (' ');
			break;
		default:
			terminal_writeCharAt (c, column, row);
			if (++column == VGA_WIDTH) {
				column = 0;
				if (++row == VGA_HEIGHT) {
					row = 0;
				}
			}
			break;
	}
}

size_t terminal_read (device_t * dev, void * ptr, size_t bytes, uint64_t offset) {
	
}
size_t terminal_write (device_t * dev, void * ptr, size_t bytes, uint64_t offset) {
	char * cptr = (char *)ptr;
	size_t i;
	for (i = 0; i < bytes; i++) {
		terminal_writeChar (cptr [i + offset]);
	}
	terminal_flush ();
}
size_t terminal_ioctl (device_t * dev, int one, int two, int three) {
	switch (one) {
		case 0:
			terminal_flush ();
			break;
		case 1:
			terminal_cls ();
			break;
		case 2:
			terminal_clsColor (getVgaColor (two, three));
			break;
		case 3:
			color = getVgaColor (two, three);
			break;
	}
}

device_t * terminal_init (const char * name, uint8_t tcolor) {
	buffer = heap_alloc (VGA_WIDTH * VGA_HEIGHT);
	memset (0, buffer, VGA_WIDTH * VGA_HEIGHT);
	color = tcolor;
	terminal_cls ();
	
	device = device_add (name, terminal_read, terminal_write, terminal_ioctl);
	return device;
}

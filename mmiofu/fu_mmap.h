#include <assert.h>
#include <fcntl.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#define IO_PAGE 0xFFFEF000
#define IO_PAGE_LEN 0x1000 // One page

char * _io_map;

void _create_io_map(void);

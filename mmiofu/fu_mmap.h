#include <assert.h>
#include <fcntl.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#define handle_error(msg) ( perror(msg), exit(EXIT_FAILURE) )

#define IO_PAGE 0xFFFEF000
#define IO_PAGE_LEN 0x1000 // One page

void * _io_map;

void _create_io_map(void);

// Offload a computation to a MMI/O Functional Unit
// Communication page is 0xFFFEF000 in physical memory
//
// protocol I'm using for this test:
// 0xFFFEF000: double A (arg)
//             double B (arg)
//             double C (result)

#include "fu_mmap.h"

bool _create_io_map()
{
  int fd = open("/dev/mem", O_RDWR);
  if (fd == -1) {
    perror("open");
    return false;
  }

  // confirm that IO_PAGE is actually on a page boundary
  assert((IO_PAGE & (sysconf(_SC_PAGE_SIZE) - 1)) == 0x0);

  char * addr =
    mmap(NULL, IO_PAGE_LEN, PROT_READ | PROT_WRITE, MAP_SHARED, fd, IO_PAGE);
  if (addr == MAP_FAILED) {
    perror("mmap");
    return false;
  }
  
  close(fd);
  _io_map = addr;
  return true;
}

// TODO: do I have to worry about munmap()? Should get closed automatically on exit

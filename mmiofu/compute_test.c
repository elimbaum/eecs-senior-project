// Offload a computation to a MMI/O Functional Unit
// Communication page is 0xFFFEF000 in physical memory
//
// protocol I'm using for this test:
// 0xFFFEF000: double A (arg)
//             double B (arg)
//             double C (result)

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
#define IO_PAGE_LEN 0x1000

void * map_io_mem()
{
  int fd = open("/dev/mem", O_RDWR);
  if (fd == -1)
    handle_error("open");

  // confirm that IO_PAGE is actually on a page boundary
  assert((IO_PAGE & (sysconf(_SC_PAGE_SIZE) - 1)) == 0x0);

  char * addr =
    mmap(NULL, IO_PAGE_LEN, PROT_READ | PROT_WRITE, MAP_SHARED, fd, IO_PAGE);
  if (addr == MAP_FAILED)
    handle_error("mmap");
  
  close(fd);

  return addr;
}

int main(int argc, char * argv[])
{
  double A = 5;
  double B = 12;

  // if given, read inputs from command line
  if (argc == 3) {
    A = strtod(argv[1], NULL);
    B = strtod(argv[2], NULL);
  }
  
  printf("Mapping IO... \n");
  double * io_map = map_io_mem();
  printf("Success!\n");

  printf("Writing data\n");
  io_map[0] = A;
  io_map[1] = B;
  printf("Finished writing data; reading\n");

  double C = io_map[2];
  printf("%g %g => %g (libm: %g)\n", A, B, C, hypot(A, B));

  munmap(io_map, IO_PAGE_LEN);
  exit(EXIT_SUCCESS);
}




#include <sys/types.h>
#include <sys/mman.h>
#include <sys/fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include <at91_pio.h>

// S4M
//---------------------------------------------------------------------------------------------------------------------------------------
//bytes     3                               2                               1                               0                           
//bits      31  30  29  28  27  26  25  24  23  22  21  20  19  18  17  16  15  14  13  12  11  10  9   8   7   6   5   4   3   2   1   0
//bit       7   6   5   4   3   2   1   0   7   6   5   4   3   2   1   0   7   6   5   4   3   2   1   0   7   6   5   4   3   2   1   0
//---------------------------------------------------------------------------------------------------------------------------------------
//Port A    -   *   -   -   -   -   -   -   -   *   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   *   S2  -   -   -   -
//Port B    *   *   S1  S1  S0  S0  S0  S0  S0  S0  *   *   *   *   *   *   DE  DE  *   *   S3  S3  S2  S2  S1  S1  S0  S0  *   *   *   *
//Port C    ID  ID  ID  ID  ID  C4  *   *   C4  C3  C3  *   *   *   *   RST *   *   *   *   -   *   *   S3  -   -   *   Rdy ADC ADC ADC ADC
//---------------------------------------------------------------------------------------------------------------------------------------


//CPU
//---------------------------------------------------------------------------------------------------------------------------------------
//bytes     3                               2                               1                               0                           
//bits      31  30  29  28  27  26  25  24  23  22  21  20  19  18  17  16  15  14  13  12  11  10  9   8   7   6   5   4   3   2   1   0
//bit       7   6   5   4   3   2   1   0   7   6   5   4   3   2   1   0   7   6   5   4   3   2   1   0   7   6   5   4   3   2   1   0
//---------------------------------------------------------------------------------------------------------------------------------------
//Port A    -   -   -   -   -   -   -   -   -   *   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   S2  S2  -   -   -   -
//Port B    Key Key S1  S1  S0  S0  S0  S0  S0  S0  Key Key *   *   *   *   DE  DE  *   *   S3  S3  S2  S2  S1  S1  S0  S0  EPM EPM EPM EPM
//Port C    -   -   -   -   -   *   -   -   Key Key Key Key *   *   NAD RST LAN NAD -   LAN -   S3  *   S3  -   -   *   RDY ADC ADC ADC ADC
//---------------------------------------------------------------------------------------------------------------------------------------

//      S0 ~ S3 = Serial Port (Port 1 ~ 4)
//      DE      = Debug Port
//      RST     = Reset 
//      RDY     = Ready LED
//      ADC     = Analog Digital Converter 
//      LAN     = LAN Port
//      NAD     = Nand Flash
//      ER      = SPI(EEPROM)
//      Key     = Keypad
//      -       = Not Used
//      *       = GPIO & User Port

#define PIOA_BASE	0xF400
#define PIOB_BASE	0xF600
#define PIOC_BASE	0xF800

inline unsigned int
read_pio(unsigned char* mem_base, unsigned pio_base, unsigned reg)
{
  return *((unsigned int*) (mem_base + pio_base + reg));
}

inline void
write_pio(unsigned char* mem_base, unsigned pio_base, unsigned reg, unsigned int value)
{
  *((unsigned int*) (mem_base + pio_base + reg)) = value;
}

int
main(int argc, char* argv[])
{
  int pagesize = getpagesize();
  
  int fd = open("/dev/mem", O_RDWR | O_SYNC);
  if (fd < 0) {
    perror("/dev/mem");
    exit(1);
  }

  unsigned char* base = (unsigned char*) mmap(0, 0x10000, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0xFFFF0000);

  if (base == MAP_FAILED) {
    perror("mmap");
    exit(1);
  }
  printf("mapped at %08x\n", (unsigned int) base);

  write_pio(base, PIOB_BASE, PIO_PDR, 0x0000000F);
  write_pio(base, PIOB_BASE, PIO_IDR, 0x0000000F);
  write_pio(base, PIOB_BASE, PIO_PER, 0x0000000F);
  write_pio(base, PIOB_BASE, PIO_PUER, 0x0000000F);
  printf("PIOB PIO_PSR: 0x%08x\n", read_pio(base, PIOB_BASE, PIO_PSR));
  printf("PIOB PIO_OSR: 0x%08x\n", read_pio(base, PIOB_BASE, PIO_OSR));
  printf("PIOB PIO_PUSR: 0x%08x\n", read_pio(base, PIOB_BASE, PIO_OSR));
  for (int i = 0; i < 10000000; i++) {
    write_pio(base, PIOB_BASE, PIO_CODR, 0x0000000F);
    write_pio(base, PIOB_BASE, PIO_SODR, 0x0000000F);
  }
  printf("PIOB PIO_ODSR: 0x%08x\n", read_pio(base, PIOB_BASE, PIO_ODSR));
  printf("PIOB PIO_PDSR: 0x%08x\n", read_pio(base, PIOB_BASE, PIO_PDSR));

  munmap(base, pagesize);
  close(fd);
}

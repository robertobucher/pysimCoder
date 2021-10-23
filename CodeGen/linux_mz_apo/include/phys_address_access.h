/*******************************************************************
  This header file contains definition of static inline functions
  for mapping physical address to virtual address and reading from and
  writing to it.

  (C) Copyright 2017 by Pavel Pisa
      e-mail:   pisa@cmp.felk.cvut.cz
      homepage: http://cmp.felk.cvut.cz/~pisa
      company:  http://www.pikron.com/
      license:  any combination of GPL, LGPL, MPL or BSD licenses

 *******************************************************************/

#ifndef PHYS_ADDRESS_ACCESS_H
#define PHYS_ADDRESS_ACCESS_H


#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>


typedef struct mem_address_map_t {
  uintptr_t regs_base_phys;
  void     *regs_base_virt;
  size_t    region_size;
} mem_address_map_t;


/*
 * The support function which returns pointer to the virtual
 * address at which starts remapped physical region in the
 * process virtual memory space.
 */
static inline
void *map_phys_address(off_t region_base, size_t region_size, int opt_cached)
{
	unsigned long mem_window_size;
	unsigned long pagesize;
	unsigned char *mm;
	unsigned char *mem;
	int fd;
	char *memdev="/dev/mem";

	/*
	 * Open a device ("/dev/mem") representing physical address space
	 * in POSIX systems
	 */
	fd = open(memdev, O_RDWR | (!opt_cached? O_SYNC: 0));
	if (fd < 0) {
		fprintf(stderr, "cannot open %s\n", memdev);
		return NULL;
	}

	/*
	 * The virtual to physical address mapping translation granularity
	 * corresponds to memory page size. This call obtains the page
	 * size used by running operating system at given CPU architecture.
	 * 4kB are used by Linux running on ARM, ARM64, x86 and x86_64 systems.
	 */
	pagesize=sysconf(_SC_PAGESIZE);

	/*
	 * Extend physical region start address and size to page size boundaries
	 * to cover complete requested region.
	 */
	mem_window_size = ((region_base & (pagesize-1)) +
			  region_size + pagesize-1) & ~(pagesize-1);

	/*
	 * Map file (in our case physical memory) range at specified offset
	 * to virtual memory ragion/area (see VMA Linux kernel structures)
	 * of the process.
	 */
	mm = mmap(NULL, mem_window_size, PROT_WRITE|PROT_READ,
		MAP_SHARED, fd, region_base & ~(pagesize-1));

	/* Report failure if the mmap is not allowed for given file or its region */
	if (mm == MAP_FAILED) {
		return NULL;
	}

	/*
	 * Add offset in the page to the returned pointer for non-page-aligned
	 * requests.
	 */
	mem = mm + (region_base & (pagesize-1));

	return mem;
}


static inline
mem_address_map_t *mem_address_map_create(off_t region_base, size_t region_size, int opt_cached)
{
	mem_address_map_t *memadrs;
	memadrs = malloc(sizeof(*memadrs));
    if (memadrs == NULL) {
		return NULL;
    }
	memadrs->regs_base_phys = region_base;
	memadrs->region_size = region_size;
	memadrs->regs_base_virt = map_phys_address(region_base, region_size, 0);
	if (memadrs->regs_base_virt == NULL) {
		return NULL;
	}
	return memadrs;
}

static inline
void mem_address_unmap_and_free(mem_address_map_t *memadrs)
{
	if (memadrs == NULL)
	    return;
	munmap(memadrs->regs_base_virt, memadrs->region_size);
	memadrs->regs_base_virt = NULL;
	free(memadrs);
}

static inline
uint32_t mem_address_reg_rd(mem_address_map_t *memadrs, unsigned reg_offs)
{
	return *(volatile uint32_t*)((char*)memadrs->regs_base_virt + reg_offs);
}

static inline
void mem_address_reg_wr(mem_address_map_t *memadrs, unsigned reg_offs, uint32_t val)
{
	*(volatile uint32_t*)((char*)memadrs->regs_base_virt + reg_offs) = val;
}


#endif /*PHYS_ADDRESS_ACCESS_H*/


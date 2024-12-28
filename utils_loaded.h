#pragma once
#include <stdint.h>
#include <elf.h>
#define nullptr ((void*)0)
#define STDOUT 1
#define STDIN 0
#define STDERR 2


typedef void dtv_t; // used in tcbhead_t

/* Replacement type for __m128 since this file is included by ld.so,
   which is compiled with -mno-sse.  It must not change the alignment
   of rtld_savespace_sse.  */
typedef struct
{
  int i[4];
} __128bits;

typedef struct
{
  void *tcb;            /* Pointer to the TCB.  Not necessarily the
                           thread descriptor used by libpthread.  */
  dtv_t *dtv;
  void *self;           /* Pointer to the thread descriptor.  */
  int multiple_threads;
  int gscope_flag;
  uintptr_t sysinfo;
  uintptr_t stack_guard;
  uintptr_t pointer_guard;
  unsigned long int vgetcpu_cache[2];
  /* Bit 0: X86_FEATURE_1_IBT.
     Bit 1: X86_FEATURE_1_SHSTK.
   */
  unsigned int feature_1;
  int __glibc_unused1;
  /* Reservation of some values for the TM ABI.  */
  void *__private_tm[4];
  /* GCC split stack support.  */
  void *__private_ss;
  /* The lowest address of shadow stack,  */
  unsigned long long int ssp_base;
  /* Must be kept even if it is no longer used by glibc since programs,
     like AddressSanitizer, depend on the size of tcbhead_t.  */
  __128bits __glibc_unused2[8][4] __attribute__ ((aligned (32)));

  void *__padding[8];
} tcbhead_t;


static int strlen(const char * str);
static int puts(const char * str);
static void hexprint(uint64_t val);

static void* mmap_above(void* addr, uint64_t size, uint64_t prot, uint64_t flags, int fd, uint64_t pgoff);
static uint64_t fsize(int fd);
static void copy_chunk(char* restrict dest, const char* restrict src, uint64_t size);

static uint64_t load_program(Elf64_Phdr* phdr, int phnum, char* file);

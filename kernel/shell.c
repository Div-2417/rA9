//https://brennan.io/2015/01/16/write-a-shell-in-c/
//add dynamic buffer allocation afterwards

#include "shell.h"
#include "tty.h"
#include "kprintf.h"
#include "drivers/keyboard.h"
#include "libc/include/string.h"
#include "drivers/io.h"
#include "pmm.h"

#include <stddef.h>
#include <stdint.h>

extern char _kernel_start[], _kernel_end[];

typedef struct {
    uint32_t size;
    uint64_t addr;
    uint64_t len;
    uint32_t type;
} __attribute__((packed)) multiboot_mmap_entry_t;

#define LineMax 256
#define ArgsMax 16

#define ReadBuff 256
#define TokBuff 256

static int help(int argc, char** argv);
static int clear(int argc, char** argv);
static int echo(int argc, char** argv);
static int meminfo(int argc, char** argv);
static int reboot(int argc, char** argv);

static const char *builtinStr[] = {
  "help",
  "clear",
  "echo",
  "meminfo",
  "reboot"
};

static const char *builtinDesc[] = {
  "list available commands",
  "clear the screen",
  "print arguments",
  "show memory info",
  "restart the system"
};

static int (*builtinFunc[])(int argc, char **argv) = {
  help,
  clear,
  echo,
  meminfo,
  reboot
};

static int num_builtins(void){
    return sizeof(builtinStr) / sizeof(builtinStr[0]);
}

static int help(int argc,char** argv){
  (void)argc;
  (void)argv;

  terminalPutString("Built-in commands:\n");

  for (int i = 0; i < num_builtins(); i++) {
    kprintf("  %s - %s\n", builtinStr[i], builtinDesc[i]);
  }

  return 0;
}

static int clear(int argc, char** argv){
  (void)argc;
  (void)argv;

  terminalClear();

  return 0;
}

static int echo(int argc, char **argv){
  for (int i = 1; i < argc; i++) {
    if (i > 1){
      kprintf(" ");
    }

      kprintf("%s", argv[i]);
  }

  kprintf("\n");
  return 0;
}

static int meminfo(int argc, char** argv){
  (void)argc;
  (void)argv;

  if (!g_multiboot_info) {
      kprintf("Multiboot info not available\n");
      return -1;
  }

  kprintf("Memory Information:\n");
  kprintf("  Lower memory: %u KB\n", g_multiboot_info->mem_lower);
  kprintf("  Upper memory: %u KB\n", g_multiboot_info->mem_upper);
  kprintf("  Total memory: %u KB (%u MB)\n",
  g_multiboot_info->mem_lower + g_multiboot_info->mem_upper,
  (g_multiboot_info->mem_lower + g_multiboot_info->mem_upper) / 1024);

  if (g_multiboot_info->flags & (1 << 6)) {
    kprintf("\nMemory map (from multiboot):\n");
    multiboot_mmap_entry_t* mmap = (multiboot_mmap_entry_t*)g_multiboot_info->mmap_addr;
    uint32_t mmap_end = g_multiboot_info->mmap_addr + g_multiboot_info->mmap_length;

    while ((uint32_t)mmap < mmap_end) {
      kprintf("  Base: 0x%llx, Length: 0x%llx, Type: %u\n",
        mmap->addr, mmap->len, mmap->type);
      mmap = (multiboot_mmap_entry_t*)((uint32_t)mmap + mmap->size + sizeof(mmap->size));
    }
  } else {
    kprintf("\nMemory map not provided by bootloader\n");
  }

  uint32_t kernel_start = (uint32_t)_kernel_start;
  uint32_t kernel_end = (uint32_t)_kernel_end;
  kprintf("\nKernel: 0x%x - 0x%x (%u KB)\n",
  kernel_start, kernel_end, (kernel_end - kernel_start + 1023) / 1024);

  kprintf("\nPMM Information:\n");
  kprintf("  Total blocks: %u\n", pmmGetBlockCount());
  kprintf("  Used blocks: %u\n", pmmGetUsedBlockCount());
  kprintf("  Free blocks: %u\n", pmmGetFreeBlockCount());

  uint32_t pmm_total_blocks = pmmGetBlockCount();
  uint32_t pmm_used_blocks = pmmGetUsedBlockCount();
  uint32_t pmm_free_blocks = pmmGetFreeBlockCount();
  kprintf("  Total memory: %u KB (%u MB)\n",
  pmm_total_blocks * pmmBlockSize / 1024,
  pmm_total_blocks * pmmBlockSize / (1024 * 1024));
  kprintf("  Used memory: %u KB (%u MB)\n",
  pmm_used_blocks * pmmBlockSize / 1024,
  pmm_used_blocks * pmmBlockSize / (1024 * 1024));
  kprintf("  Free memory: %u KB (%u MB)\n",
  pmm_free_blocks * pmmBlockSize / 1024,
  pmm_free_blocks * pmmBlockSize / (1024 * 1024));
  kprintf("  Bitmap size: %u bytes\n", pmmGetBitmapSize());
  kprintf("  Bitmap location: 0x%x\n", (uint32_t)pmmGetBitmapAddr());

  return 0;
}

static int reboot(int argc, char** argv){
    (void)argc;
    (void)argv;

    kprintf("Rebooting...\n");

    outb(0x64, 0xFE);

    asm volatile("cli");
    struct { uint16_t limit; uint32_t base; } __attribute__((packed)) idt_ptr = {0, 0};
    asm volatile("lidt %0" : : "m"(idt_ptr));
    asm volatile("int $0");

    // Should never reach here
    for(;;) asm volatile("hlt");
    return 0;
}

static size_t sReadLine(char* buff, size_t max){
  size_t len= 0;

  for (;;){
    char c= kbReadChar();

    //enter
    if (c == '\n' || c =='\r') {
      terminalPutChar(c);
      buff[len] = '\0';
      return len;
    }else if (c == '\b' || c == 127){   //backspace
      if (len>0) {
        len--;
        terminalPutChar(c);
      }
      continue;
    }else if (c >= 32 && c < 127 && len < max-1) {
      buff[len++] = c;
      terminalPutChar(c);
    }
  }
}

static int tokenizer(char* line, char** argv, int max){
  char* p = line;
  int argc = 0;

  while (*p && argc <max) {
    //skip whitespaves
    while(*p == ' '){
      p++;
    }

    //EOF
    if(*p == '\0'){
      break;
    }

    argv[argc++] = p;
    while (*p != ' ' && *p != '\0') {
      p++;
    }

    if (*p == ' ') {
      *p++ ='\0';
    }
  }

  return argc;
}

static int execute(int argc, char **argv){
  if (argc == 0)
      return 0;

  for (int i = 0; i < num_builtins(); i++) {
      if (strcmp(argv[0], builtinStr[i]) == 0){
          return builtinFunc[i](argc, argv);
      }
  }

  kprintf("Unknown command\n");
  return -1;
}

void sLoop(void){
  char line[LineMax];
  char *argv[ArgsMax];

  kprintf("starting shell....\n");
  kprintf("rA9 shell, Type 'help'.\n");

  for(;;){
    kprintf("> ");
    sReadLine(line, sizeof(line));
    int argc = tokenizer(line, argv, ArgsMax);
    execute(argc, argv);
  }
}

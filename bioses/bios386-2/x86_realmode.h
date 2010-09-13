
/* same trick the Linux kernel uses for the real-mode portion of it's code */
__asm__ (".code16gcc");

#define CODEDATA __attribute__((section(".text")))


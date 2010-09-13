
static inline uint8_t ior_b(const unsigned int p) {
	register uint8_t c; __asm__ __volatile__ ("inb %%dx,%%al" : "=a" (c) : "d" (p)); return c;
}

static inline uint8_t iori_b(const unsigned char p) {
	register uint8_t c; __asm__ __volatile__ ("inb %1,%%al" : "=a" (c) : "i" (p)); return c;
}

static inline uint16_t ior_w(const unsigned int p) {
	register uint16_t c; __asm__ __volatile__ ("inw %%dx,%%ax" : "=a" (c) : "d" (p)); return c;
}

static inline uint16_t iori_w(const unsigned char p) {
	register uint16_t c; __asm__ __volatile__ ("inw %1,%%ax" : "=a" (c) : "i" (p)); return c;
}

static inline uint32_t ior_l(const unsigned int p) {
	register uint32_t c; __asm__ __volatile__ ("inl %%dx,%%eax" : "=a" (c) : "d" (p)); return c;
}

static inline uint32_t iori_l(const unsigned char p) {
	register uint32_t c; __asm__ __volatile__ ("inl %1,%%eax" : "=a" (c) : "i" (p)); return c;
}

static inline void iow_b(const unsigned int p,const uint8_t d) {
	__asm__ __volatile__ ("outb %%al,%%dx" : : "a" (d), "d" (p));
}

static inline void iowi_b(const unsigned char p,const uint8_t d) {
	__asm__ __volatile__ ("outb %%al,%1" : : "a" (d), "i" (p));
}

static inline void iow_w(const unsigned int p,const uint16_t d) {
	__asm__ __volatile__ ("outw %%ax,%%dx" : : "a" (d), "d" (p));
}

static inline void iowi_w(const unsigned char p,const uint16_t d) {
	__asm__ __volatile__ ("outw %%ax,%1" : : "a" (d), "i" (p));
}

static inline void iow_l(const unsigned int p,const uint32_t d) {
	__asm__ __volatile__ ("outl %%eax,%%dx" : : "a" (d), "d" (p));
}

static inline void iowi_l(const unsigned char p,const uint32_t d) {
	__asm__ __volatile__ ("outl %%eax,%1" : : "a" (d), "i" (p));
}


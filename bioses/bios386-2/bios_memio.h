
static inline void memw_b(const uint32_t addr,const uint8_t b) {
	__asm__ __volatile__ (	"es\n" "movb %0,(%%esi)" : /* no outs */ : "r" (b), "S" ((unsigned int)addr));
}

static inline uint8_t memr_b(const uint32_t addr) {
	register uint8_t r;
	__asm__ __volatile__ (	"es\n" "movb (%%esi),%0" : "=r" (r) /* no outs */ : "S" ((unsigned int)addr));
	return r;
}

static inline void memw_w(const uint32_t addr,const uint16_t w) {
	__asm__ __volatile__ (	"es\n" "movw %0,(%%esi)" : /* no outs */ : "r" (w), "S" ((unsigned int)addr));
}

static inline uint16_t memr_w(const uint32_t addr) {
	register uint16_t r;
	__asm__ __volatile__ (	"es\n" "movw (%%esi),%0" : "=r" (r) /* no outs */ : "S" ((unsigned int)addr));
	return r;
}

static inline void memw_l(const uint32_t addr,const uint32_t w) {
	__asm__ __volatile__ (	"es\n" "movl %0,(%%esi)" : /* no outs */ : "r" (w), "S" ((unsigned int)addr));
}

static inline uint32_t memr_l(const uint32_t addr) {
	register uint32_t r;
	__asm__ __volatile__ (	"es\n" "movl (%%esi),%0" : "=r" (r) /* no outs */ : "S" ((unsigned int)addr));
	return r;
}


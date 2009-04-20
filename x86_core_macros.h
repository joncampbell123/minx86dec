
#if defined(ENABLE_64BIT)
static inline uint64_t fetch_u64() {
	const register uint64_t r = *((uint64_t*)(cip));
	cip += 8;
	return r;
}
static inline int64_t fetch_s64() { return (int64_t)(fetch_u64()); }
#endif

static inline uint32_t fetch_u32() {
	const register uint32_t r = *((uint32_t*)(cip));
	cip += 4;
	return r;
}
static inline int32_t fetch_s32() { return (int32_t)(fetch_u32()); }

static inline uint16_t fetch_u16() {
	const register uint16_t r = *((uint16_t*)(cip));
	cip += 2;
	return r;
}
static inline int16_t fetch_s16() { return (int16_t)(fetch_u16()); }

static inline uint8_t fetch_u8() {
	return *cip++;
}
static inline int8_t fetch_s8() { return (int8_t)(fetch_u8()); }

union x86_mrm {
	struct {
		uint8_t		rm:3;
		uint8_t		reg:3;
		uint8_t		mod:2;
	} f;
	uint8_t		raw;
};

union x86_sib {
	struct {
		uint8_t		base:3;
		uint8_t		index:3;
		uint8_t		scale:2;
	} f;
	uint8_t		raw;
};

static inline union x86_mrm fetch_modregrm() {
	union x86_mrm r;
	r.raw = fetch_u8();
	return r;
}

static inline union x86_sib fetch_sib() {
	union x86_sib r;
	r.raw = fetch_u8();
	return r;
}

static int rm_addr16_mapping[8] = {
	MX86_REG_BX,	MX86_REG_BX,	MX86_REG_BP,	MX86_REG_BP,
	MX86_REG_SI,	MX86_REG_DI,	MX86_REG_BP,	MX86_REG_BX	};

static int rm_addr32_mapping[8] = {
	MX86_REG_EAX,	MX86_REG_EBX,	MX86_REG_ECX,	MX86_REG_EDX,
	MX86_REG_ESP,	MX86_REG_EBP,	MX86_REG_ESI,	MX86_REG_EDI	};

static inline void decode_rm(union x86_mrm mrm,struct minx86dec_argv *a,const int addr32) {
	if (mrm.f.mod == 3) {
		a->regtype = MX86_RT_REG;
		a->reg = mrm.f.rm;
		return;
	}
	a->regtype = MX86_RT_NONE;

	if (addr32) {
		a->scalar = 0;
		a->memregsz = 4;
		a->memref_base = 0;

		if (mrm.f.mod == 0) {
			if (mrm.f.rm == 5) {
				a->memregs = 0;
				a->memref_base = fetch_u32();
				return;
			}
		}

		if (mrm.f.rm == 4) { /* SIB byte follows */
			union x86_sib sib;
			sib.raw = fetch_u8();
			if (sib.f.index == 4) {
				a->memregs = 1;
				a->memreg[0] = sib.f.base;
			}
			else {
				a->memregs = 2;
				a->scalar = sib.f.scale;
				a->memreg[0] = sib.f.index;
				a->memreg[1] = sib.f.base;
			}
		}
		else {
			a->memregs = 1;
			a->memreg[0] = rm_addr32_mapping[mrm.f.rm];
		}

		if (mrm.f.mod == 2)
			a->memref_base = fetch_u32();
		else if (mrm.f.mod == 1)
			a->memref_base = (uint32_t)((char)fetch_u8());
	}
	else {
		a->scalar = 0;
		a->memregsz = 2;
		if (mrm.f.mod == 0) {
			if (mrm.f.rm == 6) {
				a->memregs = 0;
				a->memref_base = fetch_u16();
				return;
			}
			a->memref_base = 0;
		}
		else if (mrm.f.mod == 2)
			a->memref_base = fetch_u16();
		else
			a->memref_base = (uint32_t)((char)fetch_u8());

		a->memregs = 2 - (mrm.f.rm >> 2);
		a->memreg[0] = rm_addr16_mapping[mrm.f.rm];
		a->memreg[1] = MX86_REG_SI + (mrm.f.rm & 1);
	}
}

static inline void decode_rm_ex(union x86_mrm mrm,struct minx86dec_argv *a,const int addr32,const int typ) {
	if (mrm.f.mod == 3) {
		a->regtype = typ;
		a->reg = mrm.f.rm;
		return;
	}
	a->regtype = MX86_RT_NONE;

	if (addr32) {
		a->scalar = 0;
		a->memregsz = 4;
		a->memref_base = 0;

		if (mrm.f.mod == 0) {
			if (mrm.f.rm == 5) {
				a->memregs = 0;
				a->memref_base = fetch_u32();
				return;
			}
		}

		if (mrm.f.rm == 4) { /* SIB byte follows */
			union x86_sib sib;
			sib.raw = fetch_u8();
			if (sib.f.index == 4) {
				a->memregs = 1;
				a->memreg[0] = sib.f.base;
			}
			else {
				a->memregs = 2;
				a->scalar = sib.f.scale;
				a->memreg[0] = sib.f.index;
				a->memreg[1] = sib.f.base;
			}
		}
		else {
			a->memregs = 1;
			a->memreg[0] = rm_addr32_mapping[mrm.f.rm];
		}

		if (mrm.f.mod == 2)
			a->memref_base = fetch_u32();
		else if (mrm.f.mod == 1)
			a->memref_base = (uint32_t)((char)fetch_u8());
	}
	else {
		a->scalar = 0;
		a->memregsz = 2;
		if (mrm.f.mod == 0) {
			if (mrm.f.rm == 6) {
				a->memregs = 0;
				a->memref_base = fetch_u16();
				return;
			}
			a->memref_base = 0;
		}
		else if (mrm.f.mod == 2)
			a->memref_base = fetch_u16();
		else
			a->memref_base = (uint32_t)((char)fetch_u8());

		a->memregs = 2 - (mrm.f.rm >> 2);
		a->memreg[0] = rm_addr16_mapping[mrm.f.rm];
		a->memreg[1] = MX86_REG_SI + (mrm.f.rm & 1);
	}
}


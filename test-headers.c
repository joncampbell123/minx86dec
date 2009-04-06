#define DEFINE_OPCODES_STR

#include "minx86dec/types.h"
#include "minx86dec/state.h"
#include "minx86dec/opcodes.h"
#include "minx86dec/opcodes_str.h"
#include <stdio.h>

int main() {
	if (minx86_read_ptr_t_test(NULL+1) != NULL+1) return 1;
	if (minx86_uint8_t_test(9) != 9) return 2;
	if (minx86_uint16_t_test(29) != 29) return 3;
	if (minx86_uint32_t_test(39) != 39) return 4;
#if defined(ENABLE_64BIT)
	if (minx86_uint64_t_test(49) != 49) return 5;
#endif
	if (minx86_optimal_t_test(22) != 22) return 22;

	return 0;
}


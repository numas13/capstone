#include <stdio.h>

#include <capstone/capstone.h>
#include <capstone/platform.h>
#include "cstool.h"

void print_insn_detail_e2k(csh handle, cs_insn *ins)
{
	cs_e2k *e2k;

	if (ins->detail == NULL)
		return;

	e2k = &ins->detail->e2k;

	printf("\top_count: %u\n", e2k->op_count);

	// TODO: e2k
}

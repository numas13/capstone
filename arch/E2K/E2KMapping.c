#include <string.h>
#include <stdlib.h>

#include "E2KMapping.h"
#include "E2KConstants.h"
#include "../../Mapping.h"
#include "../../utils.h"

const char *E2K_group_name(csh handle, unsigned int id)
{
	// TODO: e2k
	return NULL;
}

const char *E2K_insn_name(csh handle, unsigned int id)
{
	// TODO: e2k
	return NULL;
}

const char *E2K_reg_name(csh handle, unsigned int reg)
{
	// TODO: e2k
	return NULL;
}

void E2K_get_insn_id(cs_struct *h, cs_insn *insn, unsigned int opcode)
{
	insn->id = opcode;
}

void E2K_reg_access(const cs_insn *insn, cs_regs regs_read,
		     uint8_t *regs_read_count, cs_regs regs_write,
		     uint8_t *regs_write_count)
{
	// TODO: e2k
}

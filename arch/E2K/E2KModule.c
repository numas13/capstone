#include "E2KDisassembler.h"
#include "E2KInstPrinter.h"
#include "E2KMapping.h"
#include "E2KModule.h"

cs_err E2K_global_init(cs_struct *ud)
{
	ud->printer = E2K_printInst;
	ud->reg_name = E2K_reg_name;
	ud->insn_id = E2K_get_insn_id;
	ud->insn_name = E2K_insn_name;
	ud->group_name = E2K_group_name;
#ifndef CAPSTONE_DIET
	ud->reg_access = E2K_reg_access;
#endif
	ud->disasm = E2K_getInstruction;

	return CS_ERR_OK;
}

cs_err E2K_option(cs_struct *handle, cs_opt_type type, size_t value)
{
	if (type == CS_OPT_MODE)
		handle->mode = (cs_mode)value;

	return CS_ERR_OK;
}

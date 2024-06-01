#ifndef CS_E2K_MAPPING_H
#define CS_E2K_MAPPING_H

#include <capstone/capstone.h>

#include "../../cs_priv.h"

const char *E2K_group_name(csh handle, unsigned int id);
const char *E2K_insn_name(csh handle, unsigned int id);
const char *E2K_reg_name(csh handle, unsigned int reg);
void E2K_get_insn_id(cs_struct *h, cs_insn *insn, unsigned int id);
void E2K_reg_access(const cs_insn *insn, cs_regs regs_read,
		    uint8_t *regs_read_count, cs_regs regs_write,
		    uint8_t *regs_write_count);

#endif


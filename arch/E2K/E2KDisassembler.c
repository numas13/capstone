#include <string.h>
#include <stddef.h> // offsetof macro
#include <stdio.h>
#include "E2KDisassembler.h"
#include "E2KConstants.h"

#include "../../Mapping.h"
#include "../../MathExtras.h"
#include "../../utils.h"

static void init_details(MCInst *MI)
{
	cs_detail *detail = get_detail(MI);
	if (detail) {
		memset(detail, 0, offsetof(cs_detail, e2k) + sizeof(cs_e2k));
	}
}

bool E2K_getInstruction(csh ud, const uint8_t *code, size_t code_len,
			MCInst *inst, uint16_t *size, uint64_t address,
			void *info)
{
	//cs_struct *cs = (cs_struct *)ud;
	cs_buffer_private *priv = (cs_buffer_private *) inst->buffer_private;

	init_details(inst);

	if (priv->e2k_insn_index < priv->e2k_insn_count) {
		// pop decoded instruction
		*size = 0;
	} else {
		// decode new bundle
		uint32_t hs = readBytes32(inst, code);

		*size = (hs & 0xff) * 8 + 8;
		if (code_len < *size) {
			*size = 0;
			return false;
		}

		priv->e2k_insn_index = 0;
		priv->e2k_insn_count = ((hs >> 8) & 0xff) + 1;
	}

	bool is_end = ++priv->e2k_insn_index >= priv->e2k_insn_count;
	CS_INSN_FLAGS_SET(inst->flat_insn, is_end, CS_INSN_FLAG_GROUP_END);

	return true;
}

#include <capstone/platform.h>
#include "../../Mapping.h"
#include "../../utils.h"

#include "E2KInstPrinter.h"
#include "E2KMapping.h"

void E2K_printInst(MCInst *MI, SStream *O, void *Info)
{
	cs_buffer_private *priv = (cs_buffer_private *) MI->buffer_private;
	/* set pubOpcode as instruction id */
	MCInst_setOpcodePub(MI, MCInst_getOpcode(MI));

	SStream_concat(O, "insn%d", priv->e2k_insn_index);

	if (MI->flat_insn->size) {
		SStream_concat(O, " # bundle %d bytes, %d instructions",
				MI->flat_insn->size, priv->e2k_insn_count);
	}
}

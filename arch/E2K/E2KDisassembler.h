#ifndef CS_E2K_DISASSEMBLER_H
#define CS_E2K_DISASSEMBLER_H

#include "../../MCInst.h"

bool E2K_getInstruction(csh ud, const uint8_t *code, size_t code_len,
			MCInst *instr, uint16_t *size, uint64_t address,
			void *info);

#endif


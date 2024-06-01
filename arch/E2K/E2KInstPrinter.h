#ifndef CS_E2K_INSTPRINTER_H
#define CS_E2K_INSTPRINTER_H

#include <capstone/capstone.h>

#include "../../MCInst.h"
#include "../../SStream.h"

void E2K_printInst(MCInst *MI, SStream *O, void *Info);

#endif

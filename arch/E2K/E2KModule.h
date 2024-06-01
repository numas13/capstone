#ifndef CS_E2K_MODULE_H
#define CS_E2K_MODULE_H

#include "../../utils.h"

cs_err E2K_global_init(cs_struct *ud);
cs_err E2K_option(cs_struct *handle, cs_opt_type type, size_t value);

#endif

/* Capstone Disassembler Engine */
/* SuperH Backend by Yoshinori Sato <ysato@users.sourceforge.jp> */

#include <stdio.h>
#include <string.h>

#include <capstone/platform.h>
#include <capstone/capstone.h>

#define WITH_DETAILS

struct platform {
	cs_arch arch;
	cs_mode mode;
	unsigned char *code;
	size_t size;
	const char *comment;
};

static void print_string_hex(const char *comment, unsigned char *str, size_t len)
{
	unsigned char *c;

	printf("%s", comment);

	for (c = str; c < str + len; c++)
		printf("0x%02x ", *c & 0xff);

	printf("\n");
}

static void print_read_write_regs(csh handle, cs_detail *detail)
{
	int i;

	if (detail->regs_read_count > 0) {
		printf("\tRegisters read:");

		for (i = 0; i < detail->regs_read_count; ++i)
			printf(" %s",
				cs_reg_name(handle, detail->regs_read[i]));

		printf("\n");
	}

	if (detail->regs_write_count > 0) {
		printf("\tRegisters modified:");

		for (i = 0; i < detail->regs_write_count; ++i)
			printf(" %s",
				cs_reg_name(handle, detail->regs_write[i]));

		printf("\n");
	}
}

static char *reg_address_msg[] = {
	"Register indirect",
	"Register indirect with predecrement",
	"Register indirect with postincrement",
};

static void print_insn_detail(csh handle, cs_insn *insn)
{
	cs_detail *detail = insn->detail;
	cs_sh *sh = NULL;
	int i;
	int n;

	// detail can be NULL on "data" instruction if SKIPDATA option is turned ON
	if (detail == NULL)
		return;

	sh = &detail->sh;

	if (sh->op_count)
		printf("\top_count: %u\n", sh->op_count);

	for (i = 0; i < sh->op_count; i++) {
		cs_sh_op *op = &(sh->operands[i]);

		switch ((int)op->type) {
		default:
			break;

		case SH_OP_REG:
			printf("\t\toperands[%u].type: REGISTER = %s\n", i,
				cs_reg_name(handle, op->reg));
			break;

		case SH_OP_IMM:
			printf("\t\toperands[%u].type: IMMEDIATE = #%" PRIu64 "\n", i,
			       op->imm);
			break;

		case SH_OP_MEM:
			printf("\t\toperands[%u].type: MEM ", i);
			switch(op->mem.address) {
			case SH_OP_MEM_REG_IND:
			case SH_OP_MEM_REG_POST:
			case SH_OP_MEM_REG_PRE:
				printf("%s REG %s\n",
				       reg_address_msg[op->mem.address - SH_OP_MEM_REG_IND],
				       cs_reg_name(handle, op->mem.reg));
				break;
			case SH_OP_MEM_REG_DISP:
				printf("Register indirect with displacement REG %s, DISP %d\n",
				       cs_reg_name(handle, op->mem.reg),
				       op->mem.disp);
				break;
				
			case SH_OP_MEM_REG_R0:
				printf("R0 indexed\n");
				break;
				
			case SH_OP_MEM_GBR_DISP:
				printf("GBR base with displacement DISP %d\n",
				       op->mem.disp);
				break;
				
			case SH_OP_MEM_GBR_R0:
				printf("GBR base with R0 indexed\n");
				break;

			case SH_OP_MEM_PCR:
				printf("PC relative Address=0x%08x\n",
				       op->mem.disp);
				break;

			case SH_OP_MEM_TBR_DISP:
				printf("TBR base with displacement DISP %d\n",
				       op->mem.disp);
				break;
			case SH_OP_MEM_INVALID:
				break;
			}
			break;
		}

		if (sh->size != 0)
			printf("\t\t\tsize: %u\n", sh->size);

	}

	print_read_write_regs(handle, detail);

	if (detail->groups_count > 0) {
		printf("\tgroups: ");
		for (n = 0; n < detail->groups_count; n++) {
			printf("%s ", cs_group_name(handle, detail->groups[n]));
		}
		printf("\n");
	}

	printf("\n");
}

static bool consistency_checks()
{
	return true;
}

static void test()
{
#define SH4A_CODE \
  "\x0c\x31\x10\x20\x22\x21\x36\x64\x46\x25\x12\x12\x1c\x02\x08\xc1\x05\xc7\x0c" \
  "\x71\x1f\x02\x22\xcf\x06\x89\x23\x00\x2b\x41\x0b\x00\x0e\x40\x32\x00\x0a\xf1\x09\x00"

#define SH2A_CODE \
  "\x32\x11\x92\x00\x32\x49\x31\x00"

	struct platform platforms[] = {
		{
			CS_ARCH_SH,
			(cs_mode)(CS_MODE_SH4A | CS_MODE_SHFPU),
			(unsigned char *)SH4A_CODE,
			sizeof(SH4A_CODE) - 1,
			"SH_SH4A",
		},
		{
			CS_ARCH_SH,
			(cs_mode)(CS_MODE_SH2A | CS_MODE_SHFPU | CS_MODE_BIG_ENDIAN),
			(unsigned char *)SH2A_CODE,
			sizeof(SH2A_CODE) - 1,
			"SH_SH2A",
		},
	};

	uint64_t address = 0x80000000;
	csh handle;
	cs_buffer *buffer;
	int i;
	size_t count;

	if (!consistency_checks())
		abort();

	for (i = 0; i < sizeof(platforms) / sizeof(platforms[0]); i++) {
		cs_err err = cs_open(platforms[i].arch, platforms[i].mode,
				&handle);

		if (err) {
			printf("Failed on cs_open() with error returned: %u\n",
				err);
			abort();
		}

#ifdef WITH_DETAILS
		cs_option(handle, CS_OPT_DETAIL, CS_OPT_ON);
#endif

		buffer = cs_buffer_new(0);
		count = cs_disasm(handle, platforms[i].code, platforms[i].size,
				address, 0, buffer);

		if (count) {
			size_t j;
			cs_insn *insn = buffer->insn;

			printf("****************\n");
			printf("Platform: %s\n", platforms[i].comment);
			print_string_hex("Code: ", platforms[i].code,
				platforms[i].size);
			printf("Disasm:\n");

			for (j = 0; j < count; j++) {
				printf("0x%"PRIx64":\t%s\t%s\n", insn[j].address, insn[j].mnemonic, insn[j].op_str);
#ifdef WITH_DETAILS
				print_insn_detail(handle, &insn[j]);
#endif
			}
			printf("0x%" PRIx64 ":\n", insn[j-1].address + insn[j-1].size);
		}
		else {
			printf("****************\n");
			printf("Platform: %s\n", platforms[i].comment);
			print_string_hex("Code:", platforms[i].code,
				platforms[i].size);
			printf("ERROR: Failed to disasm given code!\n");
			abort();
		}

		cs_buffer_free(buffer);
		cs_close(&handle);
	}
}

int main()
{
	test();

	return 0;
}

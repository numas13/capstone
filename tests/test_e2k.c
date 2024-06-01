#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <capstone/platform.h>
#include <capstone/capstone.h>

struct platform {
	cs_arch arch;
	cs_mode mode;
	unsigned char *code;
	size_t size;
	const char *comment;
};

static csh handle;

static void print_string_hex(const char *comment, unsigned char *str, size_t len)
{
	unsigned char *c;

	printf("%s", comment);
	for (c = str; c < str + len; c++) {
		printf("0x%02x ", *c & 0xff);
	}

	printf("\n");
}

static uint16_t print_bytes(cs_insn *head, uint16_t offset) {
	if (offset < head->size) {
		printf(" 0x%" PRIx64 ":\t", head->address + offset);
		for (int k = 0; k < 2; ++k) {
			uint8_t *s = &CS_INSN_BYTES(head)[offset + k * 4];
			printf("%02x%02x%02x%02x ", s[3], s[2], s[1], s[0]);
		}
		offset += 8;
	} else {
		// no more bytes, print padding
		printf("%32c", ' ');
	}
	return offset;
}

#define E2K_CODE64 \
	"\x02\x03\x00\x00" \
	"\x00\x00\x00\x00" \
	"\x00\x00\x00\x00" \
	"\x00\x00\x00\x00" \
	"\x00\x00\x00\x00" \
	"\x00\x00\x00\x00" \
	\
	"\x01\x00\x00\x00" \
	"\x00\x00\x00\x00" \
	"\x00\x00\x00\x00" \
	"\x00\x00\x00\x00" \
	\
	"\x00\x04\x00\x00" \
	"\x00\x00\x00\x00" \
	\
	"\x07\x00\x00\x00" \
	"\x00\x00\x00\x00" \
	"\x00\x00\x00\x00" \
	"\x00\x00\x00\x00" \
	"\x00\x00\x00\x00" \
	"\x00\x00\x00\x00" \
	"\x00\x00\x00\x00" \
	"\x00\x00\x00\x00" \
	"\x00\x00\x00\x00" \
	"\x00\x00\x00\x00" \
	"\x00\x00\x00\x00" \
	"\x00\x00\x00\x00" \
	"\x00\x00\x00\x00" \
	"\x00\x00\x00\x00" \
	"\x00\x00\x00\x00" \
	"\x00\x00\x00\x00" \
	\
	"\x00\x00\x00\x00" \
	"\x00\x00\x00\x00"

static struct platform platforms[] = {
	{
		CS_ARCH_E2K,
		CS_MODE_E2K64,
		(unsigned char *)E2K_CODE64,
		sizeof(E2K_CODE64) - 1,
		"e2k64"
	}
};

static int test()
{
	uint64_t address = 0x1000;
	cs_buffer *buffer;
	int i, ret = 0;
	size_t count;

	for (i = 0; i < sizeof(platforms) / sizeof(platforms[0]); i++) {
		cs_err err = cs_open(platforms[i].arch, platforms[i].mode, &handle);
		if (err) {
			printf("Failed on cs_open() with error returned: %u\n", err);
			continue;
		}

		//To turn on or off the Print Details option
		cs_option(handle, CS_OPT_DETAIL, CS_OPT_OFF);
		//cs_option(handle, CS_OPT_DETAIL, CS_OPT_ON);

		buffer = cs_buffer_new(0);
		count = cs_disasm(handle, platforms[i].code, platforms[i].size, address, 0, buffer);

		if (count) {
			cs_insn *insn = buffer->insn;

			printf("****************\n");
			printf("Platform: %s\n", platforms[i].comment);
			print_string_hex("Code:", platforms[i].code, platforms[i].size);
			printf("Disasm:\n");

			for (size_t j = 0; j < count;) {
				cs_insn *head = insn;
				uint16_t offset = 0;

				while (j < count) {
					bool stop = CS_INSN_IS_GROUP_END(insn);

					offset = print_bytes(head, offset);
					printf("\t%s\t%s\n", insn->mnemonic, insn->op_str);
					//print_insn_detail(&insn[j]);

					// keep last insn for group end check at the end
					if (++j < count)
						++insn;

					if (stop) {
						break;
					}
				}

				offset = print_bytes(head, offset);
				puts("\t--");

				// print tail bytes
				while (offset < head->size) {
					offset = print_bytes(head, offset);
					putchar('\n');
				}
			}

			if (count && !CS_INSN_IS_GROUP_END(insn)) {
				printf("ERROR: Instruction group end is not set!\n");
				ret = 1;
			}
		} else {
			printf("****************\n");
			printf("Platform: %s\n", platforms[i].comment);
			print_string_hex("Code:", platforms[i].code, platforms[i].size);
			printf("ERROR: Failed to disasm given code!\n");
			ret = 1;
		}

		printf("\n");

		cs_buffer_free(buffer);
		cs_close(&handle);
	}

	return ret;
}

int main()
{
	return test();
}

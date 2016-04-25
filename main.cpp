// PowerPC disassembler.

/* TODO:
	- Investigate stfs instructions! (might be fixed?)
	- Investigate ps_merge10 / ps_deb()
*/

#include <stdio.h>
#include <iostream>

#include "Commondefs.h"
#include "ppcd.h"

#define MODULE_NAME "ppcd-Gekko"
#define MODULE_VER  0.03

using namespace std;

static void usage(void)
{
	printf("/* PPCD PowerPC Disassembler. Version %.2f */\n", MODULE_VER);
	printf("/* (c) 2007, org. */\n\n");
	printf("/* Usage: %s [options] <binary file> */\n\n", MODULE_NAME);
	printf("Possible options are:\n");
	printf("\t-all: All of the following.\n");
	printf("\t-a:   Output the instruction's address.\n");
	printf("\t-b:   Output the instruction's binary value.\n");
	printf("\t-m:   Output the mnemonic.\n");
	printf("\t-op:  Output the operands. (should be used in conjunction with '-m')\n");
}

static u32 FASTCALL Swap32(u32 data)
{
	return ((data >> 24)) |
		((data >> 8) & 0x0000ff00) |
		((data << 8) & 0x00ff0000) |
		((data << 24));
}

int main(int argc, char **argv)
{
	PPCD_CB disa;
	FILE* file = 0;
	u32 pc = 0x00000000, instr;
	u64 LabelCounter = 0; // Used for speeding up 'stage 2' by not starting and 0x00000000 every time a label gets checked.

	u32 branchCalls = 1; // The number of offsets called to in the disassembled code

	struct Flags InputFlags = {};

	if (argc <= 1)
	{
		usage();
		return (0);
	}

	// Get input-arguments
	for (int i = 1; i < argc; i++)
	{
		if (file == 0)
		{
			file = fopen(argv[i], "rb");
		}

		if (!strcmp(argv[i], "-all")) {
			InputFlags.outputAddress = 1;
			InputFlags.outputBinary = 1;
			InputFlags.outputMnemonics = 1;
			InputFlags.outputOperands = 1;
		}
		else if (!strcmp(argv[i], "-a"))
			InputFlags.outputAddress = 1;

		else if (!strcmp(argv[i], "-b"))
			InputFlags.outputBinary = 1;

		else if (!strcmp(argv[i], "-m"))
			InputFlags.outputMnemonics = 1;

		else if (!strcmp(argv[i], "-op"))
			InputFlags.outputOperands = 1;
	}

	if ((!InputFlags.outputAddress)
		&& (!InputFlags.outputBinary)
		&& (!InputFlags.outputMnemonics)
		&& (!InputFlags.outputOperands))
	{
		printf("You didn't enter any arguments\n");
		printf("determining disassembler-options!\n");
		printf("Exit...\n");
		return -1;
	}

	if (file == NULL)
	{
		printf("File could not be found, exit...\n");
		return (0);
	}

	u32 stage;
	if (InputFlags.outputAddress) {
		stage = 0; // TODO: Write a good explanation
	}
	else
	{
		stage = 2;
	}

	u32  oldPC = pc;

	u64* branchLocs = 0; // An array of called offsets

	while (!feof(file))
	{
		fread(&instr, 4, 1, file);

		if (!feof(file))
		{
			disa.pc = pc;
			disa.instr = Swap32(instr);
			PPCDisasm(&disa);

			switch (stage) {

			// Find the number of branch-calls
			case 0: {
				if (disa.target)
				{
					branchCalls++;
					// printf("/* Getting number of branch-calls: %d */\r", branchCalls);
				}
			} break;

			// Store called (sub-)functions in 'BranchLocs'
			case 1: {
				if (branchLocs == 0)
				{
					branchLocs = (u64*)calloc(branchCalls, sizeof(u64));
				}

				if (disa.target)
				{
					u8* copyStart = 0;
					u8* copyEnd = 0;

					for (u64 Counter = 1; Counter < branchCalls; Counter++)
					{
						// If this target isn't in the list yet
						if (branchLocs[Counter] != disa.target)
						{
							if (branchLocs[Counter] == 0)
							{
								branchLocs[Counter] = disa.target;
								break;
							}

							if (branchLocs[Counter] < disa.target)
							{
								continue;
							}
							else
							{
								copyStart = (u8*)(&branchLocs[Counter]);
								while (branchLocs[Counter])
								{
									Counter++;
								}
								copyEnd = (u8*)(&branchLocs[Counter]);

								u8* buffer = (u8*)malloc(copyEnd - copyStart);
								memcpy(buffer, copyStart, copyEnd - copyStart);
								memcpy( (copyStart + sizeof(*branchLocs)), buffer, copyEnd - copyStart);

								*(u64*)copyStart = disa.target;
								free(buffer);
								break;
							}
						}
						else
						{
							branchCalls--;
							break;
						}
					}
					// printf("/* Storing offsets into 'branchLocs', %d left. */\r", branchCalls);
				}
			} break;

			case 2: {
				// Print instruction
				if (InputFlags.outputAddress) {
					for (u64 i = LabelCounter; i < branchCalls; i++)
					{
						if (branchLocs[i] == pc)
						{
							printf("\nloc_%08X:\n", pc);
							LabelCounter = i;
							break;
						}
					}
				}
				if (InputFlags.outputBinary)
					printf("%08X", instr);
				if (InputFlags.outputMnemonics)
					printf("\t%-12s", disa.mnemonic);
				if (InputFlags.outputOperands)
					printf("%s", disa.operands); // was "%-30s" in v0.03 release

				printf("\n");
			} break;

			default: {
				fclose(file);
				return (1);
			} break;
			};

			pc += 4;
		}
		else {
			stage++;
			pc = oldPC;
			printf("\n");
			fseek(file, 0, SEEK_SET);
		}
	}

	fclose(file);
	return (1);
}

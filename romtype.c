#include "RomTypes.h"

typedef unsigned char byte;
typedef unsigned short word;

static enum RomType guessRomType(const unsigned char *rom, int size);

#include <stdio.h>

byte ROM[4 * 1024 * 1024];

void main(int argc, char **argv)
{
	FILE *file;
	int size;
	if (argc > 1)
	{
		file = fopen(argv[1], "rb");
		if (!file)
		{
			printf("File %s does not exist.\n", argv[1]);
			exit(-1);
		}
		fseek(file, 0L, SEEK_END);
		size = ftell(file);
		printf ("File:%s (%d)\n", argv[1], size);
		rewind(file);
		fread(ROM, size, 1, file);
	}
	printf("ROM type: %s\n", ROMTYPE[guessRomType(ROM, size)]);
	fflush(stdout);
}

static enum RomType guessRomType(const unsigned char * rom, int size)
{
	if (size == 0) {
		return ROM_NORMAL;
	}
	const byte* data = &rom[0];

	if (size < 0x10000) {
		if ((size <= 0x4000) &&
		           (data[0] == 'A') && (data[1] == 'B')) {
			word initAddr = data[2] + 256 * data[3];
			word textAddr = data[8] + 256 * data[9];
			if ((textAddr & 0xC000) == 0x8000) {
				if ((initAddr == 0) ||
				    (((initAddr & 0xC000) == 0x8000) &&
				     (data[initAddr & (size - 1)] == 0xC9))) {
					return ROM_PAGE2;
				}
			}
		}
		// not correct for Konami-DAC, but does this really need
		// to be correct for _every_ rom?
		return ROM_MIRRORED;
	} else if (size == 0x10000 && !((data[0] == 'A') && (data[1] == 'B'))) {
		// 64 kB ROMs can be plain or memory mapped...
		// check here for plain, if not, try the auto detection
		// (thanks for the hint, hap)
		return ROM_MIRRORED;
	} else {
		//  GameCartridges do their bankswitching by using the Z80
		//  instruction ld(nn),a in the middle of program code. The
		//  adress nn depends upon the GameCartridge mappertype used.
		//  To guess which mapper it is, we will look how much writes
		//  with this instruction to the mapper-registers-addresses
		//  occur.

		unsigned typeGuess[ROM_END_OF_UNORDERED_LIST] = {}; // 0-initialized
		for (int i = 0; i < size - 3; ++i) {
			if (data[i] == 0x32) {
				word value = data[i + 1] + (data[i + 2] << 8);
				switch (value) {
				case 0x5000:
				case 0x9000:
				case 0xb000:
					typeGuess[ROM_KONAMI_SCC]++;
					break;
				case 0x4000:
					typeGuess[ROM_KONAMI]++;
					break;
				case 0x8000:
				case 0xa000:
					typeGuess[ROM_KONAMI]++;
					break;
				case 0x6800:
				case 0x7800:
					typeGuess[ROM_ASCII8]++;
					break;
				case 0x6000:
					typeGuess[ROM_KONAMI]++;
					typeGuess[ROM_ASCII8]++;
					typeGuess[ROM_ASCII16]++;
					break;
				case 0x7000:
					typeGuess[ROM_KONAMI_SCC]++;
					typeGuess[ROM_ASCII8]++;
					typeGuess[ROM_ASCII16]++;
					break;
				case 0x77ff:
					typeGuess[ROM_ASCII16]++;
					break;
				}
			}
		}
		if (typeGuess[ROM_ASCII8]) typeGuess[ROM_ASCII8]--; // -1 -> max_int
		enum RomType type = ROM_GENERIC_8KB;
		for (int i = 0; i < ROM_END_OF_UNORDERED_LIST; ++i) {
			// debug: fprintf(stderr, "%d: %d\n", i, typeGuess[i]);
			if (typeGuess[i] && (typeGuess[i] >= typeGuess[type])) {
				type = i;
			}
		}
		// in case of doubt we go for type ROM_GENERIC_8KB
		// in case of even type ROM_ASCII16 and ROM_ASCII8 we would
		// prefer ROM_ASCII16 but we would still prefer ROM_GENERIC_8KB
		// above ROM_ASCII8 or ROM_ASCII16
		if ((type == ROM_ASCII16) &&
		    (typeGuess[ROM_GENERIC_8KB] == typeGuess[ROM_ASCII16])) {
			type = ROM_GENERIC_8KB;
		}
		return type;
	}
}
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <direct.h>
#else
#include <sys/stat.h>
#endif

#define u8 unsigned char
#define ERROR "Error: Invalid input!"

typedef struct hdr {
	int form;
	int size1;
	int audo;
	int size2;
	int count;
} hdr;

int main(int argc, char **argv) {
	if (argc != 2) {
		fprintf(stderr, "Usage: %s audiogroup1.dat", argv[0]);
		return 1;
	}

	FILE *input = fopen(argv[1], "rb");

	char *dirName = strtok(argv[1], ".");

#ifdef _WIN32
	_mkdir(dirName);
#else
	mkdir(dirName, 0777);
#endif

	hdr head;

	fread(&head, sizeof(hdr), sizeof(u8), input);

	if (head.form != 'MROF' || head.audo != 'ODUA') {
		fputs(ERROR, stderr);
		return 1;
	}

	int *offsets = calloc(head.count, sizeof(int));

	for (int i = 0; i < head.count; i++)
		fread(&offsets[i], sizeof(int), sizeof(u8), input);

	for (int i = 0, size; i < head.count; i++) {
		fseek(input, offsets[i], 0);
		fread(&size, sizeof(int), sizeof(u8), input);

		u8 *buf = calloc(size, sizeof(u8)),
			*name = calloc(0x100, sizeof(u8));

		sprintf(name, "%s/0x%x-0x%x.wav", dirName, offsets[i], offsets[i] + size);

		printf("Ripping WAV at 0x%x-0x%x...\n", offsets[i], offsets[i] + size);

		FILE *out = fopen(name, "wb");

		fread(buf, size, sizeof(u8), input);
		fwrite(buf, size, sizeof(u8), out);

		fclose(out);

		free(name);
		free(buf);
	}

	fclose(input);

	puts("\nDone!");

	return 0;
}
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#ifdef _WIN32
#include <direct.h>
#else
#include <sys/stat.h>
#endif

#define IOERR "Error: Failed to open \"%s\" for %s!\n"

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

	FILE *input;

	if (!(input = fopen(argv[1], "rb"))) {
		fprintf(stderr, IOERR, argv[1], "reading");
		return 1;
	}

	uint8_t *dirName = strtok(argv[1], "."),
		*name	 = malloc(0x100);

#ifdef _WIN32
	_mkdir(dirName);
#else
	mkdir(dirName, 0777);
#endif

	hdr head;

	fread(&head, sizeof(hdr), 1, input);

	if (head.form != 'MROF' || head.audo != 'ODUA') {
		fputs("Error: Invalid input!", stderr);
		return 1;
	}

	int *offsets = malloc(head.count * sizeof(int));

	fread(offsets, sizeof(int), head.count, input);

	for (int i = 0, size; i < head.count; i++) {
		fseek(input, offsets[i], 0);
		fread(&size, sizeof(int), 1, input);

		uint8_t *buf = malloc(size);

		sprintf(name, "%s/0x%x-0x%x.wav", dirName, offsets[i], offsets[i] + size);

		printf("Ripping WAV at 0x%x-0x%x...\n", offsets[i], offsets[i] + size);

		FILE *out;

		if (!(out = fopen(name, "wb"))) {
			fprintf(stderr, IOERR, name, "writing");
			return 1;
		}

		fread(buf, size, 1, input);
		fwrite(buf, size, 1, out);
		fclose(out);
		free(buf);
	}

	free(name);
	fclose(input);

	puts("\nDone!");

	return 0;
}

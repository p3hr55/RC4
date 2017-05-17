#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_BYTES 100

unsigned char * sched(unsigned char * s, long int * key, int ks) {
	int j = 0, i = 1;
	unsigned char temp;
	for (; i < 256; i++) {
		j = (((j + s[i]) % 256) + key[i % ks]) % 256;
		temp = s[i];
		s[i] = s[j];
		s[j] = temp;
	}
	return s;
}

unsigned char prng(unsigned char * s) {
	int j = 0, i = 0;
	unsigned char temp;
	for (; i < 255; i++) {
		j = (j + s[i]) % 256;
		temp = s[i];
		s[i] = s[j];
		s[j] = temp;
	}
	return (s[i] + s[j]) % 256;
}

unsigned char * run(char * msg, int size, unsigned char * state) {
	unsigned char * msg_dump = (unsigned char *)malloc((size + 1) * sizeof(unsigned char *));
	int i = 0;
	for (; i < size; i++)
		msg_dump[i] = (msg[i] ^ prng(state));
	msg_dump[size] = '\0';
	return msg_dump;
}

char * hstring(char *in) {
	char * t = in, *ret_val = (char *)malloc(strlen(in) * sizeof(char *));
	int i = 0, j = 0;
	for (; i < strlen(in); i++) {
		if (t[i] != '\\' && t[i] != 'x')
			ret_val[j++] = t[i];
	}

	return ret_val;
}

int main(int argc, char * argv[]) {
	unsigned char * S = (unsigned char *)malloc(255 * sizeof(unsigned char *));
	int opt, key_length = 0, i = 0, length_of_data = 0;
	char * msg, *data_in, *end, *data;
	long int key[MAX_BYTES];
	unsigned char * e_msg;

	if (argc < 4) {
		printf("Invalid entry.\nUse \"Application 0/1 \"key\" data\"\n");
		return 1;
	}

	//Valid input so fetch arguments
	msg = argv[3];
	opt = atoi(argv[1]);

	//Set up key
	key[key_length++] = strtol(argv[2], &end, 16);

	while (strcmp(end, "") && key_length < MAX_BYTES)
		key[key_length++] = strtol(end, &end, 16);

	//Set up state array
	for (i = 0; i < 255; i++)
		S[i] = (unsigned char)i;

	//Perform the encryption/decryption
	S = sched(S, key, key_length);

	//Option 1 = Encrypt
	if (opt == 1) {

		//Get length of all arguments
		for (i = 3; i < argc; i++)
			length_of_data += strlen(argv[i]);

		//Allocate Size for the string
		msg = (char *)malloc(length_of_data * sizeof(char *));

		//Copy starter string to msg
		strcpy(msg, argv[3]);

		//Recreate string
		for (i = 4; i < argc; i++) {
			strcat(msg, " ");
			strcat(msg, argv[i]);
		}

		e_msg = run(msg, strlen(msg), S);

		//Print results
		for (i = 0; i < strlen(msg); i++) {
			if ((int)e_msg[i] < 0x10) printf("\\x0%x", e_msg[i]);
			else printf("\\x%x", e_msg[i]);
		}
	}

	//Option 0 = Decrypt
	else if (opt == 0) {

		//Parses hex input into character array
		data_in = hstring(argv[3]);
		data = (char *)malloc(strlen(data_in));
		for (i = 0; i < strlen(data_in) / 2; i++)
			sscanf(data_in + 2 * i, "%02X", &data[i]);

		e_msg = run(data, strlen(msg) / 3, S);

		//Print results
		for (i = 0; i < strlen(msg) / 3; i++)
			printf("%c", e_msg[i]);
	}

	else printf("Invalid flag entered");

	printf("\n");
	return 0;
}
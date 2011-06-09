#include <stdio.h>#include <stdlib.h>#include <stdint.h>#include <unistd.h>void bitout_ui8(uint8_t a){	int i;	for (i = 0; i < 8; i++){		printf("%d",(a>>(7-i))&0x1);	}}void usage(char* binary_name){printf("usage: %s [-pcwv] -i filename\n",binary_name);printf("switches:\n");printf("  -i filename - opens \"filename\"\n");printf("  -p number   - only shows packets with the PID \"number\"\n");printf("  -c          - continues without prompting the user for a key stroke\n");printf("  -v          - verbose. More debugging output.\n");printf("  -w number   - how many 8 bit columns will be displayed per row (default: 4)\n");printf("\n");}int main(int argc, char** argv){	uint16_t pidfilter = 0;	char pidfilter_on='n';	char pusifilter_on='n';	int pusifilter;	char wait = 'y';	char* filename = NULL;	int c;	int verbose = 0;	int width = 4;		while ((c = getopt(argc,argv,"s:w:i:p:cv")) != -1){		switch (c){			case 'p':				pidfilter = atoi(optarg);				pidfilter_on='y';				if (verbose > 0)					printf("set pidfilter to: %u\n",pidfilter);				break;			case 'i':				filename = optarg;				if (verbose > 0)					printf("set filename to: %s\n",filename);				break;			case 'c':				wait = 'n';				break;			case 's':				pusifilter = atoi(optarg);				pusifilter_on = 'y';				break;			case 'v':				verbose++;				break;			case 'w':				width = atoi(optarg);				break;			default:				printf("\n");				usage(argv[0]);				abort();		}	}	if (filename == NULL){		usage(argv[0]);		return 1;	}		FILE *f;	char* data = (char*)malloc(188);	int chk = 0;	int i;	printf("opening '%s'...\n",filename);	if (!(f = fopen(filename,"r"))){		printf("couldn't open file.\n");		return 2;	}		int oldsize = ftell(f);		//sync to syncbyte	while (!feof(f) && chk != 0x47){		chk = fgetc(f);	}		int newsize = ftell(f);	if (oldsize != newsize) { fseek(f,-1,SEEK_CUR); oldsize--; }	if (feof(f)){		fclose(f);		printf("EOF before hitting mainloop.\n");		return 3;	}		while (!feof(f)){		fread(data,188,1,f);		uint16_t PID = (data[1] & 0x1F) << 8 | (data[2] & 0xff);		uint8_t PUSI = (data[1] & 0x40) >> 6;		//printf("PID 0x%04x (pidfilter: 0x%02X) | PUSI 0x%02X (pusifilter: 0x%02X)\n",PID,pidfilter,PUSI,pusifilter);		if ((pidfilter_on == 'y' && pidfilter != PID)) { if (verbose > 0){ printf("skipping PID 0x%04x packet.\n",PID); } }		else if ((pusifilter_on == 'y' && pusifilter != PUSI)) { if (verbose > 0){ printf("skipping PUSI 0x%02x packet.\n",PUSI); } }		else {			printf("dumping a PID 0x%04x (%u) packet. PUSI is 0x%02X\n",PID,PID,PUSI);			i=0;			while (i < 188){				printf("0x%04x ",i);				int j = i;				while (j < i+width && j < 188){					bitout_ui8(data[j]);					printf(" ");					j++;				}				i = j;				printf("\n");			} 			printf("\n\n");			if (wait == 'y') {				printf("\nPress enter to continue, or 'q' to quit.\n");				if (fgetc(stdin) == 'q') return 0;			}		} 	}		fclose(f);	return 0;}
#include "../common.h"
void MPEG_transport_stream(char* filename){
	FILE *f;
	char* data = (char*)malloc(TS_MAXDATA);
	f = fopen(filename,"r");
	sync_to_syncbyte(f);
	if (feof(f)){
		fclose(f);
		printf("EOF before hitting mainloop.\n");
		return;
	}
	
	while (!feof(f)){
		fread(data,TS_MAXDATA,1,f);
		transport_packet(data);
		//sync_to_syncbyte(f);
	}
	
	fclose(f);
}
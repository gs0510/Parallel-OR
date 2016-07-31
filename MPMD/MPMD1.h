typedef struct Ip_packet
{
	int version;
	int ihl;
	 int tos;
	 int tot_len;
	 int id;
	 int ttl;
	 int protocol;
	 int check;	
	char dest[20];
	char source[20];
}ip_packet;





int get_id(void*,int);
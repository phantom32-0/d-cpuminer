#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <openssl/sha.h>

char *my_itoa(int num, char *str) {
	if(str == NULL) return NULL;
    sprintf(str, "%d", num);
    return str;
}

int main (){
	char* serverip = "163.172.179.54"; //declare variables
	unsigned int serverport = 14808;
	char jobmessage[1024] = "JOB,";
	char serverreply[2048];
	int socket_desc;
	struct sockaddr_in server;
	char serverversion[8];
	char username[512];
	
	printf("C Duco Miner\nMade by phantom32 (and some revox's help)\n");

	socket_desc = socket(AF_INET , SOCK_STREAM , 0); //create socket
	if (socket_desc == -1) {
		printf("Error: Couldn't create socket\n");
		return 1;
	}

	printf("\nEnter your DUCO username: "); //ask for duco username
	scanf("%s", username);

	server.sin_addr.s_addr = inet_addr(serverip);
	server.sin_family = AF_INET;
	server.sin_port = htons(serverport); //set server ip and port

	if (connect(socket_desc, (struct sockaddr *)&server, sizeof(server)) < 0) {
		puts("Error: Couldn't connect to the server");
		return 1;
	}

	if( recv(socket_desc, serverversion, 8, 0) < 0) {
		puts("Error: Server version couldn't be received");
		return 1;
	}

	printf("Server is on version: %s\n",serverversion);

	strcat(jobmessage, username); //combine "JOB," and duco username

	while(1) {
		if(send(socket_desc , jobmessage , strlen(jobmessage) , 0) < 0)
		{
			puts("Error: Couldn't send JOB message");
			return 1;
		}

		if(recv(socket_desc, serverreply , 2048 , 0) < 0) //receive JOB
		{
			puts("Error: Server version couldn't be received");
			return 1;
		}

	    char* job = strtok (serverreply, ",");
	    char* work = strtok (NULL, ",");
	    char* diff = strtok (NULL, "");

	    printf("\nRecived data!\n");
		printf("JOB: %s\n", job);
		printf("JOB2: %s\n", work);
		printf("DIFF: %s\n", diff);
		
		char dLS[512] = "";
		
		for(int i=0; i < 100*atoi(diff)+1; i++) {
			char strintohash[128] = "";

			strcat(strintohash, job);

			my_itoa(i, dLS);
			strcat(strintohash, dLS);

			//printf("String to hash: %s\n", strintohash);

			unsigned char temp[SHA_DIGEST_LENGTH];
			char buf[SHA_DIGEST_LENGTH*2];
			memset(buf, 0x0, SHA_DIGEST_LENGTH*2);
			memset(temp, 0x0, SHA_DIGEST_LENGTH);
			SHA1((unsigned char *)strintohash, strlen(strintohash), temp);
			long iZ = 0;
			for (iZ=0; iZ < SHA_DIGEST_LENGTH; iZ++) {
		    	sprintf((char*)&(buf[iZ*2]), "%02x", temp[iZ]);
			}

			//printf("Hashed res    : %s\n", buf);
			//printf("Expected res  : %s\n\n", work);

			if (strcmp(work, buf) == 0) {
				printf("Found share! %s\nSending result...\n\n", dLS);

				if(send(socket_desc, dLS, strlen(dLS), 0) < 0) { //send result
					puts("Error: Couldn't send result");
					return 1;
				}

				char feedback[512];
				if(recv(socket_desc, feedback, 2048, 0) < 0) { // receive feedback
					puts("Error: Feedback couldn't be received");
					return 1;
				}
				printf("Feedback: %s\n", feedback);
				break;
			}
		}
	}
	return 0;
}

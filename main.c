#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <openssl/sha.h>
#include <stdlib.h>
#include <pthread.h>


#ifdef _WIN32 // include sleep library for windows or linux - idk if this can stay that way?
#include <Windows.h>
#else
#include <unistd.h>
#endif

#define serverip "51.15.127.80"

unsigned int rejectedShares = 0; // globals
unsigned int acceptedShares = 0;
unsigned int last_hash_count = 0;
unsigned int khash_count = 0;
unsigned int hash_count = 0;

char *my_itoa (int num, char *str) { // convert int to string function
	if (str == NULL) return NULL;
    sprintf(str, "%d", num);
    return str;
}

void* hashrateCounter(void* p){

	while(1) {
		last_hash_count = hash_count;
 		khash_count = last_hash_count / 1000;
  		hash_count = 0;
  	sleep(1);
  }
}


int main (int argc, char **argv) {
	const unsigned int serverport = 2811; // static server port
	char jobmessage[1024] = "JOB,";
	char serverreply[2048];
	int socket_desc;
	struct sockaddr_in server;
	char serverversion[8];
	char username[512] = "";
	
	printf("\033[1;33md-cpuminer\nMade by phantom32 and revox\n"); //color yellow
	printf("----------\n\033[0m"); //reset color

	socket_desc = socket(AF_INET , SOCK_STREAM , 0); //create socket
	if (socket_desc == -1) {
		printf("Error: Couldn't create socket\n");
		return 1;
	}

	if (argc < 2) {
		printf("Enter your DUCO username (you can also pass it when launching the miner: ./d-cpuminer username): "); //ask for duco username
		scanf("%s", username);
	} else {
		sprintf(username, argv[1], "%s"); // get username from sys argv
	}
	printf("Continuing as user %s\n", username);

	server.sin_addr.s_addr = inet_addr(serverip);
	server.sin_family = AF_INET;
	server.sin_port = htons(serverport); //set server ip and port

	if (connect(socket_desc, (struct sockaddr *)&server, sizeof(server)) < 0) {
		printf("Error: Couldn't connect to the server\n");
		return 1;
	}

	if (recv(socket_desc, serverversion, 8, 0) < 0) {
		printf("Error: Server version couldn't be received\n");
		return 1;
	}

	printf("\nConnected to the server\nServer is on version: %s\n\n",serverversion);

	strcat(jobmessage, username); //combine "JOB," and duco username

	
	printf("Mining started using DUCO-S1 algorithm\n");

	pthread_t id;
	int j = 0;
	pthread_create(&id, NULL, hashrateCounter, &j); // start hashrate counter thread
	while(1) {
		if (send(socket_desc, jobmessage, strlen(jobmessage), 0) < 0) {
			printf("Error: Couldn't send JOB message\n");
			return 1;
		}

		if (recv(socket_desc, serverreply, 2048, 0) < 0) {
			printf("Error: Server version couldn't be received\n");
			return 1;
		}

	    char* job = strtok (serverreply, ",");
	    char* work = strtok (NULL, ",");
	    char* diff = strtok (NULL, "");

	    //printf("\nRecived data!\n");
		//printf("JOB: %s\n", job);
		//printf("JOB2: %s\n", work);
		//printf("DIFF: %s\n", diff);
		
		char dLS[512] = "";
		
		for(int i=0; i < (100*atoi(diff))+1; i++) {
			hash_count++;
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
				//printf("Found share! %s\n", dLS);

				if(send(socket_desc, dLS, strlen(dLS), 0) < 0) { //send result
					printf("Error: Couldn't send result\n");
					return 1;
				}

				char feedback[512] = "";
				if(recv(socket_desc, feedback, 2048, 0) < 0) { // receive feedback
					printf("Error: Feedback couldn't be received\n");
					return 1;
				}

				//printf("Feedback: %s\n", feedback);
				if (strcmp("GOOD", feedback)==0 || strcmp("BLOCK", feedback)==0) {
					acceptedShares++;
					printf("Accepted share #%i (%s) %ikH/s\n", acceptedShares, dLS, khash_count);

				} else if (strcmp("INVU", feedback)==0) {
					printf("Error: Incorrect username\n");
					return 1;

				} else {
					rejectedShares++;
					printf("Rejected share #%i (%s) %ikH/s\n", rejectedShares, dLS, khash_count);
				}
				break;
			}
		}
	}
	return 0;
}

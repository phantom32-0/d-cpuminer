#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <openssl/sha.h>

int main (){
	char* serverip = "163.172.179.54"; //declare variables
	char username[512];
	char jobmessage[1024] = "JOB,";
	char serverreply[2048];
	char message[1024];
	unsigned int serverport = 14808;
	int hashrate;
	int sock;
	int socket_desc;
	struct sockaddr_in server;
	char serverversion[8];
	int difficulty;
	char difficultystring[256];

	printf("C Duco Miner\nMade by phantom32\n");

	socket_desc = socket(AF_INET , SOCK_STREAM , 0); //create socket
	if (socket_desc == -1)
	{
		printf("Error: Couldn't create socket\n");
		return 1;
	}
	printf("\nEnter your DUCO username: "); //ask for duco username
	scanf("%s", username);

	server.sin_addr.s_addr = inet_addr(serverip);
	server.sin_family = AF_INET;
	server.sin_port = htons(serverport); //set server ip and port

	if (connect(socket_desc , (struct sockaddr *)&server , sizeof(server)) < 0) //connect to the server
	{
		puts("Error: Couldn't connect to the server");
		return 1;
	}

	if( recv(socket_desc, serverversion , 8 , 0) < 0) //receive server version
	{
		puts("Error: Server version couldn't be received");
		return 1;
	}

	printf("Connected to Duino-Coin server\n");
	printf("Server is on version: %s\n",serverversion);

	strcat(jobmessage, username); //combine "JOB," and duco username
	printf("Sending JOB message: %s\n",jobmessage);

	if( send(socket_desc , jobmessage , strlen(jobmessage) , 0) < 0)
	{
		puts("Error: Couldn't send JOB message");
		return 1;
	}
	printf("JOB message sent\n");
	if( recv(socket_desc, serverreply , 2048 , 0) < 0) //receive JOB
	{
		puts("Error: Server version couldn't be received");
		return 1;
	}
	printf("JOB: %s\n",serverreply);
	printf("Splitting serverreply to JOB and difficulty...\n");
	char * job = strtok(serverreply, ",");
	//difficultystring[8] = strtok(NULL, ",");
	printf("JOB and difficulty: %s,%s\n",job[0],job[2]);
}

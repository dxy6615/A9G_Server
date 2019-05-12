#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

/* 0: save recv addr, 1: for saving client addr */
static struct sockaddr_in cli_addr[3];
//char data[100] = { 0 };

int main()
{
    int sockfd = -1;
    sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sockfd < 0) {
        perror("socket");
        return -1;
    }
   struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(3344);
    addr.sin_addr.s_addr = inet_addr("172.19.0.220");
    if (bind(sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind");
        return -1;

    }

while(1) {
        static char buff[1024] = { 0 };
	static unsigned char AT_got = 0;	
//        struct sockaddr_in cli_addr;
        socklen_t len = sizeof(struct sockaddr_in);
        recvfrom(sockfd, buff, 1023, 0,
                (struct sockaddr*)&cli_addr[0], &len);
	if (0 == strncmp(buff, "A9G: ", 5))
	{
		if(0 != memcmp((void*)&cli_addr[2], (void*)&cli_addr[0], sizeof(struct sockaddr_in)))
		{
			memcpy((void*)&cli_addr[2],(void*)&cli_addr[0], sizeof(struct sockaddr_in));
			printf("Got A9G addr!\n");
			AT_got = 1;
		}
	}
        if (0 == strcmp(buff, "link")) {
         memcpy((void*)&cli_addr[1], (void*)&cli_addr[0], sizeof(struct sockaddr_in));
           printf("Got client addr!\n");
		snprintf(buff, sizeof(buff), "Server: Linked!\n");
		sendto(sockfd, buff, strlen(buff), 0, (struct sockaddr*)&cli_addr[1], len);
      	}
	else if (0 == memcmp((void*)&cli_addr[1], (void*)&cli_addr[0], sizeof(struct sockaddr_in)))
	{
		printf("Client data:%s\n", buff);
		//strcpy (data, buff);
		sendto(sockfd, buff, strlen(buff), 0, (struct sockaddr*)&cli_addr[2], len);

	}
	if (0 == memcmp((void*)&cli_addr[2], (void*)&cli_addr[0], sizeof(struct sockaddr_in)))
	{
		printf("GPRS data:%s\n", buff);
		//strcpy (data, buff);
		sendto(sockfd, buff, strlen(buff), 0, (struct sockaddr*)&cli_addr[1], len);
	}

	
     	memset(buff, 0, sizeof(buff));
   }

   close(sockfd);
    return 0;
}

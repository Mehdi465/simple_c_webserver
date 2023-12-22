#include <sys/socket.h>
#include <string.h>
#include <fcntl.h>
#include <sys/sendfile.h>
#include <unistd.h>
#include <netinet/in.h>
#include <time.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>


void * handle_client(void* threadId){
	int fd = (int) threadId;
    char buffer[256] = {0};
    recv(fd, buffer, 256, 0);
    char* f = buffer + 5;
    *strchr(f,' ') = 0;
    int opened_fd = open(f, O_RDONLY);
    sendfile(fd, opened_fd, 0, 256);
    close(fd);
    close(opened_fd);
    
}

int main(void){

	struct sockaddr_in addr = {
        AF_INET,
        0x901f,   // Port 8080 in reverse hexa.
        0
    };


	int server_fd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (server_fd == -1) {
		perror("cannot create socket");
		exit(1);
	}

	if (bind(server_fd, &addr, sizeof (addr)) == -1) {
		perror("bind failed");
		close(server_fd);
		exit(-1);
	}

	if (listen(server_fd, 5) == -1) {
		perror("listen failed");
		close(server_fd);
		exit(-1);
	}

	while(1) {
		int client_fd = accept(server_fd, NULL, NULL);


		if (client_fd < 0) {
			perror("accept failed");
			close(server_fd);
			return 1;
		}

		pthread_t thread;

		pthread_create(&thread,NULL,handle_client,(void*)client_fd);
		
		pthread_join(thread,NULL);
		
		close(client_fd);
	}

	close(server_fd);
	return 0;  
}




 

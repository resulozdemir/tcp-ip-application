#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <errno.h>
#include <arpa/inet.h>

void *receive_message(void *client_socket) {
    int socket = *(int *)client_socket;
    char data[1024];
    ssize_t bytes_read;
    
    while (1) {
        memset(data, 0, sizeof(data));
        bytes_read = read(socket, data, sizeof(data));
        
        if (bytes_read == 0) {
            printf("Client closed the connection.\n");
            close(socket);
            exit(0);
        }
        
        if (strstr(data, "ID") != NULL) {		
            printf("%s\n", data);
        } else {
            printf("Client: %s", data);
        }
        
        if (strcmp(data, "exit\n") == 0) {
            break;
        }
    }
    
    return NULL;
}

int main() {
    int client_socket;
    struct sockaddr_in server_address;
    char data[1024];
    pthread_t thread_id;
    
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    
    if (client_socket == -1) {
        printf("Socket creation failed.\n");
        exit(1);
    }
    
    server_address.sin_family = AF_INET; 
    server_address.sin_port = htons(8080);   
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1");
    
    if (connect(client_socket, (struct sockaddr*)&server_address, sizeof(server_address)) == -1) {
        printf("Connection failed.\n");
        exit(1);
    }
    
    pthread_create(&thread_id, NULL, receive_message, &client_socket);
    
    while (1) {
        fgets(data, sizeof(data), stdin);

        write(client_socket, data, strlen(data));
        
    }
    
    pthread_join(thread_id, NULL);
    
    close(client_socket);
    
    return 0;
}

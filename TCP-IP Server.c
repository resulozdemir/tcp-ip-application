#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <errno.h>
#include <signal.h>

#define MAX_CLIENTS 10     

typedef struct {    					
    int socket;     					
    int id;         					
}client_t;

client_t clients[MAX_CLIENTS];       			
int client_count = 0;               			

					  
void add_client(int socket, int id) {     		
    if (client_count == MAX_CLIENTS) {    		
        printf("Maximum number of clients reached.\n");
        close(socket);
        return;
    }
    
    clients[client_count].socket = socket;
    clients[client_count].id = id;
    client_count++;
    
    
    char ID_message[128];
    snprintf(message, sizeof(message), "Your ID is: %d", id); 
    write(socket, message, strlen(message)); 		      
}


void send_message_to_client(int recipient_id, char *message) {	
    for (int i = 0; i < client_count; i++) {
        if (clients[i].id == recipient_id) {
            write(clients[i].socket, message, strlen(message));
            break;
        }
    }
}

void remove_client(int socket) {
    for (int i = 0; i < client_count; i++) {
        if (clients[i].socket == socket) {
            for (int j = i; j < client_count - 1; j++) {
                clients[j] = clients[j + 1];
            }
            client_count--;
            break;
        }
    }
}


void *handle_client(void *arg) {
    int socket = *(int *)arg; 
    char data[1024];		
    ssize_t bytes_read;		
    int recipient_id;   	
    
    
    while (1) {
        memset(data, 0, sizeof(data));
        bytes_read = read(socket, data, sizeof(data));
		
		if (bytes_read == 0) {
            printf("Client closed the connection.\n");
            close(socket);
            return NULL;
        }
		
	    sscanf(data, "%d", &recipient_id);
        send_message_to_client(recipient_id, strchr(data, ' ') + 1);
        	
        if (strcmp(data, "exit\n") == 0 || bytes_read == 0) {
    		printf("Client %d exited.\n", socket);
   			close(socket);
   			remove_client(socket);
    		return NULL;
		}

    }
    close(socket);   				 
    
    return NULL;
}

int main() {
	signal(SIGPIPE, SIG_IGN);	
    int server_socket, client_socket;   
    struct sockaddr_in server_address, client_address;  	
    socklen_t client_length;     				
    pthread_t thread_id;				
    
    server_socket = socket(AF_INET, SOCK_STREAM, 0);      
    
    if (server_socket == -1) {					
        printf("Socket creation failed.\n");
        exit(1);
    }
    
    server_address.sin_family = AF_INET;  	 
    server_address.sin_port = htons(8080);
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address)) == -1) {
        printf("Binding failed.\n");
        exit(1);
    }
    
    if (listen(server_socket, MAX_CLIENTS) == -1) {		
        printf("Listen failed.\n");
        exit(1);
    }
    
    
    while (1) {  						
        client_length = sizeof(client_address);	
        client_socket = accept(server_socket, (struct sockaddr*)&client_address, &client_length);
        
        if (client_socket == -1) {
            printf("Connection accept failed.\n");
            continue;						
        }

        add_client(client_socket, client_count + 1);	
        pthread_create(&thread_id, NULL, handle_client, &client_socket);
        pthread_detach(thread_id);  	
    }
    
    close(server_socket);     		
    
    return 0;
}

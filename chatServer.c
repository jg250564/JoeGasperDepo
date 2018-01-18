//======================================= file = chatServer.c ========
// - A chatroom server- allows for clients of chatroom to send multicast messages
// - Uses a thread to allow for parallel tcp connections
// - Uses multicast to send udp messages
// ===================================================================
//  Compile : gcc -o chatServer chatServer.c -pthread
//====================================================================
//= Execute : chatServer
//====================================================================
#define BSD

#include <stdio.h>          // Needed for printf()
#include <stdlib.h>         // Needed for exit() 
#include <string.h>         // Needed for memcpy() and strcpy() 
#include <fcntl.h>          // Needed for file i/o stuff 
#include <pthread.h>       // Needed for pthread_create() and pthread_exit() 
#include <sys/stat.h>      // Needed for file i/o constants 
#include <sys/types.h>    // Needed for sockets stuff 
#include <netinet/in.h>   // Needed for sockets stuff 
#include <sys/socket.h>   // Needed for sockets stuff 
#include <arpa/inet.h>    // Needed for sockets stuff 
#include <fcntl.h>        // Needed for sockets stuff 
#include <netdb.h>        // Needed for sockets stuff
#include "table.h"        // needed for table
#include "table.c"        // needed for table
//----- Defines -------------------------------------------------------------
#define  PORT_NUM            6020     // Port number for chat server
#define  UDP_PORT            6021     // Port number for multicast
#define  CHAT_GROUP          "224.0.0.3"  //Multicast IP Address 
#define  BUF_SIZE            4096     // Buffer size 
#define  BYE                 "Disconnected from chatRoom\n" 
#define  NOT_OK              "Invalid Command\n" 
#define  HELLO               "Connected to chat server ...\n"
#define  JOIN                "Welcome to chatRoom, commands are: WHO BYE or type a message\n"
#define  USED_NAME           "Name in Use\n"
#define  SENT                "Message sent to chatgroup\n"
int thread_count = 0;                        // amt of threads
pthread_mutex_t count_mutex;                 // for mutex lock
char   ipAdd[30];                           // for client ip addr
int fd;                                    // multicast socket
struct sockaddr_in    addr;                // Multicast address

//----- Function prototypes -------------------------------------------------
void *chat_serve(void *in_arg); // POSIX thread function to handle the service

//===== Main program ======================================================== 
int main(void) {   
	long                  server_s;             // Server socket descriptor   
	struct sockaddr_in   server_addr;          // Server Internet address
	long                  client_s;             // Client socket descriptor   
	struct sockaddr_in   client_addr;          // Client Internet address   
	struct in_addr       client_ip_addr;       // Client IP address   
	socklen_t            addr_len;             // Internet address length   
	pthread_t            thread_id;            // Thread ID   
	int                  retcode;              // Return code
    
// Create a socket, fill-in address information, and then bind it   
	server_s = socket(AF_INET, SOCK_STREAM, 0);   
	server_addr.sin_family = AF_INET;   
	server_addr.sin_port = htons(PORT_NUM);   
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
// Bind the socket to an address   
	retcode = bind(server_s, (struct sockaddr*)&server_addr, sizeof(server_addr));   
	if (retcode < 0) {     
		printf("*** ERROR - bind() failed \n");     
		exit(-1);   
	}
// Set up what looks like an ordinary UDP socket
    if((fd = socket(AF_INET,SOCK_DGRAM,0)) < 0){
		perror("udp socket");
		exit(1);
	}	
// Set up destination addr
    memset(&addr,0,sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(CHAT_GROUP);
	addr.sin_port=htons(UDP_PORT);
   // printf("listening on socket - server_s: %d\n", server_s);
// Set-up the listen   
	listen(server_s, 100);     
	printf(">>> chatServer is running on port %d <<< \n", PORT_NUM);
// Main loop to accept connections and then create   
// a thread to handle the client.
    while(1) {     
		addr_len = sizeof(client_addr);     
		// Accept a connection from a client     
		client_s = accept(server_s, (struct sockaddr *)&client_addr, &addr_len);
        memcpy(&client_ip_addr,&client_addr.sin_addr.s_addr,4);
		strcpy(ipAdd,inet_ntoa(client_ip_addr));
        printf("%s\n",ipAdd);
     
		if (client_s == -1) {       
			printf("ERROR - Unable to create a socket \n");       
			exit(1);     
		}
		else         
			printf("Accepting connection - client_s: %d\n", client_s);

        if (pthread_create(&thread_id, NULL, chat_serve, (void *)client_s) != 0)     
        {       
        	printf("ERROR - Unable to create a thread to handle the GET \n");       
			exit(1);     
		}   
	}
    return(0);	 
}

//===============================================================================
// This is the thread function to handle the service
// =============================================================================
void *chat_serve(void *in_arg)
{
int           client_s;             // Client socket descriptor   
char           in_buf[BUF_SIZE];     // Input buffer   
char           out_buf[BUF_SIZE];    // Output buffer   
int            fh;                   // File handle   
int            buf_len;              // Buffer length for file reads   
char           command[BUF_SIZE];    // Command buffer   
char           word[BUF_SIZE];       // username   
int            retcode;              // Return code   
int            i;                    // for loop
char * token;                        // to parse
char clientIp[30];                   // for ip address


 
// Set client_s to in_arg   
client_s = (long) in_arg;
pthread_mutex_lock(&count_mutex); // protect thread_count with a mutex   
thread_count++;   
pthread_mutex_unlock(&count_mutex);


	while (1) {
		// Receive a join request from a client       
		memset(in_buf,0, BUF_SIZE);       
    	retcode = recv(client_s, in_buf, BUF_SIZE, 0);       // recieve initial command       

		// If the recv() return code is bad then bail-out       
		if (retcode <= 0)                                          
		{         
			thread_count--;         
			close(client_s);         
			pthread_exit(NULL);       
		}
		// Establish string and get the first token       
		token = strtok(in_buf, " ");       
		strcpy(command, token);
		// Parse and output the tokens       
		while(token != NULL)                   
		{           
			token = strtok(NULL, " ");           
				if (token != NULL)               
					strcpy(word, token);       
		}


        if (strcmp(command, "JOIN") == 0) {     // if initially joining room,
                                                // getting username
           printf("Join command recognized\n");           // Debug statement
                       
			strcpy(out_buf,word);                        // copy name         
			if(isFound(out_buf)== 1){                      // check if name is in table
				printf("Username is found\n");                         
				strcpy(out_buf,USED_NAME);                                 
			    retcode =	send(client_s,out_buf,strlen(out_buf)+1,0);//tell client name is in use
				thread_count--;                    // close connection
				close(client_s);                        
				printf("client closed");             // Debug statement
				pthread_exit(NULL);                              
			}else{                                   // if name is not taken...
				//printf("username is available\n");             // Debug statement
				strcpy(clientIp,ipAdd);
            	addToTable(out_buf,clientIp);                           // add to table
				printf("username was added to table\n");       // Debug       
            	printf("%s",showTable());                      // Debug
				strcpy(out_buf,JOIN);                          // copy joined message                                 
         		retcode = send(client_s, out_buf, strlen(out_buf)+1, 0);//send message
			}       
		}else if (strcmp(command, "BYE") == 0) {               // if message is BYE
            strcpy(out_buf,word);
			printf("Bye command recognized\n");               //Debug
			removeFromTable(out_buf);                          // remove from Table
			printf("%s",showTable());                          // Debug
			strcpy(out_buf, BYE);                               // copy bye message
			retcode = send(client_s, out_buf, strlen(out_buf)+1, 0);// send bye message
			thread_count--;                               // reduce thread count
			close(client_s);                      // close socket
			pthread_exit(NULL);                   // exit pthread
        }else if (strcmp(command, "WHO") == 0) {                  // if message is WHO
			printf("Who command recognized\n");               //Debug
			strcpy(out_buf,showTable());            // put table contents into out_buf
            // send table contents
			retcode = send(client_s, out_buf, strlen(out_buf)+1, 0);  
		}else {
			printf("received from client:%s\n",command); // debug          
			strcpy(out_buf,"From:" );                 //  begin message
            strcat(out_buf,word);                    // add username
			strcat(out_buf,"\n");                    // add newline
			strcat(out_buf,command);                 // add message
			sendto(fd,out_buf,sizeof(out_buf),0,(struct sockaddr *)
						 &addr,sizeof(addr));//message multicasted
			strcpy(out_buf,SENT);           // copy sent message 
			retcode = send(client_s, out_buf,strlen(out_buf)+1,0);
            printf("message sent");   
		}      
	}   
	thread_count--;     // reduce thread count    
	close(client_s);     // close socket
	pthread_exit(NULL);    // exit pthread
}


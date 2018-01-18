//==================================================== file = chatClient.c   
//============================================================================= 
// - A chatroom client - allows for connecting to chatServer to send multicast messages
// - Uses a thread to allow for parallel tcp connections
// - Uses multicast to receive udp messages
// ============================================================================
// Compile : gcc -o chatClient chatClient.c
// ============================================================================
// Execute : chatClient username
// ===========================================================================
#include <stdio.h>        // Needed for printf() 
#include <stdlib.h>       // Needed for exit() 
#include <string.h>       // Needed for memcpy() and strcpy() 
#include <sys/types.h>    // Needed for sockets stuff 
#include <netinet/in.h>   // Needed for sockets stuff 
#include <sys/socket.h>   // Needed for sockets stuff 
#include <arpa/inet.h>    // Needed for sockets stuff 
#include <fcntl.h>        // Needed for sockets stuff 
#include <netdb.h>        // Needed for sockets stuff
#include <pthread.h>      // Needed for pthread_create() and pthread_exit()
//----- Defines ---------------------------------------------------------------
#define  PORT_NUM         6020       // Port number used at the server
#define  UDP_PORT         6021 
#define  IP_ADDR    "192.168.130.10"  // IP address of server (* HARDWIRED *)
#define  CHAT_GROUP "224.0.0.3"    // IP address of multicast
#define  MSGBUFSIZE      256
#define  USED_NAME    "Name in Use\n"
//-----Function Prototypes-----------------------------------------------------
void *chatGet();          // POSIX thread to handle multicasting
//===== Main program ========================================================== 
void main(int argc, char* argv[]) 
{   
	int                  client_s;        // Client socket descriptor   
	struct sockaddr_in   server_addr;     // Server Internet address   
	char                 out_buf[4096];   // Output buffer for data   
	char                 in_buf[4096];    // Input buffer for data
	int                  retcode;         // Return code   
	int                  finished = 0;
	pthread_t            thread_id;       // Thread ID

	if(argc == 2)                          // must have 2 argc to begin
	{
		// >>> Step #1 <<<   
		// Create a client socket   
		//   - AF_INET is Address Family Internet and SOCK_STREAM is streams   
		client_s = socket(AF_INET, SOCK_STREAM, 0);   
		if (client_s < 0)   
		{     
			printf("*** ERROR - socket() failed \n");     
			exit(-1);   
		}
        
    	// >>> Step #2 <<<   
    	// Fill-in the server's address information and do a connect with the   
    	// listening server using the client socket - the connect() will block.  
    	// 
    	server_addr.sin_family = AF_INET;           // Address family to use   
    	server_addr.sin_port = htons(PORT_NUM);           // Port num to use   
		server_addr.sin_addr.s_addr = inet_addr(IP_ADDR); // IP address to use
  
    	retcode = connect(client_s, (struct sockaddr *)&server_addr,                                                          sizeof(server_addr));

    	if (retcode < 0)   
		{     
			printf("*** ERROR - connect() failed \n");     
			exit(-1);   
		}

        // create thread for multicast 
		pthread_create(&thread_id,NULL,chatGet,NULL);

    	// >>> Step #3 <<<
    	//
    	char joinR[4069];                      // join request buffer
		strcpy(joinR,"JOIN ");                
		strcat(joinR,argv[1]);                // Add username to JOIN
		printf("Sent to server:%s\n ",joinR);      // Debug
    	// Send to the server using the client socket   
    	retcode = send(client_s, joinR, strlen(joinR), 0);   
    	if (retcode < 0)   
		{     
			printf("*** ERROR - send() failed \n");     
			exit(-1);   
		}
    
		while (!finished)        
		{
                  
			// Receive from the server using the client socket       
			retcode = recv(client_s, in_buf, sizeof(in_buf), 0);       
			if (retcode < 0)       
			{         
				printf("*** ERROR - recv() failed \n");         
				close(client_s);         
				exit(-1);       
			}  
        	printf(" %s \n", in_buf);      // Debug code
	    	if(strcmp(in_buf,USED_NAME)==0)                // Close connection if
            {                                              // name is in use
                printf("Name is in use, please try different username\n");
				close(client_s);                        
				exit(-1);
			}
         
	
		// Send to the server using the client socket       
		
			fgets(out_buf, 1024, stdin);              // get input from
                                                       // keyboard
			out_buf[strlen(out_buf)-1] = '\0';                      
        	retcode =  send(client_s,out_buf,strlen(out_buf),0); // send retcode
    		if (strcmp(out_buf, "BYE") == 0)             // if out_buf is BYE...
            {      
				printf("Client Leaving Chatroom\n");  // Closing message
				finished = 1;                           // done
			}       
	       
			if (retcode < 0)       
			{         
				printf("*** ERROR - send() failed \n");         
				exit(-1);       
			}       
  
		}
		   
		// >>> Step #5 <<<   
		// Close the client socket   
		retcode = close(client_s);   
		if (retcode < 0)   
		{     
			printf("*** ERROR - close() failed \n");     
			exit(-1);   
    	}
	}
}
//========================================================================
// UDP multicast listening service
//========================================================================
void *chatGet()
{
	struct sockaddr_in addr;
	int fd, addrlen, nbytes;
	struct ip_mreq mreq;
	char msgbuf[MSGBUFSIZE];

	u_int yes=1;

	// create what looks like an ordinary UDP socket 
	if((fd = socket(AF_INET,SOCK_DGRAM,0)) < 0)
	{
		perror("socket");
		exit(1);
	}

	// Allow multiple sockets to use the same port#
	if(setsockopt(fd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(yes)) < 0)
	{
		perror("Reusing ADDR failed");
		exit(1);
	}

    // set up destination address
    memset(&addr,0,sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(UDP_PORT);
	
	// bind to receive address
	if(bind(fd,(struct sockaddr *) &addr,sizeof(addr)) < 0)
	{
		perror("bind");
		exit(1);
	}

	// use setsockopt() to request that the kernel join a multicast group
	mreq.imr_multiaddr.s_addr = inet_addr(CHAT_GROUP);
	mreq.imr_interface.s_addr = htonl(INADDR_ANY);
	if(setsockopt(fd,IPPROTO_IP,IP_ADD_MEMBERSHIP, &mreq,sizeof(mreq)) < 0)
	{
		perror("setsockopt");
		exit(1);
	}

	// now just enter a read-print loop
	while(1)
	{
		addrlen = sizeof(addr);
	    if((nbytes= recvfrom(fd,msgbuf,MSGBUFSIZE,0,
					(struct sockaddr *) &addr,&addrlen)) < 0)
		{
			perror("recvfrom");
			exit(1);
		}
		puts(msgbuf);
	}
} 

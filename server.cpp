/**************************************************************************************************/
/* Author:              Tue Doan                              */
/* Major:               Software Development                  */
/* Creation Date:       November 16, 2019                     */
/* Due Date:            November 30, 2019                     */
/* Course:              CSC328 010                            */
/* Professor:           Dr. Lisa Frye                         */
/* Assignment:          Download server project               */
/* Filename:            Assignment6.cpp                       */
/* Purpose:             Phase 2 for download server           */
/* Language:            c++                                   */
/* Compile              Command: g++ -o server server.cpp     */
/*                      For server & client: make             */
/* Execute Command:     .server <port>                        */
/* Partner:             Logan Atiyeh & Brett Biever           */
/**************************************************************************************************/

#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include "message.h"
#include "cerrors.h"

using namespace std;


// Function prototypes
/***************************************************************************************************************/
/* Function name:   Client                                             */
/* Description:     sub-thread for each client                         */
/* Parameters:      void *arg - client sockaddrr structure             */
/* Return Value:    none                                               */
/****************************************************************************************************************/
void* Client(void* arg);

/***************************************************************************************************************/
/* Function name:   sendHello                                          */
/* Description:     send a hello message to client                     */
/* Parameters:      int csockfd - file descriptor for active socket    */
/* Return Value:    bool-  returns ture when hello is received,        */
/*                  otherwise false                                    */
/****************************************************************************************************************/
bool sendHello(int csockfd);

/***************************************************************************************************************/
/* Function name:   sendDir                                            */
/* Description:     send the directory to the client                   */
/* Parameters:      int csockfd - file descriptor for active socket    */
/* Return Value:    none                                               */
/****************************************************************************************************************/
void sendDir(int csockfd);

/***************************************************************************************************************/
/* Function name:   changeDir                                          */
/* Description:     change the directory                               */
/* Parameters:      int csockfd - file descriptor for active socket    */
/* Return Value:    none                                               */
/****************************************************************************************************************/
void changeDir(int csockfd);

/***************************************************************************************************************/
/* Function name:   download                                           */
/* Description:     let the client downloads a file                    */
/* Parameters:      int csockfd - file descriptor for active socket    */
/* Return Value:    none                                               */
/****************************************************************************************************************/
void download(int csockfd);

struct clientSock
{
	int sockfd;
	sockaddr_in client;
};

main(int argc, char *argv[]) {
    struct sockaddr_in server , client;  // sock_addr stucts for holding address and ports
    int lsockfd, csockfd, cSockSize;     // ints for file descriptors and size of socket structure
    
        
    server.sin_family = AF_INET;         // set address family
    server.sin_addr.s_addr = INADDR_ANY; // set address of server
		
    // check if a valid port was passed, otherwise use the default port
    if(argc == 2 && atoi(argv[1]) > 1024 && atoi(argv[1]) < 65535) {
        server.sin_port = htons(atoi(argv[1]));
    } else {
        server.sin_port = htons(53532);
    }

    // create endpoint socket and store the file descriptor and check for error
    lsockfd = socket(AF_INET, SOCK_STREAM, 0);
    checkError(lsockfd, "socket");
    
    // bind address to socket and check for error
    if(bind(lsockfd, (struct sockaddr*)&server, sizeof(server))) {
        perror("Error in binding");
        exit(-1);
    }
    // set port to passive mode and check for error
    if(listen(lsockfd, 3) == -1) {
        perror("Error in listening");
        exit(-1);
    }
        
    cSockSize = sizeof(struct sockaddr_in);  // store size of sockaddr_in
        
    // promt when wating for a connection
    printf("Waiting for connection\n");
    
    while(true) {
        pthread_t tid;      // used to store thread id for pthread_create
        
        // accept client and get new active socket
        csockfd = accept(lsockfd, (struct sockaddr*)&client, (socklen_t*)&cSockSize);
        if(csockfd == -1) {
				perror("Error in accepting connection");
				exit(-1);				
        }
			
        clientSock cs;
			
        cs.sockfd = csockfd;
        cs.client = client;
			
        int en = pthread_create(&tid, NULL, Client, &cs);
        if(en != 0) {
            printf("Error creating thread: %s", strerror(en));
        }
            
    }

    close(lsockfd);
    return 0;
}

void* Client(void* arg) {
	int csockfd = ((clientSock*)arg)->sockfd;
	sockaddr_in client = ((clientSock*)arg)->client;
	
	string message = "";    // for storing messages that are passed by the client
    char buffer[512];
        
	// if client get hello message enter connection section
    if(sendHello(csockfd)) {
        // prompt that there is a connection
        printf("connected to %s\n", inet_ntoa(client.sin_addr));
			
        // make sure message is an empty string
        message = "";
                  
        // the main message loop for the connection
        while(message != "BYE") {
            getMessage(csockfd, buffer);  // get message from client
            message = buffer;
                                
            if(message == "DIR") {
					sendDir(csockfd);
            }
            
            if(message == "PWD") {
                getcwd(buffer, 512);
                sendMessage(csockfd, buffer);
            }
            
            if(message == "DOWNLOAD") {
					download(csockfd);
            }
            
            if(message == "CD") {
					changeDir(csockfd);
            }
				// if bye message, close connection with the client	
            if(message == "BYE") {
                cout << "BYE" << endl;
                close(csockfd);
                printf("connection to %s closed\n", inet_ntoa(client.sin_addr));
            }
        }
    }
   
    pthread_exit(NULL);
}

void download(int csockfd) {
	char filePath[512]; // store the file path
	void* buffer[512];
		
	int byteCount = 0;
	bool exist;
	bool ready;
	bool crv = true;
    int rv;
	
	getMessage(csockfd, filePath); // get the file path
	int fd = open(filePath, O_RDWR); // open the file path
	
	if(fd == -1) {
        checkError(fd, "open");
		exist = false;
		rv = write(csockfd, &exist, 1);
        checkError(rv, "write");
	} else {
		exist = true;
		rv = write(csockfd, &exist, 1);
        checkError(rv, "write");
	}
	
    // if the file exist
	if(exist) {
		rv = read(csockfd, &ready, 1);
        checkError(rv, "read");
        
		if(ready) {
			cout << "download" << endl;
			do {
				byteCount = read(fd, buffer, 512);
                checkError(byteCount, "read");
                
				    do {
						rv = write(csockfd, &byteCount, 4);
                        checkError(rv, "write");
                        
						rv = read(csockfd, &crv, 1);
                        checkError(rv, "read");
                        
					} while(crv);
	
				sendMessage(csockfd, buffer);
		
			}while(byteCount);
		}
		close(fd);
	}
}

void changeDir(int csockfd) {
	char buffer[512];
	bool change;
    int rv;
	
	getMessage(csockfd, buffer); // get the directory from the client
	
    // true if changed working dir, otherwise false
    if(chdir(buffer) != -1) {
		change = false;
    } else {
		change = true;
    }
	rv = write(csockfd, &change, 1);
    checkError(rv, "write");
}

void sendDir(int csockfd) {
    char message[512] = "";
    char buffer[512] = "";
    bool done = true;
    DIR *dp = opendir(".");
    struct dirent *dirp;
    int rv;
        
    dirp = readdir(dp);         // read a directory
    while (dirp != NULL) {
        sendMessage(csockfd, dirp->d_name); // send the name of directory
                        
        dirp = readdir(dp); // read a directory again
                
        if(dirp == NULL) {
            done = false;
        }
        
        rv = write(csockfd, &done, 1);
        checkError(rv, "write");
    }
        
    closedir(dp); // close
}

bool sendHello(int csockfd)
{
    char buffer[] = "HELLO";        // store hello message
    int sendSize = sizeof(buffer);  // store size of hello message
    bool crv = true;                // client return value
    int rv;
        
    // send the size of the message to the client
    // resend the size until the client received the size
    do {
        rv = write(csockfd, &sendSize, 4);
        checkError(rv, "write");
        
        rv = read(csockfd, &crv, 1);
        checkError(rv, "read");
                
    } while(crv);
        
    crv = true; // make sure client return value is true
        
    // send the hello message to the client
    // resend the hello message until the client received full message
    do {
        rv = write(csockfd, &buffer, sendSize);
        checkError(rv, "read");
        
        read(csockfd, &crv, 1);
        checkError(rv, "read");
        
    } while(crv);
        
    return true; // return true when client receives message
}




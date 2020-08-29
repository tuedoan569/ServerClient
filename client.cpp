/********************************************************************************************************/
/* Author:              Brett Biever                               */
/* Major:               Software Development                       */
/* Creation Date:       November 16, 2019                          */
/* Due Date:            November 30, 2019                          */
/* Course:              CSC328 010                                 */
/* Professor:           Dr. Lisa Frye                              */
/* Assignment:          Download server project                    */
/* Filename:            Assignment6.cpp                            */
/* Purpose:             Phase 2 for download server                */
/* Language:            c++                                        */
/* Compile              Command: g++ -o server server.cpp          */
/*                      For server & client: make                  */
/* Execute Command:     .client acad.kutztown.edu <optional port>  */
/* Partner:             Logan Atiyeh & Tue Doan                    */
/*********************************************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <string>
#include <ctype.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h> 
#include <netdb.h>
#include <iostream>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include "message.h"
#include "cerrors.h"

using namespace std;

#define SIZE 512
void getHelloMessage(int sockfd);  //Get hello message function
void sendByeMessage(int sockfd);  //send bye message function
void displayMenu(); //display the list of available command 
void getPWD(int sockfd); //get the current directory path in the server
void getDIR(int sockfd); //get the listing in the directory in the server
void changeCD(int sockfd, char path[]); //change the directory in the server
void Download(int sockfd); //download the file if it's found 
bool DoesFileExist(char* filename); //check if file is exists
void getLPWD(); // get the current directory path locally
void getLDIR(); // get the listing in the directory locally
void changeLCD(char *path); // change the directory locally
 
int main(int argc , char *argv[])
{
    int sockfd, rv, defaultPortNum = 53532; // socket file descriptor and set a default port number
    char input[10];     //user command input
    FILE *file;         //create a file
    char path[512]; //to hold the directory path
    
    struct sockaddr_in *servaddr;
    struct addrinfo *servaddrinfo = (struct addrinfo *)malloc(sizeof(struct addrinfo));
    struct hostent *hostEnt = (struct hostent *)malloc(sizeof(struct hostent));
    socklen_t len = sizeof(servaddr); // size of server address
    

    servaddrinfo->ai_family = AF_INET;
    servaddrinfo->ai_flags = 0;
    servaddrinfo->ai_protocol = 0;
    servaddrinfo->ai_socktype = SOCK_STREAM;
    
    if(argc == 2 || argc == 3){
        if(argc ==3 && (atoi(argv[2])<1024 || atoi(argv[2])>=65535)){
            checkClientUsage(argv[0]);
            exit(1);
        }
    } else {
        checkClientUsage(argv[0]);
        exit(1);
    }
   
    getaddrinfo(argv[1], NULL, servaddrinfo, &servaddrinfo);
    servaddr = (struct sockaddr_in *)servaddrinfo->ai_addr;
    
  if (argc == 2) { // when port number is not given, use default port number
    servaddr->sin_port = htons(defaultPortNum);
      
  } else if(argc ==3){ // check port number if port number is given
    servaddr->sin_port = htons(atoi(argv[2]));
      
  } else { // otherwise print the usage and exit the program
      checkClientUsage(argv[0]);
      exit(1);
  }
       
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    checkError(sockfd, "socket");
    /*
  //Create socket
  if((sockfd = socket(AF_INET , SOCK_STREAM , 0))==-1){
    perror("Could not create socket");
    exit(1);
  }*/
 
  //Connect to remote server
  if (connect(sockfd, (struct sockaddr *) servaddr , sizeof(struct sockaddr)) < 0){
    perror("connection error"); //if client can't connect, print error message

    exit (1);
  }
  else{ 
    getHelloMessage(sockfd); //call getHelloMessage
    printf("Connected to %s",inet_ntoa(servaddr->sin_addr));//printout IP address that the cliented connected to
    printf("\n");        
                
  }
        
  bool loop = true; //set loop to true
  displayMenu(); 
  
  //loop until the user want to terminate the session by command bye
  while(loop){
    
    printf("\n");  
    printf("Please enter a command\n"); //ask for command input
    scanf("%s",input);//scan command input 
                

    //All commands are NOT case sensitive            
    //if PWD/pwd call getPWD
    if ((strcmp(input, "PWD")==0) || (strcmp(input, "pwd")==0)){
      getPWD(sockfd);//
    }     
	//if DIR/dir call getDIR
    else if ((strcmp(input,"DIR") == 0) || (strcmp(input,"dir") == 0)){
      getDIR(sockfd);
    } //if cd/CD, ask user to input directory, if available, change to that directory, if it's not, stay in current directory
    else if ((strcmp(input,"CD") == 0) || (strcmp(input,"cd") == 0)){
      printf("Enter directory\n"); 
      scanf("%s",path);
        changeLCD(path);
    }
    else if ((strcmp(input,"LPWD") == 0) || (strcmp(input,"lpwd") == 0)){
        getLPWD();
    }
    else if ((strcmp(input,"LDIR") == 0) || (strcmp(input,"ldir") == 0)){
        getLDIR();
    }
    else if ((strcmp(input,"LCD") == 0) || (strcmp(input,"lcd") == 0)){
        printf("Enter directory\n");
        scanf("%s",path);
        changeCD(sockfd,path);
    }//if DOWNLOAD/download call Download
    else if ((strcmp(input,"DOWNLOAD")==0) || (strcmp(input,"download")==0)){
      Download(sockfd);   
    }//if Bye/bye call sendByeMessage
    else if( (strcmp(input, "BYE")==0) || (strcmp(input, "bye")==0)){
      sendByeMessage(sockfd);
      loop = false;
    }
    else{ //if invalid command is entered, reprint the menu
      printf("INVALID COMMAND\n");               
      displayMenu();
    }};

  //print disconnect message with IP Address
  printf("Disconnected from %s", inet_ntoa(servaddr->sin_addr));
  printf("\n");

  close(sockfd);
     
}


/*
Function Name: 	Download()
Description: 	Send the download command and send the file name
Parameters:		int socketfd
*/


void Download(int sockfd)
{
  char message[] = "DOWNLOAD";  //message to DOWNLOAD
  char filePath[512]; //hold file name
  void* buffer[512]; //hold the content when download is occuring
  bool ExistsOnServer; //boolean if file is exist
  bool Ready = true; //if server is read to send
  int byteCount = 0; //byte count 
  int byteRecv;
  bool again = true;  
  
  sendMessage(sockfd, message); //call sendMessage to send command
  
  cout << "Enter file to download" << endl; //ask what file to download
  cin >> filePath; //user inputs filename
  
  sendMessage(sockfd, filePath); //send file name
  
  read(sockfd, &ExistsOnServer, 1); //read if file is exist
  if(ExistsOnServer)
    {
      if(DoesFileExist(filePath)) //if file is already exists
	{
	  string overWrite; //string for user input
	  //prompt if user wants to overwrite
	  cout << "File already exits would you like to overwrite? yes or no" << endl;
	  cin >> overWrite; //read input
	  
	  //if yes, download 
	  if(overWrite == "yes" || overWrite == "YES")
	    {
	      Ready = true;
	      if(unlink(filePath) == -1)
		{
		  perror("Error: ");
		  exit(-1);
		}
	    } //if no, set ready to false 
	  else if(overWrite == "no" || overWrite == "NO")
	    {
	      Ready = false;
	    }
	  else //incorrect input
	    {
	      Ready = false;
	      cout << "Must enter <yes> or <no>" << endl;
	    }
	}
      
      write(sockfd, &Ready, 1); //let the server know to download or not
      
      if(Ready) //if ready, start the download
	{
	  int fd = open(filePath, O_CREAT | O_WRONLY, 0666); //create a file and change permission
	  
	  cout << "downloading..." << endl;
	  do
	    {
	      do
		{
		  byteRecv =  read(sockfd, &byteCount, 4); //receive the size
               
		  if(byteRecv == 4)
		    again = false;
		  else
		    again = true;
               
		  write(sockfd, &again, 1); //let the server know to receive all the message 
               
		}while(again);
	      

	      getMessage(sockfd, buffer); //call getMessage

	      write(fd, buffer, byteCount); //write into file
	      
	    }while(byteCount);
	  
	  cout << "download complete" << endl;
	  close(fd); //close fd
	}
    }
  else
    cout << "File does not exist in the directory" << endl;
}

/*
Function Name: 	DoesFileExist()
Description:	Check to see if file exist
Parameters:		char* filename
Return Value:	Return boolean
*/

bool DoesFileExist(char* filename)
{
  FILE *file;
  if(file = fopen(filename, "r"))
    {
      fclose(file);
      return true;
    }

  return false;
}

/*
Function Name: 	getHelloMessage()
Description:	Get the hello message from the server
Parameters:		int socketfd
*/


void getHelloMessage(int sockfd){
        
  char buffer[10]; // initalize buffer to 10
  //call getMessage and print out hello message from server
  getMessage(sockfd,buffer);
  printf("Message from server: %s",buffer);
  printf("\n");    
}

/*
Function Name: 	sendByeMessage()
Description:	Send bye message to the server to termiate the connection
Parameters:		int sockfd
*/  
  
void sendByeMessage(int sockfd){
  char buffer[] = "BYE";  // message 
        
  sendMessage(sockfd,buffer);
}
    
/*
Function Name: 	displayMenu()
Description:	Display a list of available commmand
Parameters:		None
*/   
void displayMenu(){
  printf("List of available command line:\n");
  printf("PWD - get server current directory\n");
  printf("DIR - get server directory listing\n");
  printf("CD - change server directory\n");
  printf("LPWD - get current directory locally\n");
  printf("LDIR - get directory listing locally\n");
  printf("LCD - change directory locally\n");
  printf("DOWNLOAD - download requested file\n");
  printf("BYE - disconnect from server\n");
}

/*
Function Name: 	getPWD()
Description:	Send the PWD command and get the path of the current directoryand print
Parameters:		int sockfd
*/

void getPWD(int sockfd){
  char message[]= "PWD";
   
  char buffer[512]; // initalize buffer to 10
        
  sendMessage(sockfd,message);
  getMessage(sockfd,buffer);
  printf("%s",buffer);
  printf("\n");
}

/*
Function Name: 	getDir()
Description:	Send thee DIR command and get the listing and print
Parameters:		int sockfd
*/
 
void getDIR(int sockfd){
        
  char message[]= "DIR"; //initalize messase to DIR
  char bufferDIR[512]=""; // initalize buffer to 512
  bool con = true;
  sendMessage(sockfd,message);
        
  //loop until con is empyt
  while(con){
    //call get message
    getMessage(sockfd,bufferDIR);
    //print out the listing
    printf("%s",bufferDIR);
    printf("\n");
    read(sockfd,&con,1);//read if it is the end of listing
    if(con == 0){
      con = false; //set con to false
    }
  }
        
}

/*
Function Name: 	changeCD()
Description:	Send change directoty (CD) command and the directory path
Parameters:		int sockfd, char path[]
Return Value:	None
*/
 
void changeCD(int sockfd, char path[])
{
  char message[]= "CD"; //initalize message to CD
  char buffer[512]; //hold the message
  bool ok=false; //boolean if change is succesful or not
  sendMessage(sockfd,message);
  sendMessage(sockfd,path);
        
  read(sockfd,&ok,1); //read if change is successful or not
  if(ok == 0){
    printf("Change successful");
  }
  else{
    printf("Not valid directory");
  }
  
}
/*
Function Name:     getLPWD()
Description:       Get the path of the current directoryand print
Parameters:        none
*/
void getLPWD() {
    char buffer[512];
    getcwd(buffer, 512);
    cout << buffer;
}

/*
Function Name:     getLDIR()
Description:       Get the listing and print
Parameters:        none
*/
void getLDIR() {
    
    bool done = true;
    DIR *dp = opendir(".");
    struct dirent *dirp;
 
    dirp = readdir(dp);         // read a directory
    while (dirp != NULL) {
        
        cout << dirp->d_name << endl;
        dirp = readdir(dp); // read a directory again
    }
          
    closedir(dp); // close
}

/*
Function Name:     changeLCD()
Description:       Change the current working directory
Parameters:        char *path - path to the new directory
Return Value:    None
*/
void changeLCD(char *path) {
    // true if changed working dir, otherwise false
    if(chdir(path) != -1) {
        cout << "Change successful!" << endl;
    } else {
        cout << "Not valid directory!" << endl;
    }

}

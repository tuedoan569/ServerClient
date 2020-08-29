/**************************************************************************************************/
/* Author:              Logan Atiyeh                          */
/* Major:               Information Technology                 */
/* Creation Date:       November 16, 2019                     */
/* Due Date:            November 30, 2019                     */
/* Course:              CSC328 010                            */
/* Professor:           Dr. Lisa Frye                         */
/* Assignment:          Download server project               */
/* Filename:            message.cpp                           */
/* Purpose:             Phase 2 for download server           */
/* Language:            c++                                   */
/* Partner:             Brett Biever & Tue Doan               */
/**************************************************************************************************/

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "message.h"
#include "cerrors.h"

void sendMessage(int sockfd, void *buffer) {
    char message[512];
    memcpy(message, buffer, 512);
    int rv; // return value
    
    int sendSize = sizeof(message);
    bool crv = true; // client/server return value
    
    // send the size of the message to client/server
    // resend the size until the client/server received the correct size
    do {
        rv = write(sockfd, &sendSize, 4);   //
        checkError(rv, "write");
        
        rv = read(sockfd, &crv, 1);
        checkError(rv, "read");
        
    } while(crv);
    
    crv = true;
    
    // send the content of the message to client/server
    // resend the content until the client/server received the full message
    do {
        write(sockfd, &message, sendSize);
        checkError(rv, "write");
      
        read(sockfd, &crv, 1);
        checkError(rv, "read");
        
    } while (crv);
}

void getMessage(int sockfd, void* buffer) {
    int size, byteRecv; // size of message & return number of byte received
    bool crv = true; // client/server return value
    int rv;
    
    // read the number of byte sent by the client/server
    // and reread if they are not equal
    // on success, send true to the client/server
    do {
        byteRecv = read(sockfd, &size, 4);
        checkError(byteRecv, "write");
        
        if (byteRecv == 4) {
            crv = false;
        } else {
            crv = true;
        }
        
        rv = write(sockfd, &crv, 1);
        checkError(rv, "write");
    } while (crv);
    
    crv = true;
    
    // read the number of byte sent by the client
    // and reread if they are not equal
    // on success, send true to the client
    do {
        byteRecv = read(sockfd, buffer, size);
        checkError(byteRecv, "read");
        
        if (byteRecv == size) {
            crv = false;
        } else {
            crv = true;
        }
        
        rv = write(sockfd, &crv, 1);
        checkError(rv, "write");    // check for error
        
    } while (crv);
}

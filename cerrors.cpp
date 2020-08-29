/**************************************************************************************************/
/* Author:              Logan Atiyeh                          */
/* Major:               Information Technology                */
/* Creation Date:       November 16, 2019                     */
/* Due Date:            November 30, 2019                     */
/* Course:              CSC328 010                            */
/* Professor:           Dr. Lisa Frye                         */
/* Assignment:          Download server project               */
/* Filename:            cerrors.h                             */
/* Purpose:             Phase 2 for download server           */
/* Language:            c++                                   */
/* Partner:             Brett Biever & Tue Doan               */
/**************************************************************************************************/

#include <stdio.h>
#include <strings.h>
#include <string>
#include "cerrors.h"

using namespace std;

void checkClientUsage(char * argv) {
    printf("\nPlease enter the command line arguments as follow\n");
    printf("Usage: %s  <hostname> <port number (optional) > 1028 and <65535 >\n\n", argv);
}

void checkServerUsage(char * argv) {
    printf("\nPlease enter the command line arguments as follow\n");
    printf("Usage: %s <port number (optional) > 1028 and <65535 >\n\n", argv);
}


void checkError(int rv, string callType) {
    if (callType == "write") {
        if (rv == -1) {
            perror("Error in writing: ");
        }
    } else if (callType == "read") {
        if (rv == -1) {
            perror("Error in reading: ");
        }
    } else if (callType == "socket") {
        if (rv == -1) {
            perror("Error in creating a socket: ");
            exit(1);
        }
    } else if (callType == "open") {
        if (rv == -1) {
        perror("Error in open the file: ");
        }
    }
}

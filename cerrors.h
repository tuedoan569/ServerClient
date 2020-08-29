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

#include <string>

using namespace std;

/***********************************************************************************************************/
/* Function name:   checkClientUsage                                */
/* Description:     check if the client has the correct usage       */
/* Parameters:      char *argv: array of char of the program name   */
/* Return Value:    none                                            */
/***********************************************************************************************************/
void checkClientUsage(char *argv);

/***********************************************************************************************************/
/* Function name:   checkServerUsage                                */
/* Description:     check if the server has the correct usage       */
/* Parameters:      char *argv: array of char of the program name   */
/* Return Value:    none                                            */
/***********************************************************************************************************/
void checkServerUsage(char *argv);

/***********************************************************************************************************/
/* Function name:   checkError                                      */
/* Description:     check error for the system calls                */
/* Parameters:      int rv: return value                            */
/*                  string callType: the type of system call        */
/* Return Value:    none                                            */
/***********************************************************************************************************/
void checkError(int rv, string callType);

/**************************************************************************************************/
/* Author:              Logan Atiyeh                          */
/* Major:               Information Technology                */
/* Creation Date:       November 16, 2019                     */
/* Due Date:            November 30, 2019                     */
/* Course:              CSC328 010                            */
/* Professor:           Dr. Lisa Frye                         */
/* Assignment:          Download server project               */
/* Filename:            message.h                             */
/* Purpose:             Phase 2 for download server           */
/* Language:            c++                                   */
/* Partner:             Brett Biever & Tue Doan               */
/**************************************************************************************************/

/***********************************************************************************************************/
/* Function name:   sendMessage                                     */
/* Description:     Send a message                                  */
/* Parameters:      int csockfd: file descriptor for active socket  */
/*                  void* buffer: content of the message            */
/* Return Value:    none                                            */
/***********************************************************************************************************/
void sendMessage(int sockfd, void *buffer);

/***********************************************************************************************************/
/* Function name:   getMessage                                      */
/* Description:     Get a message                                   */
/* Parameters:      int csockfd: file descriptor for active socket  */
/*                  void* buffer: content of the message            */
/* Return Value:    none                                            */
/***********************************************************************************************************/
void getMessage(int sockfd, void *buffer);



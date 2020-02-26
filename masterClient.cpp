/* TCP-based client example of socket programming.    */
/* This client interacts with the word length server, */
/* which needs to be running first.                   */
/*                                                    */
/* Usage: cc -o wordlen-TCPclient wordlen-TCPclient.c */
/*        ./wordlen-TCPclient                         */
/*                                                    */
/* Written by Carey Williamson       January 13, 2012 */

/* Include files for C socket programming and stuff */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>
#include <iostream>
#include <netinet/in.h>
#include <arpa/inet.h>

/* Some generic error handling stuff */
extern int errno;
void perror(const char *s);

/* Manifest constants used by client program */
#define MAX_HOSTNAME_LENGTH 64
#define MAX_WORD_LENGTH 100
#define BYNAME 0
#define MYPORTNUM 12345 /* must match the server's port! */

/* Menu selections */
#define ALLDONE 0
#define ECHO 1
#define REVERSE 2
#define UPPER 3
#define LOWER 4
#define CAESER 5
#define TBD 6

using namespace std;

/* Prompt the user to enter a word */
void printmenu()
{
    cout << "\nPlease enter a message to be sent to the server ('0' to terminate): " << endl;
    cout << "   You can use the micro services, by selecting 1 or more of the following, or exit to quit" << endl;
    cout << "       1. Echo service" << endl;
    cout << "       2. Reverse service" << endl;
    cout << "       3. upper service" << endl;
    cout << "       4. lower service" << endl;
    cout << "       5. caeser cipher service" << endl;
    cout << "       6. TBD service" << endl;
}

/* Main program of client */
int main()
{
    int sockfd, sockfd2;
    char c;
    struct sockaddr_in server;
    struct hostent *hp;
    char hostname[MAX_HOSTNAME_LENGTH];
    char message[MAX_WORD_LENGTH];
    char messageback[MAX_WORD_LENGTH];
    int choice, len, bytes;

    /* Initialization of server sockaddr data structure */
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(MYPORTNUM);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");

    /* create the client socket for its transport-level end point */
    if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) == -1)
    {
        cout << "socket() call failed!" << endl;
        exit(1);
    }

    /* connect the socket to the server's address using TCP */
    if (connect(sockfd, (struct sockaddr *)&server, sizeof(struct sockaddr_in)) == -1)
    {
        cout << "connect() call failed!" << endl;
        perror(NULL);
        exit(1);
    }

    /* Print welcome banner */
    cout << "Welcome! I am the master server !!" << endl;
    printmenu();
    scanf("%d", &choice);

    /* main loop: read a word, send to server, and print answer received */
    while (choice != ALLDONE)
    {
        if (choice == UPPER)
        {
            /* get rid of newline after the (integer) menu choice given */
            c = getchar();

            /* prompt user for the input */
            printf("Enter your word: ");
            len = 2;
            //cout << "choice: " << endl;
            message[0] = '$3'; // encode microservice info
            message[1] = '#';
            while ((c = getchar()) != '\n')
            {
                message[len] = c;
                len++;
            }
            /* make sure the message is null-terminated in C */
            message[len] = '\0';

            /* send it to the server via the socket */
            send(sockfd, message, len, 0);

            /* see what the server sends back */
            if ((bytes = recv(sockfd, messageback, len, 0)) > 0)
            {
                /* make sure the message is null-terminated in C */
                messageback[bytes] = '\0';
                cout << "Answer received from server: " << messageback << endl;
            }
            else
            {
                /* an error condition if the server dies unexpectedly */
                printf("Sorry, dude. Server failed!\n");
                close(sockfd);
                exit(1);
            }
        }
        else
            printf("Invalid menu selection. Please try again.\n");

        printmenu();
        scanf("%d", &choice);
    }

    /* Program all done, so clean up and exit the client */
    close(sockfd);
    exit(0);
}
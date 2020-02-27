//TCP-based client
/* This client interacts with the master server,      */
/* which needs to be running first.                   */
/*                                                    */
/* Usage: cc -o masterClient masterClient.cpp */
/*        ./masterClient                              */

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
#define MAX_WORD_LENGTH 200000
#define BYNAME 0
#define VALID_ENTRY 1
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
    cout << "       3. Upper case service" << endl;
    cout << "       4. Lower case service" << endl;
    cout << "       5. Caeser cipher service" << endl;
    cout << "       6. TBD service" << endl;
}

/* Main program of client */
int main()
{
    int sockfd, sockfd2;
    char c;
    struct sockaddr_in server;
    char message[MAX_WORD_LENGTH];
    char messageback[MAX_WORD_LENGTH];
    int choice, initialChoice, len, bytes;
    string command;

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
        initialChoice = choice;
        len = 0;

        if (choice == ECHO)
        {
            /* get rid of newline after the (integer) menu choice given */
            c = getchar();

            /* prompt user for the input */
            cout << "Enter your word: " << endl;

            // encode microservice info
            command = to_string(initialChoice) + "#";
        }
        else if (choice == REVERSE)
        {
            c = getchar();
            cout << "Enter your word: " << endl;
            command = to_string(initialChoice) + "#";
        }
        else if (choice == UPPER)
        {
            c = getchar();
            cout << "Enter your word: " << endl;
            command = to_string(initialChoice) + "#";
        }
        else if (choice == LOWER)
        {
            c = getchar();
            cout << "Enter your word: " << endl;
            command = to_string(initialChoice) + "#";
        }
        else if (choice == CAESER)
        {
            c = getchar();
            cout << "Enter your word: " << endl;
            command = to_string(initialChoice) + "#";
        }
        else
        {
            c = getchar();
            cout << "Enter your word: " << endl;
            command = to_string(initialChoice) + "#";
        }

        while ((c = getchar()) != '\n')
        {
            message[len] = c;
            len++;
        }
        /* make sure the message is null-terminated in C */
        message[len] = '\0';

        string stringMessage = string(message);

        command += stringMessage;

        //cout << "sending " << command << " to server" << endl;

        /* send it to the server via the socket */
        send(sockfd, command.data(), command.length(), 0);

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
            cout << "Sorry, dude. Server failed! " << endl;
            close(sockfd);
            exit(1);
        }

        printmenu();
        scanf("%d", &choice);
    }

    /* Program all done, so clean up and exit the client */
    close(sockfd);
    exit(0);
}
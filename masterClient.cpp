//TCP-based client
/* This client interacts with the master server,      */
/* which needs to be running first.                   */
/*                                                    */
/* Usage: cc -o masterclient masterclient.cpp */
/*        ./masterclient                              */

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
#define MYPORTNUM 12346 /* must match the server's port! */

/* Menu selections */
#define ALLDONE 0
#define ECHO 1
#define REVERSE 2
#define UPPER 3
#define LOWER 4
#define CAESER 5
#define HASH 6

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
    cout << "       6. HASH service" << endl;
}

void errorHandling(int &tmp)
{
    if (cin.fail())
    {
        while (1)
        {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Enter in a valid option, choose from below: " << endl;
            printmenu();
            cin >> tmp;

            if (!cin.fail())
            {
                break;
            }
        }
    }
}

/* Main program of client */
int main()
{
    int sockfd, sockfd2;
    char c;
    struct sockaddr_in server;
    char message[MAX_WORD_LENGTH];
    char messageback[MAX_WORD_LENGTH];
    int choice, reuse, initialChoice, len, bytes;
    string command, serverResponse;

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
    cin >> choice;
    errorHandling(choice); // event user enters in an incorrect value

    /* main loop: read a word, send to server, and print answer received */
    while (choice != ALLDONE)
    {
        bzero(messageback, MAX_WORD_LENGTH);
        bzero(message, MAX_WORD_LENGTH);
        initialChoice = choice;
        len = 0;

        if (reuse == 1)
        {
            command = to_string(initialChoice) + "#" + serverResponse;
            reuse = 0;
        }
        else
        {
            /* get rid of newline after the (integer) menu choice given */
            c = getchar();

            /* prompt user for the input */
            cout << "Enter your word: " << endl;

            // encode microservice info
            command = to_string(initialChoice) + "#";

            while ((c = getchar()) != '\n')
            {
                message[len] = c;
                len++;
            }
            /* make sure the message is null-terminated in C */
            message[len] = '\0';

            // append command to message
            string stringMessage = string(message);
            command += stringMessage;
        }

        if (send(sockfd, command.data(), command.length(), 0) < 0)
        {
            perror("send failed");
            exit(-1);
        }

        /* see what the server sends back */
        if ((bytes = recv(sockfd, messageback, command.length(), 0)) != -1)
        {
            cout << "Answer received from server: " << messageback << endl;
            serverResponse = string(messageback);
        }
        else
        {
            /* an error condition if the server dies unexpectedly */
            perror("receive error");
            cout << "Sorry, dude. Server failed! " << endl;
            close(sockfd);
            exit(1);
        }

        printmenu();
        cin >> choice;
        cout << "\n";

        errorHandling(choice);

        // user wants to leave
        if (choice == ALLDONE)
        {
            break;
        }

        // reuse previous word
        cout << "   Press 1 to continue modifying word" << endl;
        cout << "   Press 0 to enter new word" << endl;
        cin >> reuse;
        cout << "\n";

        // reset buffers
        bzero(messageback, MAX_WORD_LENGTH);
        bzero(message, MAX_WORD_LENGTH);
    }

    /* Program all done, so clean up and exit the client */
    close(sockfd);
    exit(0);
}
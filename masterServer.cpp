/*
 * A simple TCP server that echos messages back to the client.
 * This server works with only a single client.  With a simple modification, it can take more clients.
* This example is adopted from CPSC 441 tutorials thought by Prof. Mea Wang 
 */

#include <iostream>
#include <sys/socket.h> // for socket(), connect(), send(), and recv()
#include <arpa/inet.h>  // for sockaddr_in and inet_addr()
#include <stdlib.h>     // for atoi() and exit()
#include <string.h>     // for memset()
#include <unistd.h>     // for close()
#include <signal.h>

#define MYPORTNUM 12345
#define MAX_MESSAGE_LENGTH 100
#define MAX_BUFFER_SIZE 40
#define UPPER 9090

// Configure IP address
#define SERVER_IP "127.0.0.1"

using namespace std;

const int BUFFERSIZE = 32; // Size the message buffers
const int MAXPENDING = 1;  // Maximum pending connections

/* Global variable */
int childsockfd;

/* This is a signal handler to do graceful exit if needed */
void catcher(int sig)
{
    close(childsockfd);
    exit(0);
}

string microServiceSock(int port, string messageIn)
{
    struct sockaddr_in si_server;
    struct sockaddr *server;
    int s, i = sizeof(si_server);
    socklen_t len = sizeof(si_server);
    char buf[MAX_BUFFER_SIZE];
    int readBytes;

    if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        printf("Could not set up a socket!\n");
        exit(-1);
    }

    memset((char *)&si_server, 0, sizeof(si_server));
    si_server.sin_family = AF_INET;
    si_server.sin_port = htons(port);
    server = (struct sockaddr *)&si_server;

    if (inet_pton(AF_INET, SERVER_IP, &si_server.sin_addr) == 0)
    {
        printf("inet_pton() failed\n");
        exit(-1);
    }

    bzero(buf, MAX_BUFFER_SIZE);
    strcpy(buf, messageIn.c_str());

    if (sendto(s, buf, strlen(buf), 0, server, sizeof(si_server)) == -1)
    {
        printf("sendto failed\n");
        exit(-1);
    }

    if ((readBytes = recvfrom(s, buf, MAX_BUFFER_SIZE, 0, server, &len)) == -1)
    {
        printf("Read error!\n");
        exit(-1);
    }
    buf[readBytes] = '\0'; // proper null-termination of string

    string tmp = string(buf);

    close(s);

    return tmp;
}

/* Main program for server */
int main()
{
    struct sockaddr_in server;
    static struct sigaction act;
    char messagein[MAX_MESSAGE_LENGTH];
    char messageout[MAX_MESSAGE_LENGTH];
    int parentsockfd;
    int i, j;
    int pid;
    char c;

    /* Set up a signal handler to catch some weird termination conditions. */
    act.sa_handler = catcher;
    sigfillset(&(act.sa_mask));
    sigaction(SIGPIPE, &act, NULL);

    /* Initialize server sockaddr structure */
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(MYPORTNUM);
    server.sin_addr.s_addr = htonl(INADDR_ANY);

    /* set up the transport-level end point to use TCP */
    if ((parentsockfd = socket(PF_INET, SOCK_STREAM, 0)) == -1)
    {
        cout << "socket() call failed!" << endl;
        exit(1);
    }

    /* bind a specific address and port to the end point */
    if (bind(parentsockfd, (struct sockaddr *)&server, sizeof(struct sockaddr_in)) == -1)
    {
        cout << "bind() call failed!" << endl;
        exit(1);
    }

    /* start listening for incoming connections from clients */
    if (listen(parentsockfd, 5) == -1)
    {
        cout << "isten() call failed!" << endl;
        exit(1);
    }

    /* initialize message strings just to be safe (null-terminated) */
    bzero(messagein, MAX_MESSAGE_LENGTH);
    bzero(messageout, MAX_MESSAGE_LENGTH);

    cout << "Welcome! I am the master server!! " << endl;
    cout << "server listening on TCP port " << MYPORTNUM << endl;

    /* Main loop: server loops forever listening for requests */
    for (;;)
    {
        /* accept a connection */
        if ((childsockfd = accept(parentsockfd, NULL, NULL)) == -1)
        {
            cout << "accept() call failed! " << endl;
            exit(1);
        }

        /* try to create a child process to deal with this new client */
        pid = fork();

        /* use process id (pid) returned by fork to decide what to do next */
        if (pid < 0)
        {
            cout << "fork() call failed! " << endl;
            exit(1);
        }
        else if (pid == 0)
        {
            /* the child process is the one doing the "then" part */

            /* don't need the parent listener socket that was inherited */
            close(parentsockfd);

            /* obtain the message from this client */
            while (recv(childsockfd, messagein, MAX_MESSAGE_LENGTH, 0) > 0)
            {
                /* print out the received message */
                cout << "Child process received word: " << messagein << endl;

#ifdef DEBUG
                printf("Child about to send message: %s\n", messageout);
#endif
                string tmpMessageIn = string(messagein);
                cout << "tmpMessage: " << tmpMessageIn << endl;
                //int startPos = tmpMessageIn.find("$");
                int endPos = tmpMessageIn.find("#");
                string command = tmpMessageIn.substr(0, endPos);
                string message = tmpMessageIn.substr(endPos + 1);
                string returntoClient;
                cout << "command: " << command << endl;

                for (int i = 0; i < command.length(); i++)
                {
                    if (command[i] == '3')
                    {
                        cout << "here" << endl;
                        returntoClient = microServiceSock(UPPER, message);
                    }
                }

                cout << "sending " << returntoClient.data() << " to TCP client" << endl;
                /* send the result message back to the client */
                if (send(childsockfd, returntoClient.data(), returntoClient.length(), 0) < 0)
                {
                    perror("send to TCP Client failed");
                }

                /* clear out message strings again to be safe */
                bzero(messagein, MAX_MESSAGE_LENGTH);
                bzero(messageout, MAX_MESSAGE_LENGTH);
            }

            /* when client is no longer sending information to us, */
            /* the socket can be closed and the child process terminated */
            close(childsockfd);
            exit(0);
        } /* end of then part for child */
        else
        {
            /* the parent process is the one doing the "else" part */
            // fprintf(stderr, "Created child process %d to handle that client\n", pid);
            // fprintf(stderr, "Parent going back to job of listening...\n\n");

            /* parent doesn't need the childsockfd */
            close(childsockfd);
        }
    }
}
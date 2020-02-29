/*
 * A Master TCP server that receives a message from its client.
 * Uses a UDP socket to communicate with the micro services .
 * 
 * Usage: g++ -o masterserver masterserver.cpp
 *        ./masterServer 
 */

#include <iostream>
#include <sys/socket.h> // for socket(), connect(), send(), and recv()
#include <arpa/inet.h>  // for sockaddr_in and inet_addr()
#include <stdlib.h>     // for atoi() and exit()
#include <string.h>     // for memset()
#include <unistd.h>     // for close()
#include <signal.h>

#define MYPORTNUM 12346
#define MAX_MESSAGE_LENGTH 200000
#define ECHO 8990
#define REVERSE 8991
#define UPPER 9090
#define LOWER 9091
#define CAESER 9092
#define HASH 9093

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

// UDP Client for micro services
string microServiceSock(int port, string messageIn)
{
    struct sockaddr_in si_server;
    struct sockaddr *server;
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 100000; // timer to 100ms
    int s, i = sizeof(si_server);
    socklen_t len = sizeof(si_server);
    char buf[MAX_MESSAGE_LENGTH];
    int readBytes;

    if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        cout << "Could not set up a socket!" << endl;
        exit(-1);
    }

    memset((char *)&si_server, 0, sizeof(si_server));
    si_server.sin_family = AF_INET;
    si_server.sin_port = htons(port);
    server = (struct sockaddr *)&si_server;

    if (inet_pton(AF_INET, SERVER_IP, &si_server.sin_addr) == 0)
    {
        cout << "inet_pton() failed " << endl;
        exit(-1);
    }

    bzero(buf, MAX_MESSAGE_LENGTH);
    strcpy(buf, messageIn.c_str());

    if (sendto(s, buf, strlen(buf), 0, server, sizeof(si_server)) == -1)
    {
        cout << "sendto failed" << endl;
        exit(-1);
    }

    // if the udp server doesn't respond in time
    if (setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0)
    {
        perror("Error");
    }

    if ((readBytes = recvfrom(s, buf, MAX_MESSAGE_LENGTH, 0, server, &len)) == -1)
    {
        cout << "Read error! " << endl;
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
        //cout << "bind() call failed!" << endl;
        perror("bind failed");
        exit(1);
    }

    /* start listening for incoming connections from clients */
    if (listen(parentsockfd, 5) == -1)
    {
        cout << "isten() call failed!" << endl;
        exit(1);
    }

    cout << "Welcome! I am the master server!! " << endl;
    cout << "server listening on TCP port " << MYPORTNUM << endl;

    /* Main loop: server loops forever listening for requests */
    for (;;)
    {
        /* initialize message strings just to be safe (null-terminated) */
        bzero(messagein, MAX_MESSAGE_LENGTH);
        bzero(messageout, MAX_MESSAGE_LENGTH);

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

            /* don't need the parent listener socket that was inherited */
            close(parentsockfd);

            /* obtain the message from this client */
            while (recv(childsockfd, messagein, MAX_MESSAGE_LENGTH, 0) > 0)
            {
                /* print out the received message */
                cout << "Child process received word: " << messagein << endl;

                // convert received message to string, for easier manipulation
                string tmpMessageIn = string(messagein);

                int endPos = tmpMessageIn.find("#");
                string command = tmpMessageIn.substr(0, endPos);
                string message = tmpMessageIn.substr(endPos + 1);
                string returntoClient;

                // process menu options from client
                for (int i = 0; i < command.length(); i++)
                {
                    if (command[i] == '1')
                    {
                        if (i > 0)
                        {
                            returntoClient = microServiceSock(ECHO, returntoClient);
                        }
                        else
                        {
                            returntoClient = microServiceSock(ECHO, message);
                        }
                    }
                    else if (command[i] == '2')
                    {
                        if (i > 0)
                        {
                            returntoClient = microServiceSock(REVERSE, returntoClient);
                        }
                        else
                        {
                            returntoClient = microServiceSock(REVERSE, message);
                        }
                    }
                    else if (command[i] == '3')
                    {
                        if (i > 0)
                        {
                            returntoClient = microServiceSock(UPPER, returntoClient);
                        }
                        else
                        {
                            returntoClient = microServiceSock(UPPER, message);
                        }
                    }
                    else if (command[i] == '4')
                    {
                        if (i > 0)
                        {
                            returntoClient = microServiceSock(LOWER, returntoClient);
                        }
                        else
                        {
                            returntoClient = microServiceSock(LOWER, message);
                        }
                    }
                    else if (command[i] == '5')
                    {
                        if (i > 0)
                        {
                            returntoClient = microServiceSock(CAESER, returntoClient);
                        }
                        else
                        {
                            returntoClient = microServiceSock(CAESER, message);
                        }
                    }
                    else if (command[i] == '6')
                    {
                        if (i > 0)
                        {
                            returntoClient = microServiceSock(HASH, returntoClient);
                        }
                        else
                        {
                            returntoClient = microServiceSock(HASH, message);
                        }
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

            /* parent doesn't need the childsockfd */
            close(childsockfd);
        }
    }
}
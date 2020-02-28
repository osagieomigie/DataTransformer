/* Simple program to demonstrate a UDP-based server.
 * Loops forever.
 * Receives a word from the client. 
 * Changes the word to lower case.
 * sends response back to client.
 * 
 * Compile using "g++ -o lower lower.cpp"
 */

/* Include files */
#include <stdio.h>
#include <iostream>
#include <unistd.h>
#include <string.h>
#include <cctype>
#include <stack>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

/* Manifest constants */
#define MAX_BUFFER_SIZE 200000
#define PORT 9091

/* Verbose debugging */
#define DEBUG 1

using namespace std;

string lowerArr(string tmp)
{
    int end = tmp.length();
    for (int i = 0; i < end; i++)
    {
        tmp[i] = tolower(tmp[i]);
    }
    return tmp;
}

/* Main program */
int main()
{
    struct sockaddr_in si_server, si_client;
    struct sockaddr *server, *client;
    int s, i = sizeof(si_server);
    socklen_t len = sizeof(si_server);
    char messagein[MAX_BUFFER_SIZE];
    char messageout[MAX_BUFFER_SIZE];
    int readBytes;

    if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        cout << "Could not setup a socket!" << endl;
        return 1;
    }

    memset((char *)&si_server, 0, sizeof(si_server));
    si_server.sin_family = AF_INET;
    si_server.sin_port = htons(PORT);
    si_server.sin_addr.s_addr = htonl(INADDR_ANY);
    server = (struct sockaddr *)&si_server;
    client = (struct sockaddr *)&si_client;

    if (bind(s, server, sizeof(si_server)) == -1)
    {
        cout << "Could not bind to port " << PORT << endl;
        return 1;
    }
    cout << stderr << "Welcome! I am the lower case server!!" << endl;
    cout << "server now listening on UDP port " << PORT << "..." << endl;

    /* big loop, looking for incoming messages from clients */
    for (;;)
    {
        /* clear out message buffers to be safe */
        bzero(messagein, MAX_BUFFER_SIZE);
        bzero(messageout, MAX_BUFFER_SIZE);

        /* see what comes in from a client, if anything */
        if ((readBytes = recvfrom(s, messagein, MAX_BUFFER_SIZE, 0, client, &len)) < 0)
        {
            cout << "Read error!" << endl;
            return -1;
        }
#ifdef DEBUG
        else
            cout << "Server received " << readBytes << " bytes" << endl;
#endif
        cout << "  server received " << messagein << " from IP " << inet_ntoa(si_client.sin_addr) << "port " << ntohs(si_client.sin_port) << endl;

        // reverse messagein buffer
        string tmp = string(messagein);
        string tmpResult = lowerArr(tmp);
        strcpy(messageout, tmpResult.c_str());
        messageout[readBytes] = '\0';

#ifdef DEBUG
        cout << "Server sending back the message: " << messageout << endl;
#endif

        /* send the message back to the client */
        sendto(s, messageout, readBytes, 0, client, len);
    }

    close(s);
    return 0;
}
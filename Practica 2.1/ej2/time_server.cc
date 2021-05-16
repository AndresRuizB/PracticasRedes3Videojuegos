#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <iostream>
#include <time.h>

int main(int argc, char** argv){

    struct addrinfo hints;
    struct addrinfo * res;
    bool stop = false;

    memset((void *) &hints, 0, sizeof(struct addrinfo));

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;

    int rc = getaddrinfo(argv[1], argv[2], &hints, &res);

    if(rc != 0){
        std::cerr << "[getaddrinfo]: " << gai_strerror(rc) << std::endl;
        return -1;
    }

    int sd = socket(res->ai_family, res->ai_socktype, 0);

    if(sd == -1){

        std::cerr << "[socket]: fallo al abrir el socket\n";
        return -1;
    }

    bind(sd,res->ai_addr, res->ai_addrlen);

    freeaddrinfo(res);

    while(!stop){

        char buffer[80];

        char host[NI_MAXHOST];
        char serv[NI_MAXSERV];

        struct sockaddr cliente;
        socklen_t clientelen = sizeof(struct sockaddr);

        int bytes = recvfrom(sd, (void *)buffer, 80, 0, &cliente, &clientelen);

        int bSent;

        if (bytes == 2){
            getnameinfo(&cliente, clientelen, host, NI_MAXHOST, serv, 
                    NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV);

            std::cout << bytes<< "Bytes recibidos de " << host << " : "<< serv<< std::endl;

            time_t calendarTime;
            struct tm *absoluteTime;
            int len;

            time(&calendarTime);
            absoluteTime = localtime(&calendarTime);

            switch(buffer[0]){

                case 't':
                    len = strftime(buffer, 80, "%r",absoluteTime);
                    bSent = sendto(sd, buffer, len, 0, &cliente, clientelen);
                    break;
                case 'd':                    
                    len = strftime(buffer, 80, "%F",absoluteTime);
                    bSent = sendto(sd, buffer, len, 0, &cliente, clientelen);
                    break;
                case 'q':
                    stop = true;
                    break;
            }

        }

        if(bSent == -1){
            std::cerr<<"[sendto] Error al enviar datos al cliente\n";

        }

    }

    close(sd);

    return 0;

}

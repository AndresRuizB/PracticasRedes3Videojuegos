#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <iostream>

int main(int argc, char** argv){

    struct addrinfo hints;
    struct addrinfo * res;
    char buffer[80];

    if(argc != 4){
        std::cerr<< "Incorrect number of arguments\n";
        return -1;
    }

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

    int bSent = sendto(sd, (void*)argv[3], 2, 0, res->ai_addr, res->ai_addrlen);

    if(bSent != 2){
        std::cerr << "[sendto] Error al enviar los datos";

    }

    bSent = recvfrom(sd, (void *)buffer, 80, 0, res->ai_addr, &res->ai_addrlen);

    if(bSent != 1){
        std::cerr << "[recvfrom] Error al recibir los datos";

    }

    std::cout<< buffer<<"\n";

    freeaddrinfo(res);

    close(sd);

    return 0;

}

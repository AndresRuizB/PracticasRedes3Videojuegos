#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <iostream>

int main(int argc, char** argv){

    struct addrinfo hints;
    struct addrinfo * res;

    memset((void *) &hints, 0, sizeof(struct addrinfo));

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

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

    int lst = listen(sd, 16);

    if(lst != 0){
        std::cerr << "[listen] error al intentar activar la escucha";
        return -1;
    }

    struct sockaddr cliente;
    socklen_t clientelen = sizeof(struct sockaddr);

    int socketCliente = accept(sd, &cliente, &clientelen);

    char host[NI_MAXHOST];
    char serv[NI_MAXSERV];

    getnameinfo(&cliente, clientelen, host, NI_MAXHOST, serv, 
                    NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV);

    while(true){

        char buffer[80];

        int bytes = recv(socketCliente, (void *)buffer, 80, 0);

        if (bytes <= 0){
            
            std::cout<< "Se cerro la conexion\n";
        }

        buffer[bytes] = '\0';
        send(socketCliente, buffer, bytes, 0);

    }

    return 0;

}

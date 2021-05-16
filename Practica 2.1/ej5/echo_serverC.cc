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
    bool stop = false;

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

    int conn = connect(sd, (struct sockaddr*)res->ai_addr, res-> ai_addrlen);

    if(conn != -1){

        std::cerr << "[connect] No se pudo conectar con el servidor\n";
        return -1;

    }

    while(!stop){

        std::cin>> buffer;

        if (buffer[1] == '\0' && (buffer[0] == 'Q' || buffer[0] == 'q')){

            stop = true;
        }else{
            int snd = send(sd, buffer, strlen(buffer), 0);

            if(snd == -1){
                std::cerr << "[send]  Error al enciar el mensaje";

            }

            int bytes = recv(sd, buffer, 80,0);
            buffer[bytes] = '\0';
            std::cout<< buffer << "\n";
        }

    }

    freeaddrinfo(res);
    close(sd);

    return 0;

}

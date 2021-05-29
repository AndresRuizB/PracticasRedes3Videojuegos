#include "Socket.h"
#include "Serializable.h"

#include "string.h"

bool operator== (const Socket &s1, const Socket &s2){

    //Comparar el tipo (sin_family), dirección  (sin_addr.s_addr) y puerto (sin_port)

    const struct sockaddr_in* s_in1 = (const struct sockaddr_in * )&(s1.sa);
    const struct sockaddr_in* s_in2 = (const struct sockaddr_in * )&(s2.sa);

    return  (s_in1->sin_family == s_in2->sin_family  &&
            s_in1->sin_addr.s_addr == s_in2->sin_addr.s_addr &&
            s_in1->sin_port == s_in2->sin_port);
}

std::ostream& operator<<(std::ostream& os, const Socket& dt){

    //Imprime la dirección y puerto en número con el formato:"dirección_ip:puerto"

    char host[NI_MAXHOST], serv[NI_MAXSERV];

    getnameinfo((struct sockaddr*)& (dt.sa), dt.sa_len, host, NI_MAXHOST, serv, NI_MAXSERV, NI_NUMERICHOST);
    os << host<< ":"<< serv;

    return os;

}

Socket::Socket(const char * address, const char * port){
    /**
     *  Construye el socket UDP con la dirección y puerto dados. Esta función
     *  usara getaddrinfo para obtener la representación binaria de dirección y
     *  puerto.
     *
     *  Además abrirá el canal de comunicación con la llamada socket(2).
     *
     *    @param address cadena que representa la dirección o nombre
     *    @param port cadena que representa el puerto o nombre del servicio
     */

    struct addrinfo hints;
    struct addrinfo * res;

    memset((void *) &hints, 0, sizeof(struct addrinfo));

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;

    int rc = getaddrinfo(address, port, &hints, &res);

    if(rc != 0){
        std::cerr << "[getaddrinfo]: error: " << gai_strerror(rc) << "\n";
    }

    int sd = socket(res->ai_family, res->ai_socktype, 0);

    if(sd == -1){

        std::cerr << "[socket]: fallo al abrir el socket\n";
    }

    sa = *res ->ai_addr;
    sa_len = res->ai_addrlen;
}

int Socket::recv(Serializable &obj, Socket * &sock){

    struct sockaddr sa;
    socklen_t sa_len = sizeof(struct sockaddr);

    char buffer[MAX_MESSAGE_SIZE];
    ssize_t lm = recvfrom(sd, buffer, MAX_MESSAGE_SIZE, 0, &sa, &sa_len);

    if(lm < 0){
        return -1;        
    }

    if(sock != 0){
        sock = new Socket(&sa, sa_len);
    }

    obj.from_bin(buffer);

    return 0;
}

int Socket::send(Serializable& obj, const Socket& sock){

    obj.to_bin();

    int bytes = sendto(sock.sd, obj.data(), obj.size(), 0, &sock.sa, sock.sa_len);

    return bytes > 0;

}
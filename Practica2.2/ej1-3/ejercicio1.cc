#include "Serializable.h"

#include <iostream>
#include <string>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

class Jugador: public Serializable
{
public:
    Jugador(const char * _n, int16_t _x, int16_t _y):x(_x),y(_y)
    {
        strncpy(name, _n, 80);
    };

    virtual ~Jugador(){};

    void to_bin()
    {
        int32_t data_size = serializedSize();
        alloc_data(data_size);
        
        char *tmp = _data;

        memcpy(tmp, name, NAME_MAX*sizeof(char));
        tmp += NAME_MAX * sizeof(char);

        memcpy(tmp, &x, sizeof(int16_t));
        tmp += sizeof(int16_t);

        memcpy(tmp, &y, sizeof(int16_t));
    }

    int from_bin(char * data)
    {
        char *tmp = data;
        int32_t data_size = serializedSize();
        alloc_data(data_size);

        memcpy (name, tmp, NAME_MAX * sizeof(char));
        tmp += NAME_MAX * sizeof(char);        

        memcpy(&x, tmp, sizeof(int16_t));
        tmp += sizeof(int16_t);

        memcpy(&y, tmp, sizeof(int16_t));

        return 0;
    }


public:

    static const size_t NAME_MAX = 80;

    char name[NAME_MAX];

    int16_t x;
    int16_t y;

    int serializedSize(){

        return NAME_MAX * sizeof(char) + 2 * sizeof(int16_t);

    }

    friend std::ostream & operator << (std::ostream &out, const Jugador &j){

        out << "Name: " << j.name << " ";
        out << "Position: " << j.x << ", " << j.y << "\n";

    }

};

int main(int argc, char **argv)
{
    Jugador one_r("", 0, 0);
    Jugador one_w("Player_ONE", 123, 987);

    // 1. Serializar el objeto one_w

    one_w.to_bin();
    // 2. Escribir la serialización en un fichero

    int fd = open("./datos", O_CREAT | O_TRUNC | O_RDWR, 0666);
    write(fd, one_w.data(), one_w.size());
    close(fd);

    // 3. Leer el fichero

    char *buffer = (char *)malloc(one_r.serializedSize());
    int buffSize = one_r.serializedSize();
    fd = open("./datos", O_RDONLY);

    read(fd, buffer, buffSize);

    // 4. "Deserializar" en one_r

    one_r.from_bin(buffer);

    close(fd);
    free(buffer);

    // 5. Mostrar el contenido de one_r

    std::cout << one_r;

    return 0;
}


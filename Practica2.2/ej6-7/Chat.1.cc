#include "Chat.h"

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void ChatMessage::to_bin()
{
    alloc_data(MESSAGE_SIZE);

    memset(_data, 0, MESSAGE_SIZE);

    //Serializar los campos type, nick y message en el buffer _data
    char* ptr = _data;

    memcpy(ptr, &type, sizeof(uint8_t));
    ptr += sizeof(uint8_t);

    memcpy(ptr, &type, 8 * sizeof(char));
    ptr += 8 * sizeof(char);

    memcpy(ptr, &type, 80 * sizeof(char));
}

int ChatMessage::from_bin(char * bobj)
{
    alloc_data(MESSAGE_SIZE);

    memcpy(static_cast<void *>(_data), bobj, MESSAGE_SIZE);

    //Reconstruir la clase usando el buffer _data
    char* ptr = _data;

    memcpy(&type, ptr, sizeof(uint8_t));
    ptr += sizeof(uint8_t);

    nick = std::string(ptr, ptr + 8 * sizeof(char));
    ptr += 8 * sizeof(char);

    message = std::string(ptr, ptr + 80 * sizeof(char));

    return 0;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void ChatServer::do_messages()
{
    while (true)
    {
        /*
         * NOTA: los clientes están definidos con "smart pointers", es necesario
         * crear un unique_ptr con el objeto socket recibido y usar std::move
         * para añadirlo al vector
         */

        ChatMessage cm;

        Socket *client = nullptr;
        socket.recv(cm, client);

        std::unique_ptr<Socket> clientSocket(client);

        //Recibir Mensajes en y en función del tipo de mensaje
        // - LOGIN: Añadir al vector clients
        // - LOGOUT: Eliminar del vector clients
        // - MESSAGE: Reenviar el mensaje a todos los clientes (menos el emisor)
    
        switch (cm.type)
        {
            case ChatMessage::LOGIN :
                std::cout << "[LOGIN]: " << cm.nick << "\n";
                clients.push_back(std::move(clientSocket));
                break;

            case ChatMessage::LOGOUT :
                for(auto it = clients.begin(); it != clients.end(); ++it){
                    
                    if(**it == *clientSocket){
                        it = clients.erase(it);
                        std::cout<< "[LOGOUT]: "<< cm.nick << "\n";
                        break;
                    }
                }
                break;

            case ChatMessage::MESSAGE : 
                for(auto it = clients.begin; it != clients.end(); it++){

                    if(! **it == *clientSocket){    //al que lo envió no se lo reenvias
                        socket.send(cm , **it);
                    }
                }
                break;

            default:
                break;
        } 
    
    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void ChatClient::login()
{
    std::string msg;

    ChatMessage cm(nick, msg);
    cm.type = ChatMessage::LOGIN;

    socket.send(cm, socket);
}

void ChatClient::logout()
{
    std::string msg;

    ChatMessage cm(nick, msg);
    cm.type = ChatMessage::LOGIN;

    socket.send(cm, socket);
}

void ChatClient::input_thread()
{
    while (true)
    {
        // Leer stdin con std::getline
        // Enviar al servidor usando socket

        std::string line;
        std::getline(std::cin, line);

        ChatMessage cm(nick, line);
        cm.type = ChatMessage::MESSAGE;

        socket.send(cm, socket);
    }
}

void ChatClient::net_thread()
{
    while(true)
    {
        //Recibir Mensajes de red
        //Mostrar en pantalla el mensaje de la forma "nick: mensaje"
        ChatMessage cm;
        
        socket.recv(cm);
    }
}


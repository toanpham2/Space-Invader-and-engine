//
//  Weather update server in C++
//  Binds PUB socket to tcp://*:5556
//  Publishes random weather updates
//
#include <zmq.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <chrono>
#include <thread>
#include <iostream>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <cmath>
#include <chrono>
#include <mutex>
#include <condition_variable>
#include "Platforms.h"
#include <thread>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#if (defined (WIN32))
#include <zhelpers.hpp>
#endif

//vector to store playable characters
std::vector <playableRec*> playbleCharactersServerside;

//Function to call sftime
int64_t sfTimeServer(){
    using namespace std::chrono;
    return duration_cast<seconds>(system_clock::now().time_since_epoch()).count();
}

//to be threaded
void timerServerSide(){
    zmq::context_t context1 (1);
    //publisher to puyblish new characters
    zmq::socket_t publisher2 (context1, zmq::socket_type::pub);
    publisher2.bind("tcp://*:5562");
    //socket for recieving movement of character
    zmq::socket_t socket3 (context1, zmq::socket_type::rep);
    socket3.bind("tcp://*:5563");
    
    while(true){
        //if recieve message from client dontwait
        zmq::message_t afkUpdate;
        if (socket3.recv(afkUpdate, zmq::recv_flags::dontwait)) {
            //make message into std::string format
            std::string receivedCharacter(static_cast<char*>(afkUpdate.data()), afkUpdate.size());
            std::stringstream characterStream(receivedCharacter);
            std::string id;
            characterStream >> id;
            //recived time of character with that ID = real time
            for (auto& character : playbleCharactersServerside) {
                if(character->getID() == id){
                    character->setRecievedTime(sfTimeServer());
                    
                }
                        
            } 
            //socket3 reply here
            socket3.send(zmq::message_t(), zmq::send_flags::none);
        }
        //for every clients in list 
        for (auto& character : playbleCharactersServerside) {
            //timeGap = getreal - recieved time of said client
            std::cout << "recieved time and ID " + std::to_string(character->getRecievedTime()) + character->getID()  << std::endl;
            std::cout << "curr time  " + std::to_string(sfTimeServer())  << std::endl;
            int64_t timeGap = sfTimeServer() - character->getRecievedTime();
            //if timeGap > 5
            if(timeGap > 5){
                //publish message to disconnect this client with their ID
                std::stringstream characterData;
                characterData << character->getID();
                std::string serializedCharacter = characterData.str();
                // send back the afk character to all clients 
                zmq::message_t characterMessage(serializedCharacter.c_str(), serializedCharacter.size());
                publisher2.send(characterMessage, zmq::send_flags::none);
            }        
        }   
        
              
            
                

        
    }
}

int main () {


   // Create a game timeline
    Timeline gameTimeServer(nullptr, 1);
    




    //vector to store moving platforms
    std::vector <movingPlatform*> movingPlatformsServerside;
    
    // //make event manager
    // EventManager eventManagerServer(&gameTimeServer);
    // //make event handler
    // EventHandler eventHandlerServer();
    // eventManagerServer.registerHandler("EventNewSpawn", eventHandlerServer);
    

    //pup- sub to publish new positions
    zmq::context_t context (1);
    zmq::socket_t publisher (context, zmq::socket_type::pub);
    publisher.bind("tcp://*:5557");


    zmq::socket_t socket (context, zmq::socket_type::rep);
    socket.bind("tcp://*:5558");

    //socket for recieving new characters
    zmq::socket_t socket1 (context, zmq::socket_type::rep);
    socket1.bind("tcp://*:5559");

    //socket for recieving movement of character
    zmq::socket_t socket2 (context, zmq::socket_type::rep);
    socket2.bind("tcp://*:5560");

   //publisher to puyblish new characters
    zmq::socket_t publisher1 (context, zmq::socket_type::pub);
    publisher1.bind("tcp://*:5561");

    int clients = 0;


    int i = 0; 

    //THREAD FOR AFK
    //std::thread disconThread(timerServerSide);

    while (true) {

       
        // Sending back client ID and updating the list
        zmq::message_t request;
        socket.recv(request, zmq::recv_flags::dontwait);
        int j = 0;
        if (request.to_string() == "Hello") {
            clients++;
            std::string message = "ClientID" + std::to_string(clients);
            zmq::message_t reply(message.size());
            memcpy(reply.data(), message.c_str(), message.size());
            socket.send(reply, zmq::send_flags::none);

            // If receive a new playable rectangle
            zmq::message_t characterRequest;
            if (socket1.recv(characterRequest, zmq::recv_flags::none)) {
                socket1.send(zmq::message_t(), zmq::send_flags::none);
                std::string receivedCharacter(static_cast<char*>(characterRequest.data()), characterRequest.size());
                std::cout << "RECIEVED ON SERVER SIDE" + receivedCharacter  << std::endl;
                // Deserialize the received message into a playable character object
                std::stringstream characterStream(receivedCharacter);
                float posX, posY, sizeX, sizeY;
                std::string id;
                characterStream >> posX >> posY >> sizeX >> sizeY >> id;

                // Create a new playable character object from the received data
                
                
                // Loop through the existing characters and add a new character if the ID doesn't match
                bool isNewCharacter = true;
                for (const auto& existingCharacter : playbleCharactersServerside) {
                    if (existingCharacter->getID() == id) {
                        isNewCharacter = false;
                        break;
                    }
                }
                
                if (isNewCharacter) {

                    sf::Vector2f position(posX, posY);
                    sf::Vector2f size(sizeX, sizeY);
                    playableRec *receivedPlayableChar = new playableRec(size, position);
                    receivedPlayableChar->setID(id);
                    playbleCharactersServerside.push_back(receivedPlayableChar);
                    
                    for (auto& character : playbleCharactersServerside) {
                        std::stringstream characterData;
                        characterData << character->getPosition().x << " ";
                        characterData << character->getPosition().y << " ";
                        characterData << character->getSize().x << " ";
                        characterData << character->getSize().y << " ";
                        characterData << character->getID();

                        std::string serializedCharacter = characterData.str();
        
                        // send back the new character to every clients if it's a hello message
                        zmq::message_t characterMessage(serializedCharacter.c_str(), serializedCharacter.size());
                        
                        publisher1.send(characterMessage, zmq::send_flags::none);
                        
                    }

                    
                    
                }
                // Send out each of the playable characters
            }
            std::cout << "Size " + std::to_string(playbleCharactersServerside.size()) << std::endl;

        }
        //std::cout << "debug1.1" << std::endl;
        //recieve another req rep message from client, this time being the updated movement of a character
        //send back an empty message
        //loop through the characters list and compare their IDs, if match, update that character with it's new movement location
        zmq::message_t movementRequest;
        if (socket2.recv(movementRequest, zmq::recv_flags::dontwait)) {
            //socket2.recv(movementRequest, zmq::recv_flags::none);
            std::string receivedMovement(static_cast<char*>(movementRequest.data()), movementRequest.size());
            //std::cout <<"recieved from client " + receivedMovement  << std::endl;
            // Deserialize the movement message into character ID and new position
            std::stringstream movementStream(receivedMovement);
            std::string id;
            float posX, posY, sizeX, sizeY;
            movementStream >> posX >> posY >> sizeX >> sizeY >> id;
            
            //std::cout << "debug2.1" << std::endl;
            // Loop through the characters list and compare their IDs
            for (auto& character : playbleCharactersServerside) {
                if (character->getID() == id) {
                    // Update the character's position with the new movement location
                    //std::cout << "char message debugging position server side" +  movementRequest.to_string() << std::endl;
                    //std::cout << "Id matched"  << std::endl;
                    character->setPosition(sf::Vector2f(posX, posY));
                    character->setSize(sf::Vector2f(sizeX, sizeY));
                    
                    std::stringstream characterData;
                    characterData << character->getPosition().x << " ";
                    characterData << character->getPosition().y << " ";
                    characterData << character->getSize().x << " ";
                    characterData << character->getSize().y << " ";
                    characterData << character->getID();
                    //std::cout << "debug5.1" << std::endl;
                    i++;
                    std::string serializedCharacter = characterData.str() + " " + std::to_string(i);
                    //std::cout << "server side publishing a character pos to each clients " +  serializedCharacter << std::endl;
                    // Publish the character to all clients
                    zmq::message_t characterMessage(serializedCharacter.c_str(), serializedCharacter.size());
                    
                    publisher.send(characterMessage, zmq::send_flags::none);
                }
            }
            //std::cout << "debug3.1" << std::endl;
            socket2.send(zmq::message_t(), zmq::send_flags::none);
        }
        

        //std::cout << "debug4.1" << std::endl;
        //publishing each characters to all the clients
        
        
        
    }
    return 0;
}
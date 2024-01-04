#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <cmath>
#include <chrono>
#include <mutex>
#include <condition_variable>
#include "Platforms.h"
#include <thread>
#include <zmq.hpp>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "v8helpers.h"
#include <cstdio>
#include "ScriptManager.h"
#include <v8.h>
#include <libplatform/libplatform.h>

std::mutex mutexName;


//to be threaded
// void timerClientSide(playableRec& player ){
//     //socket to send heartbeat
//     zmq::context_t context1 (1);
//     zmq::socket_t socket3(context1, zmq::socket_type::req);
//     socket3.connect("tcp://localhost:5563");
//     while(true){
        
        
//         std::stringstream characterStream;
//         characterStream << player.getID();
//         std::string serializedCharacter = characterStream.str();
//         // Send the serialized string to the server
//         zmq::message_t characterMessage(serializedCharacter.c_str(), serializedCharacter.size());
//         socket3.send(characterMessage, zmq::send_flags::none);
//         //socket3 recieve a respond
//         // Receive a response from the server (can be empty, or used for acknowledgment)
//         zmq::message_t serverResponseAFK;
//         socket3.recv(serverResponseAFK, zmq::recv_flags::none);
//         std::this_thread::sleep_for(std::chrono::seconds(1));
//     }
// }




// Function to handle the movement of the moving platform enemies
void moveMovingPlatform( movingPlatform& platform,  movingPlatform& platform2, movingPlatform& platform3, movingPlatform& platform4, movingPlatform& platform5, bullet& bullet, landPlatform& left, landPlatform& right ) {
    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(8));
        {
            // Lock the mutex to access shared data
            std::unique_lock<std::mutex> lock(mutexName);
            // for platforms collision
            platform.collideBullet(bullet.getGlobalBounds());
            platform.collidePlatform(600, left.getGlobalBounds());
            platform.collidePlatform(600, right.getGlobalBounds());
            platform.outOfMap(1000, 150.f, 1.f);

            platform2.collideBullet(bullet.getGlobalBounds());
            platform2.collidePlatform(600, left.getGlobalBounds());
            platform2.collidePlatform(600, right.getGlobalBounds());
            platform2.outOfMap(1000, 250.f, 1.f);

            platform3.collideBullet(bullet.getGlobalBounds());
            platform3.collidePlatform(600, left.getGlobalBounds());
            platform3.collidePlatform(600, right.getGlobalBounds());
            platform3.outOfMap(1000, 350.f, 1.f);

            platform4.collideBullet(bullet.getGlobalBounds());
            platform4.collidePlatform(600, left.getGlobalBounds());
            platform4.collidePlatform(600, right.getGlobalBounds());
            platform4.outOfMap(1000, 450.f, 1.f);

            platform5.collideBullet(bullet.getGlobalBounds());
            platform5.collidePlatform(600, left.getGlobalBounds());
            platform5.collidePlatform(600, right.getGlobalBounds());
            platform5.outOfMap(1000, 550.f, 1.f);

            // for stationary platform fluctuation
            //and.flux(-1, platform.getGlobalBounds());
        }
    }
}


int main() {

    //initializing V8
    std::unique_ptr<v8::Platform> platform = v8::platform::NewDefaultPlatform();
    v8::V8::InitializePlatform(platform.release());
    v8::V8::InitializeICU();
    v8::V8::Initialize();
    v8::Isolate::CreateParams create_params;
    create_params.array_buffer_allocator = v8::ArrayBuffer::Allocator::NewDefaultAllocator();
    v8::Isolate *isolate = v8::Isolate::New(create_params);
    
    {
        
        v8::Isolate::Scope isolate_scope(isolate); // must enter the virtual machine to do stuff
        v8::HandleScope handle_scope(isolate);

		// Best practice to isntall all global functions in the context ahead of time.
        v8::Local<v8::ObjectTemplate> global = v8::ObjectTemplate::New(isolate);
        // Bind the global 'print' function to the C++ Print callback.
        global->Set(isolate, "print", v8::FunctionTemplate::New(isolate, v8helpers::Print));
		// // Bind the global static factory function for creating new GameObject instances
		// global->Set(isolate, "gameobjectfactory", v8::FunctionTemplate::New(isolate, GameObject::ScriptedGameObjectFactory));
		// Bind the global static function for retrieving object handles
		global->Set(isolate, "gethandle", v8::FunctionTemplate::New(isolate, ScriptManager::getHandleFromScript));
        v8::Local<v8::Context> default_context =  v8::Context::New(isolate, NULL, global);
		v8::Context::Scope default_context_scope(default_context); // enter the context

        ScriptManager *sm = new ScriptManager(isolate, default_context); 

        int client = 0;
        std::vector <playableRec*> playbleCharacters; 

        zmq::context_t context(1);
        //  Socket to talk to server - pub sub for publishing new positions
        std::cout << "Connecting to server...\n" << std::endl;
        zmq::socket_t subscriber (context, zmq::socket_type::sub);
        subscriber.connect("tcp://localhost:5557");
        subscriber.setsockopt(ZMQ_SUBSCRIBE, "", 0);


        //  Socket to talk to server - pub sub for when new char connects
        std::cout << "Connecting to server...\n" << std::endl;
        zmq::socket_t subscriber1 (context, zmq::socket_type::sub);
        subscriber1.connect("tcp://localhost:5561");
        subscriber1.setsockopt(ZMQ_SUBSCRIBE, "", 0);


        //Socket to talk to server - req rep to send hello and recieve back ID
        zmq::socket_t socket(context, zmq::socket_type::req);
        socket.connect("tcp://localhost:5558");
        zmq::message_t request(5);
        memcpy(request.data(), "Hello", 5);
        socket.send(request, zmq::send_flags::none);
        zmq::message_t reply;
        socket.recv(reply, zmq::recv_flags::none);
        std::string receivedMessage(static_cast<char*>(reply.data()), reply.size());
        std::cout <<"Received: " << receivedMessage << std::endl;

        //socket to recieve new characters
        zmq::socket_t socket1(context, zmq::socket_type::req);
        socket1.connect("tcp://localhost:5559");

        //socket to update movement of the main character of each client new characters
        zmq::socket_t socket2(context, zmq::socket_type::req);
        socket2.connect("tcp://localhost:5560");

        //pub sub to recieve AFK message
        zmq::socket_t subscriber2 (context, zmq::socket_type::sub);
        subscriber2.connect("tcp://localhost:5562");
        subscriber2.setsockopt(ZMQ_SUBSCRIBE, "", 0);

        
    
        // Create a new context
		v8::Local<v8::Context> object_context = v8::Context::New(isolate, NULL, global);
		sm->addContext(isolate, object_context, "object_context");
        //recieve client number from server  - make new playblechar + add to local list send to server with req rep
        playableRec *playableChar = new playableRec(sf::Vector2f(30, 30), sf::Vector2f(300.f, 700.f));
        playableChar->exposeToV8(isolate, object_context);
        sm->addScript("increaseXVal", "changeSpeed.js", "object_context");

        playableChar->setID(receivedMessage);
        playbleCharacters.push_back(playableChar);


        //THREAD FOR AFK
        //std::thread timerClientSideThread(timerClientSide, std::ref(*playableChar));


        //send the created playableChar to the server - making it into a string
        std::stringstream characterStream;
        characterStream << playableChar->getPosition().x << " ";
        characterStream << playableChar->getPosition().y << " ";
        characterStream << playableChar->getSize().x << " ";
        characterStream << playableChar->getSize().y << " ";
        characterStream << playableChar->getID();
        std::string serializedCharacter = characterStream.str();
        std::cout << serializedCharacter << std::endl;
        // Send the serialized string to the server
        zmq::message_t characterMessage(serializedCharacter.c_str(), serializedCharacter.size());
        socket1.send(characterMessage, zmq::send_flags::none);

        //recieve empoty respond from server
        socket1.recv(reply, zmq::recv_flags::none);
        //recieve the new character from server and update own list 
    
        bool isGround = false;
        sf::RenderWindow window(sf::VideoMode(800, 600), "My window", sf::Style::Default);

        //to handle scrolling
        sf::View view;
        view.reset(sf::FloatRect(0, 0, 800, 600));
        view.setViewport(sf::FloatRect(0, 0, 1.0f, 1.0f));
        sf::Vector2f positionView(400,600);

        // Create a game timeline
        Timeline gameTime(nullptr, 1);
        

        // moving platform1
        movingPlatform *movingPlat1 = new movingPlatform(sf::Vector2f(30, 60), sf::Vector2f(150.f, 1.f));
        movingPlat1->setX(1);
        movingPlat1->setY(1);

        // moving platform2 
        movingPlatform *movingPlat2 = new movingPlatform(sf::Vector2f(30, 60), sf::Vector2f(250.f, 1.f));
        movingPlat2->setX(1);
        movingPlat2->setY(1);


        // moving platform2 
        movingPlatform *movingPlat3 = new movingPlatform(sf::Vector2f(30, 60), sf::Vector2f(350.f, 1.f));
        movingPlat3->setX(1);
        movingPlat3->setY(1);

        // moving platform2 
        movingPlatform *movingPlat4 = new movingPlatform(sf::Vector2f(30, 60), sf::Vector2f(450.f, 1.f));
        movingPlat4->setX(1);
        movingPlat4->setY(1);

        // moving platform2 
        movingPlatform *movingPlat5 = new movingPlatform(sf::Vector2f(30, 60), sf::Vector2f(550.f, 1.f));
        movingPlat5->setX(1);
        movingPlat5->setY(1);

        // land platform 
        landPlatform landShape(sf::Vector2f(1000.f, 15.f), sf::Vector2f(10.f, 905.f));
        landShape.setFillColor(sf::Color(255,0,0));
        landShape.setDeadPlatform(false);
        // land platform 
        landPlatform landShapeEdge(sf::Vector2f(800.f, 800.f), sf::Vector2f(700.f, 400.f));
        landShapeEdge.setDeadPlatform(false);
        landShapeEdge.setFillColor(sf::Color(255,255,0));

        // land platform 
        landPlatform *landShapeJumpOsb= new landPlatform(sf::Vector2f(200.f, 3000.f), sf::Vector2f(700.f, 10.f));
        landShapeJumpOsb->setDeadPlatform(true);
        landShapeJumpOsb->setFillColor(sf::Color(0,255,200));

        landPlatform *landShapeJumpOsb2= new landPlatform(sf::Vector2f(200.f, 3000.f), sf::Vector2f(-100.f, 10.f));
        landShapeJumpOsb2->setDeadPlatform(true);
        landShapeJumpOsb2->setFillColor(sf::Color(0,255,200));


        bullet *shotBullet = new bullet(sf::Vector2f(10, 40), sf::Vector2f(50000, 50000));
        
        //make event manager
        EventManager eventManager(&gameTime);
        //make event handler
        EventHandler eventHandler(playableChar, landShapeJumpOsb, 600 );
        
        //registering handlers to their specific events
        eventManager.registerHandler("EventPlayerMovement", &eventHandler);
        eventManager.registerHandler("EventCollision", &eventHandler);
        eventManager.registerHandler("DeadMapLimit", &eventHandler);
        eventManager.registerHandler("EventNewSpawn", &eventHandler);
        //REGISTER HERE



        sf::Texture texture;
        if (!texture.loadFromFile("res/lovebird-8244066_1280.jpg")) {
            std::cout << "Load failed" << std::endl;
        }

        // Create a thread for moving platform logic
        std::thread platformThread(moveMovingPlatform, std::ref(*movingPlat1),  std::ref(*movingPlat2),  std::ref(*movingPlat3),   std::ref(*movingPlat4),  std::ref(*movingPlat5),  std::ref(*shotBullet), std::ref(*landShapeJumpOsb), std::ref(*landShapeJumpOsb2));
        int64_t last_time = gameTime.getTime();
        int i = 0;
        int speedUp = 1;
        while (window.isOpen()) {
        //recieve when new playablerecs from server with req rep + don't wait flag 
        //loop through the current playbleCharacters vector, if same id isn't found
            //make new character with that information and push it back  
            //playbleCharacters.push_back();
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed)
                    window.close();

                if (event.type == sf::Event::KeyPressed) {
                    if (event.key.code == sf::Keyboard::P) {
                        if (gameTime.isPaused()) {
                            gameTime.unpause();
                        } else {
                            gameTime.pause();
                        }
                    } else if (event.key.code == sf::Keyboard::Num1) {
                        // Change timeline speed to 0.5x
                        gameTime.changeTic(2);
                    } else if (event.key.code == sf::Keyboard::Num2) {
                        // Change timeline speed to 1x (real time)
                        gameTime.changeTic(4);
                    } else if (event.key.code == sf::Keyboard::Num3) {
                        // Change timeline speed to 2x
                        gameTime.changeTic(8);
                    }
                }
            }

            window.clear(sf::Color::Black);


            // Calculate elapsed time based on game timeline - do this in server and then send to client 
            float elapsedTime = gameTime.getTime() ; // Convert to seconds
            int64_t frame_delta = elapsedTime - last_time;
            last_time = elapsedTime;

            if(subscriber1.recv(reply, zmq::recv_flags::dontwait)){
                //std::cout << reply.to_string() << std::endl;
                //std::cout << "Debug " << std::endl;
                std::string receivedCharacter(static_cast<char*>(reply.data()), reply.size());
            
                // Deserialize the received message (custom string format) into a playable character object
                std::stringstream characterStream1(receivedCharacter);
                float posX, posY, sizeX, sizeY;
                std::string id;
                characterStream1 >> sizeX >> sizeY >> posX >> posY >> id;
                
                
                //std::cout << "recieved a new character " + receivedCharacter << std::endl;
                // Check if the character already exists in the client's list, and add it if it doesn't
                bool isNewCharacter = true;
                for (const auto& existingCharacter : playbleCharacters) {
                    if (existingCharacter->getID() == id) {
                        isNewCharacter = false;
                        break;
                        
                    }
                    //std::cout << "Debug 3" << std::endl;
                }
                if (isNewCharacter) {
                    // Create a new playable character object from the received data
                    sf::Vector2f position(posX, posY);
                    sf::Vector2f size(sizeX, sizeY);
                    //raise eventNewSpawn
                    EventNewSpawn *eventNewSpawn = new EventNewSpawn(position, size, id, &playbleCharacters);
                    eventManager.raise(eventNewSpawn);
                }

            } 
            

            //do movement of first character
            //std::cout << "Debug 5 " << std::endl;
            //EVENT HANDLING HANDLING THE Q
            bool movedClient = false;
            bool reload = false;
            
            if(window.hasFocus()){


                    
                    //send the character with updated movement  to server via req rep
                    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) && playableChar->getLives() > 0)
                    {
                        //sm-run one ?
                        playableChar->movement(isGround, landShape.getGlobalBounds(), frame_delta);
                        //create event
                        EventPlayerMovement *eventMovement = new EventPlayerMovement(sf::Keyboard::A);
                        //pass to manager goes with key A
                        //RAISE HERE
                        eventManager.raise(eventMovement);
                        movedClient = true;
                    }
                    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) && playableChar->getLives() > 0)
                    {
                        playableChar->movement(isGround, landShape.getGlobalBounds(), frame_delta);
                        //create event
                        EventPlayerMovement *eventMovement = new EventPlayerMovement(sf::Keyboard::S);
                        //pass to manager goes with key A
                        //RAISE HERE
                        eventManager.raise(eventMovement);
                        movedClient = true;
                    }
                    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) && playableChar->getLives() > 0)
                    {
                        playableChar->movement(isGround, landShape.getGlobalBounds(), frame_delta);
                        //create event
                        EventPlayerMovement *eventMovement = new EventPlayerMovement(sf::Keyboard::D);
                        //pass to manager goes with key A
                        //RAISE HERE
                        eventManager.raise(eventMovement);
                        movedClient = true;
                    }
                    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) && playableChar->getLives() > 0)
                    {
                        playableChar->movement(isGround, landShape.getGlobalBounds(), frame_delta);
                        //create event
                        EventPlayerMovement *eventMovement = new EventPlayerMovement(sf::Keyboard::W);
                        //pass to manager goes with key A
                        //RAISE HERE
                        eventManager.raise(eventMovement);
                        movedClient = true;
                    }
                    if (sf::Keyboard::isKeyPressed(sf::Keyboard::V) && playableChar->getLives() > 0  && speedUp > 0)
                    {
                        
                        sm->runOne("increaseXVal", reload, "object_context");
					    reload = false;
                        std::cout << std::to_string(playableChar->getXVel()) << std::endl;
                        speedUp --;
                    }
                    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && playableChar->getLives() > 0)
                    {   
                        shotBullet->setPosition(playableChar->getPosition().x + 15, playableChar->getPosition().y);
                        
                    }

                    if(playableChar->getPosition().y >= 900.0){
                        EventDeathOffMap *eventDeath = new EventDeathOffMap();
                        eventManager.raise(eventDeath);
                        movedClient = true;
                    }

                    if(playableChar->contactEnemy(movingPlat1->getGlobalBounds())){
                        EventDeathOffMap *eventDeath = new EventDeathOffMap();
                        eventManager.raise(eventDeath);
                        movedClient = true;
                    }

                    if(playableChar->contactEnemy(movingPlat2->getGlobalBounds())){
                        EventDeathOffMap *eventDeath = new EventDeathOffMap();
                        eventManager.raise(eventDeath);
                        movedClient = true;
                    }

                    if(playableChar->contactEnemy(movingPlat3->getGlobalBounds())){
                        EventDeathOffMap *eventDeath = new EventDeathOffMap();
                        eventManager.raise(eventDeath);
                        movedClient = true;
                    }

                    if(playableChar->contactEnemy(movingPlat4->getGlobalBounds())){
                        EventDeathOffMap *eventDeath = new EventDeathOffMap();
                        eventManager.raise(eventDeath);
                        movedClient = true;
                    }

                    if(playableChar->contactEnemy(movingPlat5->getGlobalBounds())){
                        EventDeathOffMap *eventDeath = new EventDeathOffMap();
                        eventManager.raise(eventDeath);
                        movedClient = true;
                    }

                    if(playableChar->getGlobalBounds().intersects(landShapeJumpOsb->getGlobalBounds())){
                        EventCollision *eventCol = new EventCollision();
                        eventManager.raise(eventCol);
                        movedClient = true;
                    }
                    
                    
                    if(playableChar->getGlobalBounds().intersects(landShapeJumpOsb2->getGlobalBounds())){
                        EventCollision *eventCol = new EventCollision();
                        eventManager.raise(eventCol);
                        movedClient = true;
                    }

                    if(playableChar->getLives() == 0){
                       playableChar->setFillColor(sf::Color(128,128,128));
                    }

                    if(movedClient){
                        std::stringstream characterData;
                        characterData << playableChar->getPosition().x << " ";
                        characterData << playableChar->getPosition().y << " ";
                        characterData << playableChar->getSize().x << " ";
                        characterData << playableChar->getSize().y << " ";
                        characterData << playableChar->getID();
                        ///std::cout << "debugging x pos" + std::to_string(playableChar->getPosition().y) << std::endl;
                        std::string serializedCharacter = characterData.str();
                        //std::cout << "bug fix client sending char to servent " + serializedCharacter  << std::endl;
                        //std::cout << "Debug 7 " << std::endl;
                        zmq::message_t characterMessage(serializedCharacter.c_str(), serializedCharacter.size());
                        //std::cout << "client sending character with update movement to server" + characterMessage.to_string() << std::endl;
                        socket2.send(characterMessage, zmq::send_flags::none);

                        // Receive a response from the server (can be empty, or used for acknowledgment)
                        zmq::message_t serverResponse;
                        socket2.recv(serverResponse, zmq::recv_flags::none);
                    }
                
                    shotBullet->bulletDir();
                playableChar->setTexture(&texture);
            }    
            eventManager.doEvent();
            //std::cout << "Debug 6 " << std::endl;
            
            
            
        

            //error here in this loop 
            //take  published respond from the server of every characters in the list with updated location
            //client update their list with this list
            zmq::message_t characterResponse;
            
            if (subscriber.recv(characterResponse, zmq::recv_flags::dontwait)) {
                
                std::string receivedCharacter(static_cast<char*>(characterResponse.data()), characterResponse.size());
                i++;
                // Deserialize the received message into a playable character object
                std::stringstream characterStream(receivedCharacter);
                float posX, posY, sizeX, sizeY;
                std::string id;
                
                characterStream >> posX >> posY >> sizeX >> sizeY >> id;
                
                // establish position of each characters
                sf::Vector2f position(posX, posY);
                sf::Vector2f size(sizeX, sizeY);

                //check it's id and update it's position
                for (auto& existingCharacter : playbleCharacters) {
                    if (existingCharacter->getID() == id && id != playableChar->getID()) {
                        //std::cout << existingCharacter->getID() + " enter if statement " + id << std::endl;
                        // Update the existing character's position
                        existingCharacter->setPosition(position);
                        existingCharacter->setSize(size);
                        //std::cout << "ID matched " +  existingCharacter->getID() << std::endl;
                    }
                }
            }

            // //loop to update a character connection status if they afk 
            // zmq::message_t characterResponse2;
            // if (subscriber2.recv(characterResponse2, zmq::recv_flags::dontwait)) {
                
            //     std::string receivedCharacter(static_cast<char*>(characterResponse2.data()), characterResponse2.size());
            //     // Deserialize the received message into a playable character object
            //     std::stringstream characterStream(receivedCharacter);
            //     std::string id;
                
            //     characterStream >>  id;
            //     //check it's id and update it's position
            //     for (auto& existingCharacter : playbleCharacters) {
            //         if (existingCharacter->getID() == id ) {
            //             existingCharacter->setConnected(false);
            //         }
            //     }
            // }

            positionView.x = playableChar->getPosition().x + 10 - (400);
            positionView.y = playableChar->getPosition().y + 10 - (300);

            if(positionView.x < 0){
                positionView.x = 0;
            }
            if(positionView.y < 0){
                positionView.y = 0;
            }

            view.reset(sf::FloatRect(positionView.x, positionView.y, 800, 600));

            window.setView(view);
            window.draw(landShape);
            if(movingPlat1->getAlive()){
                window.draw(*movingPlat1);
            }
            if(movingPlat2->getAlive()){
                window.draw(*movingPlat2);
            }
            if(movingPlat3->getAlive()){
                window.draw(*movingPlat3);
            }
            if(movingPlat4->getAlive()){
                window.draw(*movingPlat4);
            }
            if(movingPlat5->getAlive()){
                window.draw(*movingPlat5);
            }

            //window.draw(movingPlatVert);
            //window.draw(landShapeEdge);
            window.draw(*landShapeJumpOsb);
            window.draw(*landShapeJumpOsb2);
            window.draw(*shotBullet);
            //draw every characters in the character list
            for (auto& existingCharacter : playbleCharacters) {
                
                if(existingCharacter->getConnected() == true ){
                    if(existingCharacter->getLives() == 0){
                        existingCharacter->setFillColor(sf::Color(128,128,128));
                    }
                    window.draw(*existingCharacter);
                }
            }

            //window.draw(playableChar);
            //std::cout << "Size" + std::to_string(playbleCharacters.size()) << std::endl;
            window.display();
        }

        // Join the platform thread before exiting
        platformThread.join();

    }
    
    isolate->Dispose();
    v8::V8::Dispose();
    v8::V8::ShutdownPlatform();


    return 0;
}

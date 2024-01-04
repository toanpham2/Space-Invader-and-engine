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
#include <queue>
#include "v8helpers.h"
#include <cstdio>

//pass in a newCharter pointer in memory
EventHandler :: EventHandler(playableRec *newCharacter, landPlatform *platformNew, int deathZoneNew ){
    playerChar = newCharacter;
    platform = platformNew;
    deathZone = deathZoneNew;
}

EventHandler :: EventHandler(){
}

//update that character position
void EventHandler :: onEventInput(Event *e){

    //movement input event handler
    if (e->getType() == "EventPlayerMovement") {
        EventPlayerMovement* movementEvent = dynamic_cast<EventPlayerMovement*>(e);
        if (movementEvent) {
            sf::Keyboard::Key keyPressed = movementEvent->getChar();
            // Implement the logic for handling the player movement event
            if(keyPressed == sf::Keyboard::A ){
                float newXPos = playerChar->getPosition().x - playerChar->getXVel() * playerChar->getmovementFactor();
                playerChar->setPosition(newXPos, playerChar->getPosition().y);
            }
            if(keyPressed == sf::Keyboard::W ){
                float newYPos = playerChar->getPosition().y - playerChar->getYVel() * playerChar->getmovementFactor();
                playerChar->setPosition( playerChar->getPosition().x, newYPos);
            }
            if(keyPressed == sf::Keyboard::S ){
                float newYPos = playerChar->getPosition().y + playerChar->getYVel() * playerChar->getmovementFactor();
                playerChar->setPosition( playerChar->getPosition().x, newYPos);
            }
            if(keyPressed == sf::Keyboard::D ){
                float newXPos = playerChar->getPosition().x + playerChar->getXVel() * playerChar->getmovementFactor();
                playerChar->setPosition(newXPos, playerChar->getPosition().y);
            }
        }
    }


    //collision event handler
    if(e->getType() == "EventCollision"){
        EventCollision* collisionEvent = dynamic_cast<EventCollision*>(e);
        //will implement non death collision type later 
        if (collisionEvent) {
            if(platform->getDeadPlatform() == true){
                playerChar ->setPosition( playerChar->getSpawnX(), playerChar->getSpawnY());
            }    
        }
    }

    //death event handler
    if(e->getType() =="DeadMapLimit"){
        EventDeathOffMap* deathEvent = dynamic_cast<EventDeathOffMap*>(e);
        //will implement non death collision type later 
        if (deathEvent) {
            std::cout << "Died " << std::endl;
            playerChar ->setPosition( playerChar->getSpawnX(), playerChar->getSpawnY());
            if(playerChar->getLives() > 0){
                playerChar -> setLives(playerChar->getLives()-1);
            }
            std::cout << "Lives left: "+ std::to_string(playerChar->getLives()) + " " + playerChar->getID() << std::endl;
        }
    }

    //new spawn event handler
    if(e->getType() == "EventNewSpawn"){
        EventNewSpawn* spawnEvent = dynamic_cast<EventNewSpawn*>(e);
        if(spawnEvent){
            playableRec *receivedPlayableChar = new playableRec(spawnEvent->getPos(), spawnEvent->getSize());
            receivedPlayableChar->setID(spawnEvent->getIDEvent());
            spawnEvent->getVecPointer()->push_back(receivedPlayableChar);
        }
    }
}
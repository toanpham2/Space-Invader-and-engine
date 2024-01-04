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


void EventNewSpawn :: dummy(){

}

EventNewSpawn :: EventNewSpawn (sf::Vector2f newPos, sf::Vector2f newSize, std::string id, std::vector<playableRec*> *newPointerToVec) : Event(){
    setType( "EventNewSpawn");
    setPos(newPos);
    setSize(newSize);
    setID(id);
    vecPointer = newPointerToVec;
}
sf::Vector2f EventNewSpawn :: getPos(){
    return pos;
}
void EventNewSpawn :: setPos(sf::Vector2f newPosFunc){
    pos = newPosFunc;
}

sf::Vector2f EventNewSpawn :: getSize(){
    return size;
}

void EventNewSpawn :: setSize(sf::Vector2f newSizefunc){
    size = newSizefunc;
}

std::string EventNewSpawn :: getIDEvent(){
    return id;
}

void EventNewSpawn :: setID(std::string newIDfunc){
    id = newIDfunc;
}

std::vector<playableRec*>* EventNewSpawn :: getVecPointer(){
    return vecPointer;
}





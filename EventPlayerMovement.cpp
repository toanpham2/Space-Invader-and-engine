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


void EventPlayerMovement :: dummy(){

}

EventPlayerMovement :: EventPlayerMovement(sf::Keyboard::Key keyPressed) : Event(){
    setChar(keyPressed);
    setType( "EventPlayerMovement");
}

sf::Keyboard::Key EventPlayerMovement :: getChar(){
    return currInput;
}

void EventPlayerMovement :: setChar(sf::Keyboard::Key newKey){
    currInput = newKey;
}
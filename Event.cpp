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
#include "Platforms.h"

Event :: Event(){
}

std::string Event :: getType(){
    return type;
}

void Event :: setType(std::string newType){
    type = newType;
}

int64_t Event :: getTime(){
    return time;
}

void Event :: setTime(int64_t newTime){
    time = newTime;
}
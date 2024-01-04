#include "Platforms.h"
#include <SFML/System/Time.hpp>
#include <iostream>
#include <chrono>
#include <ctime>
//constructor for Timeline
Timeline::Timeline(Timeline *anchor, int64_t tic){
    this->anchor =  anchor;
    this->tic = tic;
    paused = false;
    elapsed_paused_time = 0;
    if(anchor){
        start_time = anchor->getTime();
    }
    else{
        start_time = sfTime();
    }

}

//function to get the time
int64_t Timeline::getTime(){
    std::unique_lock<std::mutex> lock(m);
    int64_t currTime = sfTime();
    // std::cout <<  currTime << std::endl;
    // std::cout <<  start_time << "this is start time" << std::endl;
    if(paused){
        //std::cout <<  (currTime - start_time)/tic <<"if paused" << std::endl;
        //return (currTime - start_time)/tic;
        return (0);
        
    }else{
        //std::cout <<  (currTime - start_time - elapsed_paused_time) / tic <<"if unpaused" << std::endl;
        return ((currTime - start_time - elapsed_paused_time) / tic);
    }

}

//function to pause 
void Timeline::pause(){
    std::unique_lock<std::mutex> lock(m);
    if(!paused){
        paused = true;
        int64_t currTime = sfTime();
        int64_t currPausedTime = (currTime - start_time)/tic;
        last_paused_time = currPausedTime;
        
    }    
}

//function to unpause g
void Timeline::unpause(){
    std::unique_lock<std::mutex> lock(m);
    if(paused){
        paused = false;
        int64_t currTime = sfTime();
        int64_t currPausedTime = (currTime - start_time)/tic;
        elapsed_paused_time += (currPausedTime - last_paused_time) * tic;
        
    }    

}

//function to change tic
void Timeline::changeTic(int ticNew){
    std::unique_lock<std::mutex> lock(m);
    tic = ticNew;
}

//Function to check if time is Paused
bool Timeline::isPaused(){
    std::unique_lock<std::mutex> lock(m);
    return paused;
}

//Function to call sftime
int64_t Timeline::sfTime(){
    using namespace std::chrono;
    return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}
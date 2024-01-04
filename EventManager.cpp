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
#include <thread>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <queue>
#include "Platforms.h"


EventManager :: EventManager(Timeline *gameTime){
	gameTimeEM = gameTime;
}

void EventManager :: registerHandler(std::string eventType, EventHandler* handler){
    //register a list of events to the handler for private map 
	handlers[eventType] = handler;
}

void EventManager :: sendEvent(Event* event){
	std::string eventType = event->getType();
        if (handlers.find(eventType) != handlers.end()) {
            EventHandler* handler = handlers[eventType];
            //fix this event to player input event
            handler->onEventInput(event);
        }
}

void EventManager :: doEvent(){
	int64_t currentTimestamp = gameTimeEM->getTime(); 

        std::lock_guard<std::mutex> lock(eventMutex);

        while (!raised_events.empty()) {
            Event* event = raised_events.front();

            if (event->getTime() <= currentTimestamp) {
                raised_events.pop();
                std::string eventType = event->getType();
                sendEvent(event);
                delete event;
            } else {
                break;
            }
        }
}

void EventManager :: raise(Event *e){
    std::lock_guard<std::mutex> lock(eventMutex);
    raised_events.push(e);

}
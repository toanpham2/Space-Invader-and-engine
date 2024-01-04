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

//header for playable character
class playableRec : public sf::RectangleShape
{
public :

    playableRec(const sf::Vector2f& xandy, const sf::Vector2f& position);
    bool movement(bool grounded, const sf::FloatRect ground, int64_t timeT);
    void setID(std::string newID);
    void deathZoneY(int bounderies);
    void deathZoneX( const sf::FloatRect bounds);
    std::string getID() const;
    void setSpawnX(float spawinPointX);
    void setSpawnY(float spawinPointY);
    float getSpawnX();
    float getSpawnY();
    void setRecievedTime(int64_t time);
    int64_t getRecievedTime();
    void setConnected(bool status);
    bool getConnected();
private :

    //speed of moving character
    float xCharVel = 3000;
    float yCharVel = 4000;
    float spawninX = 100.0;
    float spawninY = 150.0;
    int64_t recievedTime;
    bool connected = true;
    std::string ID ; 
    EventManager* eventManager;
};

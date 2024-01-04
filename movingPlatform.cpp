#include <iostream>
#include "Platforms.h"
//constructor for movingPlatform
movingPlatform::movingPlatform(const sf::Vector2f& xandy, const sf::Vector2f& position) : sf::RectangleShape(xandy){
    //moving platform    
    setPosition(position);
}

int movingPlatform::getX(){
    return xVel;
}

int movingPlatform::getY(){
    return yVel;
}

void movingPlatform::setX(int newVel){
    xVel = newVel;
}

void movingPlatform::setY(int newVel){
    yVel = newVel;
}

void movingPlatform::setAlive(bool aliveStat){
    alive = aliveStat;
}

bool movingPlatform::getAlive(){
    return alive;
}

//function for moving platform to collide with stationary platform
void movingPlatform::collidePlatform(const int bounderies, const sf::FloatRect bounds){
    //physics of moving platform

        if(getGlobalBounds().intersects(bounds)){
            xVel *= -1 ;
        }
        float newXPos = getPosition().x + xVel ;
        float newYPos = getPosition().y + yVel/3  ;
        setPosition(newXPos, newYPos);

}

void movingPlatform:: outOfMap(const int bounderies, float xSpawn, float ySpawn){
    if(getPosition().y > (bounderies) && spawnTurn > 0){
        //std::cout<<"respawn enemies"<<std::endl;
        setPosition(xSpawn, ySpawn);
        setAlive(true);
        spawnTurn = spawnTurn - 1;
    }

}

//function for moving platform to collide with stationary platform
void movingPlatform::collidePlatformVert(const int bounderies, const sf::FloatRect bounds){
    //physics of moving platform

        if(getPosition().y < (bounderies * -1) || getGlobalBounds().intersects(bounds)){
            yVel *= -1 ;
        }
        float newXPos = getPosition().x + xVel ;
        float newYPos = getPosition().y + yVel/2  ;
        setPosition(newXPos, newYPos);

}

void movingPlatform::collideBullet(const sf::FloatRect bounds){
    if(getGlobalBounds().intersects(bounds)){
        setAlive(false);
    }
    float newXPos = getPosition().x + xVel ;
    float newYPos = getPosition().y + yVel/3  ;
    setPosition(newXPos, newYPos);
}

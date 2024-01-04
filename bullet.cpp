#include <iostream>
#include "Platforms.h"
//constructor for bullet
bullet::bullet(const sf::Vector2f& xandy, const sf::Vector2f& position) : sf::RectangleShape(xandy){
    //bullet  
    setPosition(position);
    setFillColor(sf::Color(255, 255, 0));
}

void bullet::setY(int newVel){
    yVelBullet = newVel;
}

int bullet::getY(){
    return yVelBullet;
}

void bullet::bulletDir(){
    float newYPos = getPosition().y - yVelBullet;
    float newXPos = getPosition().x;
    setPosition(newXPos, newYPos);
}
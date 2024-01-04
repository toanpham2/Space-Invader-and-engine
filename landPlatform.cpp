#include "Platforms.h"
#include <iostream>
//constructor for stationaryPlatform
landPlatform :: landPlatform(const sf::Vector2f& xandy, const sf::Vector2f& position) : sf::RectangleShape(xandy){ 
    setPosition(position);
     //fill out the color of the shape
    setFillColor(sf::Color(100, 250, 50));
    // set a 10-pixel wide orange outline
    setOutlineThickness(10.f);
    setOutlineColor(sf::Color(250, 150, 100));
}

//function for stationary platform to fluctuate up and down
void landPlatform::flux(int upDown,  const sf::FloatRect bounce ){
    //physics of land platform
    if(getGlobalBounds().intersects(bounce)){
        fluxVal *= (upDown );
        float newYPos= getPosition().y + fluxVal ;
        setPosition(getPosition().x, newYPos);
    }
    

}

bool landPlatform :: getDeadPlatform(){
    return deadPlatform;
}

void landPlatform :: setDeadPlatform(bool platTypeStat){
    deadPlatform = platTypeStat;
}
#include "Platforms.h"
#include <iostream>
#include <filesystem>
#include <cstring>
#include "ScriptManager.h"
#include "v8helpers.h"

v8::Local<v8::Object> playableRec::exposeToV8(v8::Isolate *isolate, v8::Local<v8::Context> &context, std::string context_name)
{
	std::vector<v8helpers::ParamContainer<v8::AccessorGetterCallback, v8::AccessorSetterCallback>> v;
	v.push_back(v8helpers::ParamContainer<v8::AccessorGetterCallback, v8::AccessorSetterCallback>("xSpeed", getXSpeed, setXSpeed));
	return v8helpers::exposeToV8("playableRec0", this, v, isolate, context, context_name);
}


void playableRec::getXSpeed(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	v8::Local<v8::Object> self = info.Holder();
	v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
	void* ptr = wrap->Value();
	int x_val = static_cast<playableRec*>(ptr)-> xCharVel;
	info.GetReturnValue().Set(x_val);
}



void playableRec::setXSpeed(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info)
{
	v8::Local<v8::Object> self = info.Holder();
	v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
	void* ptr = wrap->Value();
	static_cast<playableRec*>(ptr)->xCharVel = value->Int32Value();
}

//constructor for playable character
playableRec :: playableRec(const sf::Vector2f& xandy, const sf::Vector2f& position) : sf::RectangleShape(xandy){
    //moving platform    
    setPosition(position);
     //fill out the color of the shape
    setFillColor(sf::Color(255, 0, 0));
    
}
//GET REAL TIME
//function for playable character
//params are for jump function, have not implemented
//refracr for manager + handler
void playableRec :: movement(bool grounded, const sf::FloatRect ground, int64_t timeT){
    movementFactor = float(timeT) / 10000; 
   
}

bool playableRec :: contactEnemy(const sf::FloatRect ground){
    if(getGlobalBounds().intersects(ground)){
        return true;
    }
    else{
        return false;
    }
}



std::string playableRec :: getID() const{
    return ID;
}

void playableRec :: setID(std::string newID){
    ID = newID;
}

void playableRec :: setSpawnX(float val){
    spawninX = val;
}


void playableRec :: setRecievedTime(int64_t time){
    recievedTime = time;
}
    
void playableRec :: setSpawnY(float val){
    spawninY = val;
}

float playableRec :: getSpawnX(){
    return 300;
}

float playableRec :: getSpawnY(){
    return 780;
}

int64_t playableRec :: getRecievedTime(){
    return recievedTime;
}

void playableRec :: setConnected(bool status){
    connected = status ;
}

bool playableRec :: getConnected(){
    return connected ;
}

int playableRec :: getXVel(){
    return xCharVel;
}

float playableRec :: getYVel(){
    return yCharVel;
}

void playableRec :: setXVel(int val){
    xCharVel = val;
}

void playableRec :: setYVel(float val){
    yCharVel = val;
}

void playableRec ::  setmovementFactor(float val){
    movementFactor = val;
}

float playableRec :: getmovementFactor(){
    return movementFactor;
}

int playableRec :: getLives(){
    return lives;
}

void playableRec :: setLives(int newLives){
    lives = newLives;
}

void playableRec :: setAmmo(int ammoCount){
    ammo = ammoCount;
}

int playableRec :: getAmmo(){
   return ammo; 
}

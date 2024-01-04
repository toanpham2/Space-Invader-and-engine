#ifndef MOVEPLATFORM
#define MOVEPLATFORM

#include <v8/v8.h>
#include <filesystem>
#include <SFML/Graphics.hpp>
#include <zmq.hpp>
#include <thread>
#include <mutex>
#include <queue>
//header for moving platform
class movingPlatform : public sf::RectangleShape
{
public :
    movingPlatform(const sf::Vector2f& xandy, const sf::Vector2f& position);
    void collidePlatform(const int bounderies, const sf::FloatRect bounds);
    void collidePlatformVert(const int bounderies, const sf::FloatRect bounds);
    void collideBullet(const sf::FloatRect bounds);
    void outOfMap(const int bounderies, float xSpawn, float ySpawn);
    void setX(int newVel);
    int getX (); 
    void setY(int newVel);
    int getY ();
    void setAlive(bool aliveStat);
    bool getAlive();         
private:
    //speed of moving platform
    float xVel = 1;
    float yVel = 0;
    bool alive = true;
    int spawnTurn = 3;
};


//header stationary platform
class landPlatform : public sf::RectangleShape
{
public :

    landPlatform(const sf::Vector2f& xandy, const sf::Vector2f& position);
    void flux( int upDown, const sf::FloatRect bounds);
    bool getDeadPlatform();
    void setDeadPlatform(bool deadStatus);
private :
    //how much a stationary platform fluctuates when under collision from moving platform 
    float fluxVal = 20;
    bool deadPlatform;
};

class bullet: public sf::RectangleShape
{
public :
    bullet(const sf::Vector2f& xandy, const sf::Vector2f& position);
    void bulletDir();
    void setY(int newVel);
    int getY ();        
private:
    //speed of bullet
    float yVelBullet = 1;
};


//header for playable character
class playableRec : public sf::RectangleShape
{
public :
    
    playableRec(const sf::Vector2f& xandy, const sf::Vector2f& position);
    v8::Local<v8::Object> exposeToV8(v8::Isolate *isolate, v8::Local<v8::Context> &context, std::string context_name="default");
    static void setXSpeed(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info);
    static void getXSpeed(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info); // note return type
    void movement(bool grounded, const sf::FloatRect ground, int64_t timeT);
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
    int getXVel();
    float getYVel();
    float getmovementFactor();
    void setmovementFactor(float val);
    void setXVel(int val);
    void setYVel(float val);
    int getLives();
    void setLives(int newLives);
    void setAmmo(int ammoCount);
    int getAmmo();
    bool contactEnemy(const sf::FloatRect ground);
    int xCharVel = 3000;
private :

    //speed of moving character
    
    float yCharVel = 4000;
    float spawninX = 300;
    float spawninY = 780;
    float movementFactor;
    int64_t recievedTime;
    bool connected = true;
    std::string ID ; 
    int lives = 5;
    int ammo = 10;

};



class Timeline {

//header for timeline class
public:
    Timeline(Timeline *anchor, int64_t tic);
    int64_t getTime(); //this can be game or system time implementation
    void pause();
    void unpause();
    void changeTic(int tic); //optional
    bool isPaused(); //optional

private:
    std::mutex m; //if tics can change size and the game is multithreaded
    int64_t start_time; //the time of the *anchor when created
    int64_t elapsed_paused_time;
    int64_t last_paused_time;
    int64_t tic; //units of anchor timeline per step
    bool paused;
    Timeline *anchor; //for most general game time, system library pointer
    int64_t sfTime();

};

class Event {
	public:
		//Type as string 
        Event();
		std::string getType();
        void setType(std::string newType);
        int64_t getTime();
        void setTime(int64_t newTime);
        virtual void dummy() = 0;
        //add time
    private:
        std::int64_t time;
        std::string type;
};

class EventPlayerMovement : public Event {
	public:
        void dummy() override;
        EventPlayerMovement(sf::Keyboard::Key KeyPressed);
		sf::Keyboard::Key getChar();
        void setChar(sf::Keyboard::Key NewInput);

    private:
        sf::Keyboard::Key currInput;
};


class EventCollision : public Event {
    public :
        void dummy() override;
        EventCollision();


};

class EventDeathOffMap : public Event {
    public :
        void dummy() override;
        EventDeathOffMap();
};

class EventHandler {
	public:
        EventHandler(playableRec *newCharacter, landPlatform *platformNew, int deathZoneNew);
        EventHandler();
		void onEventInput(Event *e);    
    private:
        //pointer to playablleChar
        playableRec *playerChar;
        //pointer to a platform
        landPlatform *platform;
        int deathZone;

};

class EventNewSpawn : public Event {
    public :
        void dummy() override;
        EventNewSpawn(sf::Vector2f newPos, sf::Vector2f newSize, std::string id, std::vector<playableRec*>* newPointerToVec);
        sf::Vector2f getPos();
        void setPos(sf::Vector2f newPosFunc);
        sf::Vector2f getSize();
        void setSize(sf::Vector2f newSizefunc);
        std::string getIDEvent();
        void setID(std::string newIDfunc);
        std::vector<playableRec*>* getVecPointer();

    private:
        sf::Vector2f pos;
        sf::Vector2f size;
        std::string id;
        std::vector<playableRec*> *vecPointer;
};


class EventManager {
	public:
		EventManager(Timeline *gameTime);
        void registerHandler(std::string eventType, EventHandler* handler);
		void raise(Event *e);
        void doEvent();
        void sendEvent(Event* event);
	private:
        Timeline *gameTimeEM; 
	    std::map<std::string, EventHandler*> handlers;
        std::queue<Event*> raised_events;
		std::mutex eventMutex;
};






#endif
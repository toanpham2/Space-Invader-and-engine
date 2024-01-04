#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <cmath>
#include <chrono>
#include <mutex>
#include <condition_variable>
#include "Platforms.h"
#include <thread>



std::mutex mutexName;

// Function to handle the movement of the moving platform
void moveMovingPlatform( movingPlatform& platform, landPlatform& land) {
    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(8));
        {
            // Lock the mutex to access shared data
            std::unique_lock<std::mutex> lock(mutexName);
            // for platforms collision
            platform.collidePlatform(800, land.getGlobalBounds());
            // for stationary platform fluctuation
            land.flux(-1, platform.getGlobalBounds());
        }
    }
}

int main() {

    

    bool isGround = false;

    sf::RenderWindow window(sf::VideoMode(800, 600), "My window", sf::Style::Default);

    // Create a game timeline
    Timeline gameTime(nullptr, 1);

    // moving platform
    movingPlatform movingPlat(sf::Vector2f(60, 30), sf::Vector2f(600.f, 450.f));

    // land platform
    landPlatform landShape(sf::Vector2f(800.f, 800.f), sf::Vector2f(-300.f, 400.f));

    // land platform
    landPlatform landShapeEdge(sf::Vector2f(800.f, 800.f), sf::Vector2f(-300.f, 400.f));

    // controllable char
    playableRec playableChar(sf::Vector2f(30, 30), sf::Vector2f(100.f, 150.f));

    sf::Texture texture;
    if (!texture.loadFromFile("res/lovebird-8244066_1280.jpg")) {
        std::cout << "Load failed" << std::endl;
    }

    // Create a thread for moving platform logic
    std::thread platformThread(moveMovingPlatform, std::ref(movingPlat), std::ref(landShape));
    int64_t last_time = gameTime.getTime();
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::P) {
                    if (gameTime.isPaused()) {
                        gameTime.unpause();
                    } else {
                        gameTime.pause();
                    }
                } else if (event.key.code == sf::Keyboard::Num1) {
                    // Change timeline speed to 0.5x
                    gameTime.changeTic(2);
                } else if (event.key.code == sf::Keyboard::Num2) {
                    // Change timeline speed to 1x (real time)
                    gameTime.changeTic(4);
                } else if (event.key.code == sf::Keyboard::Num3) {
                    // Change timeline speed to 2x
                    gameTime.changeTic(8);
                }
            }
        }

        window.clear(sf::Color::Black);

        // Calculate elapsed time based on game timeline
        float elapsedTime = gameTime.getTime() ; // Convert to seconds
        int64_t frame_delta = elapsedTime - last_time;
        last_time = elapsedTime;
        // Update moving platform and stationary platform
        {
            std::unique_lock<std::mutex> lock(mutexName);
            movingPlat.collidePlatform(800, landShape.getGlobalBounds());
            landShape.flux(-1, movingPlat.getGlobalBounds());
        }

        // Update character movement based on elapsed time trying to implement jumpibng, not working yet
        playableChar.movement(isGround, landShape.getGlobalBounds(), frame_delta);
        playableChar.movement(isGround, movingPlat.getGlobalBounds(), frame_delta);

        // Draw everything
        window.draw(landShape);
        window.draw(movingPlat);
        playableChar.setTexture(&texture);
        window.draw(playableChar);

        window.display();
    }

    // Join the platform thread before exiting
    platformThread.join();

    return 0;
}

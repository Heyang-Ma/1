// TBSGame.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <SFML/Graphics.hpp>
#include "GameController.h"



void HandleEvents(sf::RenderWindow& window, GameController* gameController)
{

    while (const std::optional event = window.pollEvent())
    {
        if (event->is<sf::Event::Closed>())
        {
            window.close();
        }
        else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
        {
            if (keyPressed->scancode == sf::Keyboard::Scancode::Escape)
                window.close();
            else
            {
                gameController->HandleKeyPress(keyPressed);
            }
        }
        else if (const auto* mouseButtonPressed = event->getIf<sf::Event::MouseButtonPressed>())
        {
            gameController->HandleMousePress(mouseButtonPressed);
            gameController->gameRunning = true;
        }
    }
}



int main()
{
    // define window
    sf::RenderWindow window(sf::VideoMode({ 1600, 1000 }), "TBS Game");
    window.setVerticalSyncEnabled(true);

    // define the game controller
    GameController gameController;

    gameController.Init();
    gameController.StartGame();


    //define the view
    sf::View gameView(sf::FloatRect({ 0, 0 }, { 1600, 1000.f }));
    gameView.setViewport(sf::FloatRect({ 0.f, 0.f }, { 1, 1.f }));

    gameView.setCenter({ 800.f, 500.f });

    sf::Clock clock;

    while (window.isOpen())
    {




        // handle all events

        HandleEvents(window, &gameController);


        // display the game section of the window
        window.clear();
        window.setView(gameView);
        gameController.Render(window);
        gameController.DisplayUI(window);


        // display the window
        window.display();


        // tick the game controller       
        sf::Time elapsed = clock.restart();
        float dt = elapsed.asSeconds();
        gameController.Update(dt);



    }
}


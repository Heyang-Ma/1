#include "GameController.h"

#include <iostream>
#include <sstream>
#include <string>
#include <chrono>
#include <thread>

using namespace std;

GameController::GameController()
{

    // obtain Math and Random singletons
    Global_Random = AIRandom_Global::getInstance();
    Global_Math = AIMath_Global::getInstance();
}

GameController::~GameController()
{

}



// ** Intialisation **

/*
* Init should be called prior to starting to game
*/

void GameController::Init()
{
    //setup display
    LoadGraphicsAssests();
    CreateGround();
    SetMapSprites();

    // initialise the AI 
    InitAI();

    //initialise the game environment
    InitGame();

}


/*
*InitGame should be called once to setup all the game components and set the game ready for the first turn
*/
void GameController::InitGame()
{
    SetupCharacters();
    SetTeamColours();
    UpdateTileStatus();
    activeChar = 0;
    turnState = TurnState::Waiting;
    statusText = "Waiting for Player";
}


/*
* InitAI initalises the AIConstructor. Must be called before game starts.
*/

void GameController::InitAI()
{
    //DefineAI will initialise all of the Actions, Considerations and Options used by the AI
    rbsAIConstructor.Init();
    rbsAIConstructor.DefineAI();

    dtAIConstructor.Init();
    dtAIConstructor.DefineAI();

    dtAIConstructorWizard.Init();
    dtAIConstructorWizard.DefineAI();

    rbsAIConstructorWizard.Init();
    rbsAIConstructorWizard.DefineAI();

}



/*
LoadGraphicsAssests will load the graphics assets from file erady for use
*/
void GameController::LoadGraphicsAssests()
{
    bool load = textureGround.loadFromFile("Assets/Ground.png");
    load = textureFighter.loadFromFile("Assets/Fighter_v3.png");
    load = textureArcher.loadFromFile("Assets/Archer_v3.png");
    load = textureWizard.loadFromFile("Assets/Wizard_v3.png");
    load = powerButton.loadFromFile("Assets/PowerButton.png");
    load = meleeButton.loadFromFile("Assets/MeleeButton.png");
    load = rangedButton.loadFromFile("Assets/RangedButton.png");
    load = moveButton.loadFromFile("Assets/MoveButton.png");
    load = highlight.loadFromFile("Assets/Highlight.png");


    powerButtonSprite = new sf::Sprite(powerButton);
    meleeButtonSprite = new sf::Sprite(meleeButton);
    rangedButtonSprite = new sf::Sprite(rangedButton);
    moveButtonSprite = new sf::Sprite(moveButton);

    activePlayerHighlight = new sf::Sprite(highlight);
    activePlayerHighlight->setColor(sf::Color::Yellow);
    activePlayerHUDHighlight = new sf::Sprite(highlight);
    activePlayerHUDHighlight->setColor(sf::Color::Yellow);

    actionHighlight = new sf::Sprite(highlight);
    actionHighlight->setColor(sf::Color::Red);
    moveHighlight = new sf::Sprite(highlight);
    moveHighlight->setColor(sf::Color::Green);

    font.openFromFile("Assets/DefaultAriel.ttf");

}




/*
* CreateCharacter will create and initialise a AIActor_Unit with the provided settings
*/
void GameController::CreateCharacter(int _team, int _spawnX, int _spawnY, AIActor_Unit::CharClass _class, string _actorID, AIActor_Unit::Controller _controller, AIConstructorBase& _constructor)
{
    AIActor_Unit* _c = new AIActor_Unit(_class);
    _c->Spawn(_spawnX, _spawnY);
    _c->Init(_actorID, _constructor);
    _c->InitChar(_controller, true, _class, _team);
    _c->SetGameContext(*this);


    chars.push_back(_c);
    if (_class == AIActor_Unit::Archer)
    {
        charSprites.push_back(sf::Sprite(textureArcher));
        hudSprites.push_back(sf::Sprite(textureArcher));
    }
    else if (_class == AIActor_Unit::Fighter)
    {
        charSprites.push_back(sf::Sprite(textureFighter));
        hudSprites.push_back(sf::Sprite(textureFighter));
    }
    else if (_class == AIActor_Unit::Wizard)
    {
        charSprites.push_back(sf::Sprite(textureWizard));
        hudSprites.push_back(sf::Sprite(textureWizard));
    }



}

/*
* SetupCharacters should be used to configure any character that appears on the map
* 

* 
*/
void GameController::SetupCharacters()
{

    //*** If you wish to change the characters that appear in the game, edit this function ***


    //team, posx, posy, class, name, conroller, AI constructor
    CreateCharacter(0, 4, 4, AIActor_Unit::CharClass::Fighter, "HumanFighter1", AIActor_Unit::Controller::Human, rbsAIConstructor);
    CreateCharacter(1, 5, 4, AIActor_Unit::CharClass::Fighter, "AIArcher1", AIActor_Unit::Controller::AI, rbsAIConstructor);
    CreateCharacter(0, 0, 4, AIActor_Unit::CharClass::Fighter, "HumanFighter2", AIActor_Unit::Controller::Human, rbsAIConstructor);
    CreateCharacter(1, 10, 4, AIActor_Unit::CharClass::Fighter, "AIFighter2", AIActor_Unit::Controller::AI, rbsAIConstructor);


}





/**
* CreateGround will set the map tile terrain types
 - currently all grass
*/
void GameController::CreateGround()
{
    for (int y = 0; y < mapHeight; y++)
    {
        for (int x = 0; x < mapWidth; x++)
        {

            terrain[x][y] = TerrainType::Ground;
            mapHighlight[x][y] = false;
        }

    }
}

/*
* SetMapSprites will create the sprites for the map terrain
*/
void GameController::SetMapSprites()
{
    for (int x = 0; x < mapWidth; x++)
    {
        for (int y = 0; y < mapHeight; y++)
        {
            if (terrain[x][y] == TerrainType::Ground)
            {
                mapArea.push_back(sf::Sprite(textureGround));
            }

        }
    }
}




/*
* SetTeamColours will set the color of the sprites based on the team ID
* Default - 0 = blue (human), 1 = red (AI)
*/
void GameController::SetTeamColours()
{
    for (int i = 0; i < chars.size(); i++)
    {
        if (chars[i]->teamID == Team0ID)
        {
            charSprites[i].setColor(sf::Color(150, 150, 255));
            hudSprites[i].setColor(sf::Color(150, 150, 255));
        }
        else
        {
            charSprites[i].setColor(sf::Color(255, 150, 150));
            hudSprites[i].setColor(sf::Color(255, 150, 150));
        }
    }
}


//** Events  **


/*
* HandleKeyPress will handle the SFML key presses
*/

void GameController::HandleKeyPress(const sf::Event::KeyPressed* _keyPress)
{

    if (!gameRunning)
        return;



}


/*
* HandleKeyPress will handle the SFML mouse presses
*/
void GameController::HandleMousePress(const sf::Event::MouseButtonPressed* _mousePress)
{

    if (!gameRunning)
        return;

    if (_mousePress->button == sf::Mouse::Button::Right)
    {


    }
    else if (_mousePress->button == sf::Mouse::Button::Left)
    {

        // based in the x,y click position and game state, check each button for actions

        sf::Vector2f _clickPos(_mousePress->position.x, _mousePress->position.y);

        if (turnState == TurnState::Waiting)
        {

            // if waiting for human turn, pressing any button will update the UI to show possible actions

            if (moveButtonSprite->getGlobalBounds().contains(_clickPos))
            {
                moveButtonSprite->setColor(sf::Color::Cyan);
                turnState = TurnState::MoveSelected;
                UpdateTurnHighlight();
            }
            if (meleeButtonSprite->getGlobalBounds().contains(_clickPos))
            {
                meleeButtonSprite->setColor(sf::Color::Cyan);
                turnState = TurnState::MeleeSelected;
                UpdateTurnHighlight();
            }
            if (rangedButtonSprite->getGlobalBounds().contains(_clickPos))
            {
                rangedButtonSprite->setColor(sf::Color::Cyan);
                turnState = TurnState::RangedSelected;
                UpdateTurnHighlight();
            }
            if (powerButtonSprite->getGlobalBounds().contains(_clickPos))
            {
                powerButtonSprite->setColor(sf::Color::Cyan);
                turnState = TurnState::PowerSelected;
                UpdateTurnHighlight();
            }



        }
        else if (turnState == TurnState::MoveSelected)
        {

            // if the player has selected Move, show moves or allow user to cancel

            if (moveButtonSprite->getGlobalBounds().contains(_clickPos))
            {
                std::cout << "Move Un Pressed" << std::endl;
                turnState = TurnState::Waiting;
                moveButtonSprite->setColor(sf::Color::White);
                ClearTurnHighlight();
            }
            else {

                int _gridX = ConvertScreenPosToGridPos(_mousePress->position.x);
                int _gridY = ConvertScreenPosToGridPos(_mousePress->position.y);

                // only highlighted grid squares are valid clicks, ignore all others
                if (mapHighlight[_gridX][_gridY])
                {
                    // perform action, then move to next turn
                    MoveActiveChar(_gridX, _gridY);
                    ProgressNextTurn();
                    moveButtonSprite->setColor(sf::Color::White);

                }
            }
        }
        else if (turnState == TurnState::MeleeSelected)
        {
            if (meleeButtonSprite->getGlobalBounds().contains(_clickPos))
            {
                meleeButtonSprite->setColor(sf::Color::White);
                turnState = TurnState::Waiting;
                ClearTurnHighlight();
            }
            else if (isHighlightedCharSelected(_clickPos))
            {
                int _gridX = ConvertScreenPosToGridPos(_mousePress->position.x);
                int _gridY = ConvertScreenPosToGridPos(_mousePress->position.y);
                meleeButtonSprite->setColor(sf::Color::White);

                // perform action, then move to next turn
                PerformAttack(GetPlayerAtGridPod(_gridX, _gridY), AIActor_Unit::AttackType::Melee);
                ProgressNextTurn();
            }
        }
        else if (turnState == TurnState::RangedSelected)
        {
            if (rangedButtonSprite->getGlobalBounds().contains(_clickPos))
            {
                rangedButtonSprite->setColor(sf::Color::White);
                turnState = TurnState::Waiting;
                ClearTurnHighlight();
            }
            else if (isHighlightedCharSelected(_clickPos))
            {

                int _gridX = ConvertScreenPosToGridPos(_mousePress->position.x);
                int _gridY = ConvertScreenPosToGridPos(_mousePress->position.y);
                rangedButtonSprite->setColor(sf::Color::White);

                // perform action, then move to next turn
                PerformAttack(GetPlayerAtGridPod(_gridX, _gridY), AIActor_Unit::AttackType::Ranged);
                ProgressNextTurn();
            }
        }
        else if (turnState == TurnState::PowerSelected)
        {


            if (powerButtonSprite->getGlobalBounds().contains(_clickPos))
            {
                powerButtonSprite->setColor(sf::Color::White);
                turnState = TurnState::Waiting;
                ClearTurnHighlight();
            }
            else if (isHighlightedCharSelected(_clickPos))
            {

                int _gridX = ConvertScreenPosToGridPos(_mousePress->position.x);
                int _gridY = ConvertScreenPosToGridPos(_mousePress->position.y);
                powerButtonSprite->setColor(sf::Color::White);

                // perform action, then move to next turn
                PerformAttack(GetPlayerAtGridPod(_gridX, _gridY), AIActor_Unit::AttackType::Power);
                chars[activeChar]->BeginCooldown(); // start the cooldown
                ProgressNextTurn();
            }
        }


    }
}




//**  Game Loop **



/*
* Update is called each frame and should only contain code desined to be called this often
*/
void GameController::Update(float dt)
{

    if (gameRunning)
    {
        if (turnState == TurnState::EndTurn) {
            NextTurn();
        }
        else if (turnState == TurnState::AITakingTurn)
        {
            AITurn();
        }
    }
}



/*
*StartGame will start the game running, allowing actions to take place
*/

void GameController::StartGame()
{
    gameRunning = true;
}


/*
*StartGame will start the game running, allowing actions to take place
*/

void GameController::EndGame()
{
    gameRunning = false;
}




// ** Character Actions ** 


/*
* MoveActiveChar will mvoe the active character to the specified grid position
*/
void GameController::MoveActiveChar(int _gridX, int _gridY)
{
    std::string s = "Action Chosen: Move to Pos ";
    s.append(std::to_string(_gridX)).append(" ").append(std::to_string(_gridY));
    UpdateStatusText(s);
    chars[activeChar]->Move(_gridX, _gridY);
}


std::string GetStringAttackType(AIActor_Unit::AttackType _type)
{
    std::vector<std::string> n{ "Melee", "Power", "Ranged" };
    return n[_type];
}


/*
* PerformAttack will cuase the active character to attack the provided target character with the specificed attack type
*/
void GameController::PerformAttack(AIActor_Unit* _target, AIActor_Unit::AttackType _type)
{

    if (_target == nullptr)
    {
        cout << "WARNING: Attempting to attack null target." << std::endl;
        return;
    }

    std::string s = "Action Chosen:  Attack. Type = ";
    s.append(GetStringAttackType(_type));
    UpdateStatusText(s);

    _target->UpdateHealth(-chars[activeChar]->GetDamageForAttackType(_type));
}


/*
* AttackCharAtPos will trigger an attack to happen on the characer at the provided position
*/
void GameController::AttackCharAtPos(AIMath_Global::Vector2f _pos, AIActor_Unit::AttackType _attackType)
{
    PerformAttack(GetPlayerAtGridPod(_pos.x, _pos.y), _attackType);
}






// ** Environment Checks **



/*
* getNextActiveChar will return the index of the character next in line to take their turn
*/
int GameController::getNextActiveChar()
{
    int nextChar = activeChar;

    for (int i = 0; i < chars.size(); i++)
    {
        nextChar = (nextChar + 1) % chars.size();

        if (chars[nextChar]->isActive)
        {
            return nextChar;
        }
    }
}


/*
* isHighlightedCharSelected will return true if the character at the provoded click pos is true, otherwise false
*/
bool GameController::isHighlightedCharSelected(sf::Vector2f _clickPos)
{
    int _gridX = ConvertScreenPosToGridPos(_clickPos.x);
    int _gridY = ConvertScreenPosToGridPos(_clickPos.y);

    return mapHighlight[_gridX][_gridY];

}

/*
* GetPlayerAtGridPod will return a pointer to the character as position specified. If there is no character at the position, nullptr will be returned
*/
AIActor_Unit* GameController::GetPlayerAtGridPod(int _x, int _y)
{

    for (int i = 0; i < chars.size(); i++)
    {
        if (chars[i]->gridPosX == _x && chars[i]->gridPosY == _y)
        {
            return chars[i];
        }
    }

    return nullptr;
}


/*
* GetOppTeam will retrn the opposing team ID to the one provided
*/
int GameController::GetOppTeam(int _teamID)
{
    if (_teamID == Team0ID)
    {
        return Team1ID;
    }
    else
    {
        return Team0ID;
    }
}



/*
* CalcClosestChar will return the index of the closest chracter of the provided team to the character at the provided index
*/
int GameController::CalcClosestChar(int _teamID, int _fromChar)
{

    int _acX = chars[_fromChar]->gridPosX;
    int _acY = chars[_fromChar]->gridPosY;

    int _closestChar = 0;
    float _clostestDist = 1000;

    for (int i = 0; i < chars.size(); i++)
    {
        if (i != _fromChar && chars[i]->teamID == _teamID && chars[i]->isActive)
        {
            float _dist = Global_Math->CalcManhattanDistance(AIMath_Global::Vector2f(_acX, _acY), AIMath_Global::Vector2f(chars[i]->gridPosX, chars[i]->gridPosY));

            if (_dist < _clostestDist)
            {
                _clostestDist = _dist;
                _closestChar = i;
            }
        }
    }

    return _closestChar;
}


/*
* isTileOccupied will return true if the provided tile is occupied, otherwise false
*/
bool GameController::isTileOccupied(int _x, int _y)
{
    return tileOccupied[_x][_y];
}


// ** Rendering **




void GameController::Render(sf::RenderWindow& window)
{
    DrawMap(window);
    DrawCharacters(window);
    DrawTurnHighlights(window);
}


/*
* DisplayUI will display the button input panel, and the lower HUD
*/
void GameController::DisplayUI(sf::RenderWindow& window)
{

    DisplayCharHUD(window);
    DisplayButtonPanel(window);


}



/*
DisplayCharHUD will draw the lower HUD showing the turn order and unit health
*/
void GameController::DisplayCharHUD(sf::RenderWindow& window)
{

    int _xSpace = 256;
    int _xOffset = 32;


    int _a = activeChar;
    int _displayPos = 0;
    int numChars = chars.size();

    for (int i = 0; i < numChars; i++)
    {

        if (chars[_a]->isActive)
        {

            sf::Text text(font);
            ostringstream os;
            os << "Health " << chars[_a]->GetBBValue("Health") << std::endl;
            string s = os.str();

            // set the string to display
            text.setString(s);

            // set the character size
            text.setCharacterSize(28); // in pixels, not points!

            // set the color
            text.setFillColor(sf::Color::White);
            float _xPos = (_displayPos * _xSpace) + _xOffset;
            text.setPosition({ _xPos, 950.f });

            window.draw(text);

            hudSprites[_a].setPosition({ _xPos , 750.f });
            hudSprites[_a].setScale({ 2,2 });
            window.draw(hudSprites[_a]);

            _displayPos++;

        }

        _a++;
        _a = _a % numChars;

    }



    // display the highlight for the active player
    activePlayerHighlight->setPosition({ ConvertGridPosToSreenPos(chars[activeChar]->gridPosX), ConvertGridPosToSreenPos(chars[activeChar]->gridPosY) });
    window.draw(*activePlayerHighlight);

    activePlayerHUDHighlight->setPosition({ 0, 750 });
    activePlayerHUDHighlight->setScale({ 3,3 });
    window.draw(*activePlayerHUDHighlight);


    // display the status text
    sf::Text text(font); 


    ostringstream os;


    os << statusText << std::endl;
    string s = os.str();

    // set the string to display
    text.setString(s);

    // set the character size
    text.setCharacterSize(36); // in pixels, not points!

    // set the color
    text.setFillColor(sf::Color::White);
    text.setPosition({ 10.f,675.f });

    window.draw(text);



}


/*
* DisplayButtonPanel will show hte button panel on the right side of the scree that the user used to take their turn
*/
void GameController::DisplayButtonPanel(sf::RenderWindow& window)
{

    sf::Text text(font); // a font is required to make a text object


    ostringstream os;


    os << "TBS Game";
    string s = os.str();

    // set the string to display
    text.setString(s);

    // set the character size
    text.setCharacterSize(28); // in pixels, not points!

    // set the color
    text.setFillColor(sf::Color::White);
    text.setPosition({ 1400,0.f });

    window.draw(text);

    moveButtonSprite->setPosition(sf::Vector2f(1400, 200));
    window.draw(*moveButtonSprite);

    rangedButtonSprite->setPosition(sf::Vector2f(1400, 400));
    window.draw(*rangedButtonSprite);

    meleeButtonSprite->setPosition(sf::Vector2f(1400, 600));
    window.draw(*meleeButtonSprite);

    // onnly show power attack if the rule is enabled 
    if (chars[activeChar]->powerAttackEnabled && powerAttackRule)
    {
        powerButtonSprite->setPosition(sf::Vector2f(1400, 800));
        window.draw(*powerButtonSprite);
    }

}


/*
* DrawTurnHighlights will show the UI grid highlights when the player is taking their tirn
*/
void GameController::DrawTurnHighlights(sf::RenderWindow& window)
{


    for (int x = 0; x < mapWidth; x++)
    {
        for (int y = 0; y < mapHeight; y++)
        {
            if (mapHighlight[x][y])
            {
                if (turnState == TurnState::MoveSelected)
                {
                    moveHighlight->setPosition({ ConvertGridPosToSreenPos(x), ConvertGridPosToSreenPos(y) });
                    window.draw(*moveHighlight);
                }
                else
                {
                    actionHighlight->setPosition({ ConvertGridPosToSreenPos(x), ConvertGridPosToSreenPos(y) });
                    window.draw(*actionHighlight);
                }


            }

        }
    }

}



/*
* DrawMap will render all the map tiles
*/
void GameController::DrawMap(sf::RenderWindow& window)
{
    for (int x = 0; x < mapWidth; x++)
    {
        for (int y = 0; y < mapHeight; y++)
        {
            int _index = (x * mapHeight) + y;

            mapArea[_index].setPosition({ x * mapSectionXY, y * mapSectionXY });
            window.draw(mapArea[_index]);
        }
    }
}




/*
* DrawCharacters will draw each of the characters on the map at their current position
*/
void GameController::DrawCharacters(sf::RenderWindow& window)
{
    for (int i = 0; i < chars.size(); i++)
    {
        if (chars[i]->isActive)
        {
            charSprites[i].setPosition({ ConvertGridPosToSreenPos(chars[i]->gridPosX),ConvertGridPosToSreenPos(chars[i]->gridPosY) });
            window.draw(charSprites[i]);
        }

    }
}



// ** Game Status **



/*
* UpdateStatusText will set the text that appears below the game play area, as well as log to the console output
*/
void GameController::UpdateStatusText(std::string _newText)
{
    cout << "TURN LOG: " << _newText << std::endl;
    statusText = _newText;
}


/*
/ UpdateTileStatus will refresh the tileOccupied variable based on the current character positions
*/
void GameController::UpdateTileStatus()
{
    for (int x = 0; x < mapWidth; x++)
    {
        for (int y = 0; y < mapHeight; y++)
        {
            tileOccupied[x][y] = false;
        }
    }

    for (int i=0;i<chars.size();i++)
    {
        if (chars[i]->isActive)
        {
           tileOccupied[chars[i]->gridPosX][chars[i]->gridPosY] = true;
        }
        
    }

}

// ** Game Rules and Logic **



/*
* CheckWinState will check if one team has won by defeating all opposing characers. If so, the game ends. 
*/
void GameController::CheckWinState()
{
    int _t0Count = 0;
    int _t1Count = 0;

    for (int i = 0; i < chars.size(); i++)
    {
        if (chars[i]->isActive)
        {
            if (chars[i]->teamID == Team0ID)
            {
                _t0Count++;
            }
            else if (chars[i]->teamID == Team1ID)
            {
                _t1Count++;
            }
        }
    }

    if (_t0Count == 0)
    {
        cout << "AI Wins" << endl;
        UpdateStatusText("AI Wins");
        EndGame();
    }
    else if (_t1Count == 0)
    {
        cout << "Human Wins" << endl;
        UpdateStatusText("Human Wins");
        EndGame();
    }

}


/*
* RemoveDefeatedChars will remove any defeated characters from the game by setting them as inactive
* the character will still exist, but will not take turns or influence the match
*/

void GameController::RemoveDefeatedChars()
{
    int currentCharCount = chars.size();
    for (int i = currentCharCount - 1; i >= 0; i--)
    {
        if (chars[i]->isActive && chars[i]->GetBBValue("Health") == 0)
        {
            chars[i]->isActive = false;
            cout << "GAME LOG: Player " << i << " deafeated " << std::endl;
        }
    }
}



/*
RecoverHealth should be called at the end of each round , causing each character to recover health
*/
void GameController::RecoverHealth()
{
    for (int i = 0; i < chars.size(); i++)
    {
        if (chars[i]->isActive)
        {
            chars[i]->RecoverHealth();
        }

    }
}


/*
ProgressNextTurn should be called once a character's action (AI or Human) has been chosen and fired. 
*/
void GameController::ProgressNextTurn()
{

    ClearTurnHighlight();
    UpdateTileStatus();

    clock.restart();

    turnState = TurnState::EndTurn;



}


/*
* NextRound should be called once each character has taken their turn and the round is complete
*/
void GameController::NextRound()
{
    UpdateCharCooldown();
    RecoverHealth();
}



/*
NextTurn should be called after a character has taken their turn to progress the game to the next turn
*/
void GameController::NextTurn()
{

    // implement a small delay to allow all actions to be seen
    if (clock.getElapsedTime().asSeconds() < turnDelay)
        return;

    UpdateStatusText("Processesing...");

    // check for defeated characters
    RemoveDefeatedChars();

    // update the tracking data
    UpdateCharPositionData();

    // check if game is over
    CheckWinState();



    if (gameRunning)
    {
        // find the next active character
        int nextActiveChar = getNextActiveChar();

        // if the index of the next character is lower, it's a new round
        if (nextActiveChar < activeChar)
        {
            NextRound();
        }

        activeChar = nextActiveChar;


        // if the neext charteracter is on a cooldown, skip and proceed to next character
        if (chars[activeChar]->GetBBValue("Cooldown") > 0)
        {
            UpdateStatusText("Cooldown Active, moving to next Char...");
            ProgressNextTurn();

        }
        else
        {

            if (chars[activeChar]->charController == AIActor_Unit::AI)
            {
                UpdateStatusText("AI Thinking...");
                clock.restart();
                turnState = TurnState::AITakingTurn;
            }
            else
            {
                UpdateStatusText("Waiting for Player...");
                turnState = TurnState::Waiting;
            }

        }

    }

  

}


/*
* AITurn will tick the active character, causing them to select and execude an action for their turn
*/
void GameController::AITurn()
{

    // implement a slight delay, so the player sees the AI thinking and then acting
    if (clock.getElapsedTime().asSeconds() < turnDelay)
        return;

    chars[activeChar]->Update(1.f);
    //UpdateStatusText("AI Turn Complete");

    ProgressNextTurn();

}


/*
* UpdateCharPositionData will update the data on each AIActor's blackboard 
*/
void GameController::UpdateCharPositionData()
{

    // TASK TODO - if you want to add additional data to the AIActor's blackboard, edit the code below

    for (int i = 0; i < chars.size(); i++)
    {

        // get nearest enemy
        int _nearestEnemyIndex = CalcClosestChar(GetOppTeam(chars[i]->teamID), i);

        AIMath_Global::Vector2f _nearestPos(chars[_nearestEnemyIndex]->gridPosX, chars[_nearestEnemyIndex]->gridPosY);
        // store distance
        int _dist = Global_Math->CalcManhattanDistance(AIMath_Global::Vector2f(chars[i]->gridPosX, chars[i]->gridPosY),
            _nearestPos);

        chars[i]->EditBBValue("NearestEnemyDist", _dist);

        chars[i]->EditBBValueVec("NearestEnemyPos", _nearestPos);

        // store class
        chars[i]->AddBBValue("NearestEnemyClass", chars[i]->charClass);

        // store health
        chars[i]->EditBBValue("NearestEnemyHealth", chars[_nearestEnemyIndex]->GetBBValue("Health"));
    }
}


/*
UpdateCharCooldown will reduce each character's cooldown status should they have one
*/
void GameController::UpdateCharCooldown()
{
    for (int i = 0; i < chars.size(); i++)
    {
        if (chars[i]->GetBBValue("Cooldown") > 0)
        {
            chars[i]->TickCooldown();
        }
    }
}


/*
* UpdateTurnHighlight will update the tracking variables for the UI highlights based on the UI input status
*/
void GameController::UpdateTurnHighlight()
{
    ClearTurnHighlight();


    int _acX = chars[activeChar]->gridPosX;
    int _acY = chars[activeChar]->gridPosY;

    if (turnState == TurnState::MoveSelected)
    {

        // if move selected, highlight all positions within movement range 

        int _acM = chars[activeChar]->moveSpeed;

        for (int x = 0; x < mapWidth; x++)
        {
            for (int y = 0; y < mapHeight; y++)
            {
                //int _md = abs(x - _acX) + abs(y - _acY);
                int _mDist = Global_Math->CalcManhattanDistance(AIMath_Global::Vector2f(x, y),
                    AIMath_Global::Vector2f(_acX, _acY));

                // only highlight tiles if they are unoccupied
                if(_mDist <= _acM && !tileOccupied[x][y])
                {
                    mapHighlight[x][y] = true;
                }
            }
        }
    }
    else if (turnState == TurnState::MeleeSelected || turnState == TurnState::PowerSelected)
    {

        // if melee or power attack selected, higlight any enemies within 1 square

        for (int i = 0; i < chars.size(); i++)
        {
            int _ecX = chars[i]->gridPosX;
            int _ecY = chars[i]->gridPosY;

            int _mDist = Global_Math->CalcManhattanDistance(AIMath_Global::Vector2f(_acX, _acY),
                AIMath_Global::Vector2f(_ecX, _ecY));

            if (i != activeChar && chars[i]->teamID != chars[activeChar]->teamID && chars[i]->isActive && _mDist == 1)
            {       
                    mapHighlight[_ecX][_ecY] = true;

            }
        }

    }
    else
    {

        // if ranged attack is slected, highlight any enemies more than 1 square away

        for (int i = 0; i < chars.size(); i++)
        {
            int _ecX = chars[i]->gridPosX;
            int _ecY = chars[i]->gridPosY;

            int _mDist = Global_Math->CalcManhattanDistance(AIMath_Global::Vector2f(_acX, _acY),
                AIMath_Global::Vector2f(_ecX, _ecY));

            if (i != activeChar && chars[i]->teamID != chars[activeChar]->teamID && chars[i]->isActive && _mDist>1)
            {
          
                mapHighlight[_ecX][_ecY] = true;
            }
        }
    }

}

/*
* ClearTurnHighlight will clear the tracking variables for the UI highlights 
*/
void GameController::ClearTurnHighlight()
{
    for (int x = 0; x < mapWidth; x++)
    {
        for (int y = 0; y < mapHeight; y++)
        {
            mapHighlight[x][y] = false;
        }
    }
}







// ** Conversion / Math **




int GameController::ConvertScreenPosToGridPos(float screenPos)
{

    return (screenPos / mapSectionXY);
}

float GameController::ConvertGridPosToSreenPos(int gridPos)
{

    return (gridPos * mapSectionXY);
}








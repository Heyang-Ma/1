#pragma once


#ifndef AI_GAME_CONTROLLER_H
#define AI_GAME_CONTROLLER_H


#include <SFML/Graphics.hpp>
#include "AIActor_Unit.h"
#include "AIConstructor_RBS.h"
#include "AIConstructor_DT.h"
#include "AIConstructor_DTWizard.h"
#include "AIConstructor_RBSWizard.h"



class GameController
{
public:

	static const int mapWidth = 20;
	static const int mapHeight = 10;

	static const int Team0ID = 0;
	static const int Team1ID = 1;


	enum TerrainType { Ground };
	enum TurnState { Waiting, AITakingTurn, MoveSelected, MeleeSelected, PowerSelected, RangedSelected, EndTurn };


	GameController();
	~GameController();



	//Intialisation
	void Init();


	//Events and Game Loop

	void Update(float dt);
	void HandleKeyPress(const sf::Event::KeyPressed* _keyPress);
	void HandleMousePress(const sf::Event::MouseButtonPressed* _mousePress);
	


	// Rendering
	void DisplayUI(sf::RenderWindow& window);
	void Render(sf::RenderWindow& window);
	

	// Game Status
	void StartGame();
	void EndGame();
	bool gameRunning;
	int activeChar;


	// Character Actions
	void AttackCharAtPos(AIMath_Global::Vector2f _pos, AIActor_Unit::AttackType _attackType);
	void MoveActiveChar(int _x, int _y);

	// Environment Checks
	bool isTileOccupied(int _x, int _y);


private:


	const float mapSectionXY = 64.0f;

	// Create the AI constructors
	AIConstructor_RBS rbsAIConstructor;
	AIConstructor_DT dtAIConstructor;
	AIConstructor_DTWizard dtAIConstructorWizard;
	AIConstructor_RBSWizard rbsAIConstructorWizard;


	//Intialisation
	void InitGame();
	void InitAI();
	void LoadGraphicsAssests();
	void SetupCharacters();
	void CreateGround();
	void SetMapSprites();
	void CreateCharacter(int _team, int _spawnX, int _spawnY, AIActor_Unit::CharClass _class, std::string _actorID, AIActor_Unit::Controller _controller, AIConstructorBase& _constructor);



	// Rendering
	void DisplayButtonPanel(sf::RenderWindow& window);
	void DisplayCharHUD(sf::RenderWindow& window);
	void DrawCharacters(sf::RenderWindow& window);
	void DrawMap(sf::RenderWindow& window);
	void DrawTurnHighlights(sf::RenderWindow& window);

	// Math/Conversion

	int ConvertScreenPosToGridPos(float screenPos);
	float ConvertGridPosToSreenPos(int gridPos);

	// Game Status, Rules & Logic

	void UpdateTurnHighlight();
	void ClearTurnHighlight();
	void SetTeamColours();

	bool isHighlightedCharSelected(sf::Vector2f _clickPos);
	void ProgressNextTurn();
	void NextTurn();
	void NextRound();

	void RemoveDefeatedChars();
	void CheckWinState();
	void UpdateCharCooldown();
	void RecoverHealth();

	void AITurn();

	void UpdateCharPositionData();
	void UpdateTileStatus();
	void UpdateStatusText(std::string _newText);
	int getNextActiveChar();

	// Player Actions

	void PerformAttack(AIActor_Unit* _target, AIActor_Unit::AttackType _type);


	// calculations and checks
	int CalcClosestChar(int _teamID, int _fromChar);
	int GetOppTeam(int _teamID);
	AIActor_Unit* GetPlayerAtGridPod(int _x, int _y);


	

	// Status & Tracking
	TurnState turnState;
	bool powerAttackRule = false;
	bool mapHighlight[mapWidth][mapHeight];
	bool tileOccupied[mapWidth][mapHeight];

	sf::Clock clock;
	const float turnDelay = 2.0f;

	std::string statusText;


	//Graphics

	sf::Font font;

	TerrainType terrain[mapWidth][mapHeight];

	sf::Texture textureGround;
	sf::Texture textureFighter;
	sf::Texture textureWizard;
	sf::Texture textureArcher;

	std::vector< sf::Sprite > mapArea;


	std::vector<sf::Sprite> charSprites;
	std::vector<sf::Sprite> hudSprites;

	std::vector<AIActor_Unit*> chars;

	sf::Sprite* moveButtonSprite;
	sf::Sprite* meleeButtonSprite;
	sf::Sprite* powerButtonSprite;
	sf::Sprite* rangedButtonSprite;

	sf::Sprite* activePlayerHighlight;
	sf::Sprite* activePlayerHUDHighlight;

	sf::Sprite* actionHighlight;
	sf::Sprite* moveHighlight;


	sf::Texture moveButton;
	sf::Texture meleeButton;
	sf::Texture powerButton;
	sf::Texture rangedButton;

	sf::Texture highlight;

	
	//Global Access
	AIRandom_Global* Global_Random;
	AIMath_Global*  Global_Math;



};


#endif // !AI_GAME_CONTROLLER_H
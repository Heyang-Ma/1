#include "AIActor_Unit.h"
#include "AIBrain_TBS.h"
#include "GameController.h"
#include <iostream>

AIActor_Unit::AIActor_Unit()
{
	maxHealth = 0;
	meleeDamage = 0;
	rangedDamage = 0;
	moveSpeed = 0;

	gridPosX = 0;
	gridPosY = 0;

}


AIActor_Unit::AIActor_Unit(CharClass _class)
{
	maxHealth = classMaxHealth[_class];
	meleeDamage = classMeleeDamage[_class];;
	rangedDamage = classRangedDamage[_class];;
	moveSpeed = classMoveSpeed[_class];;


	gridPosX = 0;
	gridPosY = 0;
}

AIActor_Unit::AIActor_Unit(int _maxHealth, int _meleeDamage, int _rangedDamage, int _moveSpeed)
{
	maxHealth = _maxHealth;
	meleeDamage = _meleeDamage;
	rangedDamage = _rangedDamage;
	moveSpeed = _moveSpeed;

	gridPosX = 0;
	gridPosY = 0;

}

AIActor_Unit::~AIActor_Unit()
{

}



// **  AIActorBase overrides **






bool AIActor_Unit::Init(std::string _id, AIConstructorBase& _constructor)
{
	actorID = _id;

	// create new brain
	brain = new AIBrain_TBS();

	brain->Init(_constructor, *this);



	return true;
}



// **  Initialisation **



void AIActor_Unit::Spawn(int _x, int _y)
{
	gridPosX = _x;
	gridPosY = _y;
}


void AIActor_Unit::InitChar(Controller _controller, bool _isActive, CharClass _class, int _team)
{
	charController = _controller;
	isActive = _isActive;
	charClass = _class;
	teamID = _team;


	if (charClass == CharClass::Fighter)
	{
		powerAttackEnabled = true;
	}
	else
	{
		powerAttackEnabled = false;
	}

	AddBBValue("Health", maxHealth);
	AddBBValue("Cooldown", 0);
}




/*
* SetGameContext will set the game controller context
*
 - AI decision making data should be on the blackboard, not accessed via the game controller

*/
void AIActor_Unit::SetGameContext(GameController& _gameController) {

	gameController = &_gameController;
}




//  ** Game Loop **


/*
* Update will cause the AI Actor to use the brain to make a decision
*/
void AIActor_Unit::Update(float _dt)
{
	brain->Update();
}




// ** AI Actions **



/*
MoveAwayFromClosestEnemy will move the character away from the nearest known enemy
*/
void AIActor_Unit::MoveAwayFromClosestEnemy()
{

	int _acX = gridPosX;
	int _acY = gridPosY;
	int _acM = moveSpeed;

	AIMath_Global::Vector2f _nearestPos = GetBBValueVec("NearestEnemyPos");
	std::cout << "Nearest Enemy is at pos " << _nearestPos.x << " " << _nearestPos.y << std::endl;

	int _tgX = _nearestPos.x;
	int _tgY = _nearestPos.y;

	sf::Vector2i bestPos;
	int bestDist = 0;

	for (int x = 0; x < gameController->mapWidth; x++)
	{
		for (int y = 0; y < gameController->mapHeight; y++)
		{

			if (!gameController->isTileOccupied(x, y))
			{
			
				//grid space is in range and on map, check distance
				int _mDist = abs(x - _tgX) + abs(y - _tgY);
				int _mDistPl = abs(x - _acX) + abs(y - _acY);

				if (_mDistPl <= _acM && _mDist > bestDist)
				{
					bestDist = _mDist;
					bestPos.x = x;
					bestPos.y = y;
				}
			}

		}
	}

	gameController->MoveActiveChar(bestPos.x, bestPos.y);
	std::cout << "ACTION: MoveAwayFromClosestEnemy" << std::endl;
}


/*
* MoveToClosestEnemy will move the characters towards the nearest enemy
*/
void AIActor_Unit::MoveToClosestEnemy()
{
	int _acX = gridPosX;
	int _acY = gridPosY;
	int _acM = moveSpeed;

	AIMath_Global::Vector2f _nearestPos = GetBBValueVec("NearestEnemyPos");

	int _tgX = _nearestPos.x;
	int _tgY = _nearestPos.y;

	sf::Vector2i bestPos;
	int bestDist = 1000;

	for (int x = 0; x < gameController->mapWidth; x++)
	{
		for (int y = 0; y < gameController->mapHeight; y++)
		{
			if (!gameController->isTileOccupied(x, y))
			{


				int _mDist = abs(x - _tgX) + abs(y - _tgY);
				int _mDistPl = abs(x - _acX) + abs(y - _acY);

				if (_mDistPl <= _acM && _mDist < bestDist)
				{

					bestDist = _mDist;
					bestPos.x = x;
					bestPos.y = y;
				}

			}

		}
	}

	gameController->MoveActiveChar(bestPos.x, bestPos.y);

	std::cout << "ACTION: MoveToClosestEnemy" << std::endl;
}


/*
* RangedAttackNearest will attack the nearest enemy with a Ranged atack
*/
void AIActor_Unit::RangedAttackNearest()
{
	AIMath_Global::Vector2f _nearestPos = GetBBValueVec("NearestEnemyPos");
	std::cout << "ACTION: RangedAttackNearest @ " << _nearestPos.x << " " << _nearestPos.y << std::endl;

	gameController->AttackCharAtPos(_nearestPos, AttackType::Ranged);



}


/*
* MeleeAttackNearest will attack the nearest enemy with a Melee atack
*/
void AIActor_Unit::MeleeAttackNearest()
{
	AIMath_Global::Vector2f _nearestPos = GetBBValueVec("NearestEnemyPos");
	std::cout << "ACTION: MeleeAttackNearest @ " << _nearestPos.x << " " << _nearestPos.y << std::endl;

	gameController->AttackCharAtPos(_nearestPos, AttackType::Melee);
}


/*
* 
* 	//# TODO - Example additional Action Functions
*/

/*
void AIActor_Unit::MoveAwayFromAllCharacters()
{


	// create grid of map size
	// set value for each grid point to the distance of the closest enemy character to that position
	// select the grid point with highest value that is within move range

}
/*




// ** Game Rules and Logic **


/*
* unsed, but can be used for initialisation
*/
void AIActor_Unit::PreTransition()
{

}

/*
RecoverHealth will cuase the chracter to incease their health level by the defined amount
*/
void AIActor_Unit::RecoverHealth()
{
	UpdateHealth(healthRecovery);
}





void AIActor_Unit::Move(int _x, int _y)
{
	gridPosX = _x;
	gridPosY = _y;
}


/*
* UpdateHealth will update the health value on the blackboard by the specified amount
*/
void AIActor_Unit::UpdateHealth(int _change)
{

	int _curHealth = GetBBValue("Health");

	int _newHealth = _curHealth +_change;

	if (_newHealth >= maxHealth)
		_newHealth = maxHealth;

	if (_newHealth < 0)
		_newHealth = 0;

	EditBBValue("Health", _newHealth);
}


void AIActor_Unit::BeginCooldown()
{
	EditBBValue("Cooldown", cooldownTime);


}


void AIActor_Unit::TickCooldown()
{
	int _curCooldown = GetBBValue("Cooldown");
	int _newCooldown = _curCooldown - 1;

	if (_newCooldown < 0)
		_newCooldown = 0;

	EditBBValue("Cooldown", _newCooldown);
}


/*
* GetDamageForAttackType will return the damage this character will do with the specified attack
*/
int AIActor_Unit::GetDamageForAttackType(AttackType _type)
{
	if (_type == AttackType::Melee)
	{
		return meleeDamage;
	}
	else if (_type == AttackType::Ranged)
	{
		return rangedDamage;
	}
	else if (_type == AttackType::Power)
	{
		if (meleeDamage >= rangedDamage)
			return meleeDamage * 2;
		else
			return rangedDamage * 2;
	}
}
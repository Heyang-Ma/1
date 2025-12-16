#pragma once

#ifndef AI_ACTOR_UNIT_H
#define AI_ACTOR_UNIT_H

#include "../../AI_Framework/framework.h"

class GameController;


class AIActor_Unit :
    public AIActorBase
{
public:
	enum CharClass { Fighter=0, Archer, Wizard };
	enum Controller { Human, AI };
	enum AttackType { Melee=0, Power, Ranged };

	int classMaxHealth[3] = {20, 15, 10};
	int classMeleeDamage[3] = { 6, 3, 2 };
	int classRangedDamage[3] = { 3, 6, 9 };
	int classMoveSpeed[3] = { 3, 4,  2};


	AIActor_Unit();
	AIActor_Unit(CharClass _class);
	AIActor_Unit(int _maxHealth, int _meleeDamage, int _rangedDamage, int _moveSpeed);

	~AIActor_Unit();

	// AIActorBase overrides
	bool Init(std::string _id, AIConstructorBase& _constructor) override;
	void SetGameContext(GameController& _gameController) ;
	void Update(float _dt) override;
	void PreTransition() override;//State Transition Functions

	void Spawn(int _x, int _y);
	void InitChar(Controller _controller, bool _isActive, CharClass _class, int _team);
	void BeginCooldown();
	void TickCooldown();
	void RecoverHealth();

	// actions

	void MoveAwayFromClosestEnemy();
	void MoveToClosestEnemy();
	void RangedAttackNearest();
	void MeleeAttackNearest();




	const int cooldownTime = 2;
	const static int healthRecovery = 1;

	int gridPosX;
	int gridPosY;

	//int health;

	bool isActive;

	int maxHealth;
	int meleeDamage;
	int rangedDamage;
	int moveSpeed;

	//int cooldown;
	bool powerAttackEnabled;

	Controller charController;
	int teamID;
	CharClass charClass;

	void Move(int _x, int _y);
	void UpdateHealth(int _change);
	int GetDamageForAttackType(AttackType _type);


	GameController* gameController;


	/*std::string enumToString(CharClass charClass) { return classToString[charClass]; };
	

	std::map<CharClass, std::string> classToString = { { Fighter, "Fighter" },
								 { Archer, "Archer" },
								 { Wizard, "Wizard" } };*/
};


#endif //!AI_ACTOR_UNIT_H
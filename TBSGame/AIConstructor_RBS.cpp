#include "AIConstructor_RBS.h"
#include "AIActor_Unit.h"

bool AIConstructor_RBS::Init() {

	method = AIConstructorBase::Method::RBS;
	return true;
}


/*
DefineActions creates all the possible actions that the AI can perform
*/
void AIConstructor_RBS::DefineActions()
{

	//Define the function for close range Melee Attack
	auto attackMeleeFunction = [](AIBrainBlackboardBase& bb) -> ActionStatus {
		AIActor_Unit* actor = static_cast<AIActor_Unit*>(bb.GetActorContext());

		// Call behaviour functions here
		actor->MeleeAttackNearest();


		return ActionStatus::ACTION_SUCCESS;
		};


	// Link the function to an identifier
	AddActionByName("ActionAttackMelee", attackMeleeFunction);


	//Define the function for Ranged Attack
	auto attackRangedFunction = [](AIBrainBlackboardBase& bb) -> ActionStatus {
		AIActor_Unit* actor = static_cast<AIActor_Unit*>(bb.GetActorContext());

		// Call behaviour functions here
		actor->RangedAttackNearest();


		return ActionStatus::ACTION_SUCCESS;
		};


	// Link the function to an identifier
	AddActionByName("ActionAttackRanged", attackRangedFunction);



	// TASK TODO - add any more Actions you need below




}


/*
	DefineConsiderations creates all Considerations that will be linked to Options
// Options will only be selected if the Condition is true
*/
void AIConstructor_RBS::DefineConsiderations()
{

	// Consideration - Is there an enemy within 1 tile?
	auto cEnemyNear = [](AIBrainBlackboardBase& bb) -> bool {

		bool enemyNear = (bb.GetValue("NearestEnemyDist") ==1);
		return(enemyNear);

		};


	AddConsiderationByName("ConsiderationEnemyNear", cEnemyNear);


	// TASK TODO - add more Considerations below


}



/*
* DefineOptions links the Options with Considerations and Actions
*/
void AIConstructor_RBS::DefineOptions()
{
	// TASK TODO - add any additional Options you need to create, linking them with an Action
	AddOptionByName("OptionAttackRanged", "ActionAttackRanged", 3);
	AddOptionByName("OptionAttackMelee", "ActionAttackMelee", 2);



	// TASK TODO - add Considerations to Options by calling 'AddOptionConsideration()'
	// Options will only be selected if the Condition is true





}












// ** Helper Funcitons **
// these link to the AI Framework and should not be edited




void AIConstructor_RBS::AddActionByName(std::string _actionName, std::function<ActionStatus(AIBrainBlackboardBase&)> _function)
{

	//Define an action, and init with the function which will be called when the action is selected
	AIAction* _action = new AIAction();
	_action->Init(_actionName, _function);
	actions.insert({ _actionName, _action });
}

void AIConstructor_RBS::AddConsiderationByName(std::string _actionName, std::function<bool(AIBrainBlackboardBase&)> _function)
{

	//Define an action, and init with the function which will be called when the action is selected
	AIConsideration* _consideration = new AIConsideration();
	_consideration->Init(_actionName, _function);
	considerations.insert({ _actionName, _consideration });
}

void AIConstructor_RBS::AddOptionByName(std::string _optionName, std::string _actionName, int _priority)
{
	// create an option for action
	AIOption* _option = new AIOption();
	_option->Init(_optionName, actions[_actionName], _priority);
	options.insert({ _optionName, _option });
}

void AIConstructor_RBS::AddOptionConsideration(std::string _optionName, std::string _considerationName)
{
	// create an option for action


	options[_optionName]->AddOptionConsideration(considerations[_considerationName]);
}



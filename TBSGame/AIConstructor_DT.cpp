#include "AIConstructor_DT.h"
#include "AIActor_Unit.h"
#include "DT_Reasoner.h"


bool AIConstructor_DT::Init() {

	method = AIConstructorBase::Method::DT;
	return true;
}


/*
DefineActions creates all the possible actions that the AI can perform
*/
void AIConstructor_DT::DefineActions()
{
	// TASK TODO
	// Add all Actions you will need



}

/*
   DefineConsiderations creates all Considerations that will be linked to Options
// Options will only be selected if the Condition is true
*/
void AIConstructor_DT::DefineConsiderations()
{
	// TASK TODO
	// Add all Considerations you will need


}


/*
* DefineOptions links the Options with Considerations and Actions
*/
void AIConstructor_DT::DefineOptions()
{

	// TASK TODO
	// Add Leaf Node Actions using AddOptionByName()
	// Add Decision Node Options (Sub Reasoner) using AddSubReasonerOptionByName()







	// TASK TODO
	// Add Considerations to Options using AddOptionConsideration()









	// TASK TODO
	// Create Tree Structure by adding Options to the Root and any Sub Reasoner Option
	// Start with the Root - e.g. 	AddOptionsToSubReasoner("Root", "OptionMelee");
	// You will not need to create the Root, it is already created
	// Add Options to any SubReasoner Option - e.g. 	AddOptionsToSubReasoner("OptionMoveEval", "OptionMoveToEnemy");
	



}








// ** Helper Funcitons **
// these link to the AI Framework and should not be edited 


void AIConstructor_DT::AddActionByName(std::string _actionName, std::function<ActionStatus(AIBrainBlackboardBase&)> _function)
{

	//Define an action, and init with the function which will be called when the action is selected
	AIAction* _action = new AIAction();
	_action->Init(_actionName, _function);
	actions.insert({ _actionName, _action });
}

void AIConstructor_DT::AddConsiderationByName(std::string _actionName, std::function<bool(AIBrainBlackboardBase&)> _function)
{

	//Define an action, and init with the function which will be called when the action is selected
	AIConsideration* _consideration = new AIConsideration();
	_consideration->Init(_actionName, _function);
	considerations.insert({ _actionName, _consideration });
}

void AIConstructor_DT::AddOptionByName(std::string _optionName, std::string _actionName, int _priority)
{
	// create an option for action
	AIOption* _option = new AIOption();
	_option->Init(_optionName, actions[_actionName], _priority);
	options.insert({ _optionName, _option });
}

void AIConstructor_DT::AddOptionConsideration(std::string _optionName, std::string _considerationName)
{
	// create an option for action


	options[_optionName]->AddOptionConsideration(considerations[_considerationName]);
}

void AIConstructor_DT::AddSubReasonerOptionByName(std::string _subReasonerName, int _priority)
{

	/// create the Action - a SubReasoner Actions
	AIActionSubReasoner* _subReasoner = new AIActionSubReasoner();

	_subReasoner->Init(_subReasonerName, nullptr);
	actions.insert({ _subReasonerName, _subReasoner });

	// create the option, and link to the Action
	AIOption* _option = new AIOption();
	_option->Init(_subReasonerName, actions[_subReasonerName]);
	options.insert({ _subReasonerName, _option });


}


void AIConstructor_DT::AddOptionsToSubReasoner(std::string _subreasoner,  std::string _options)
{

	subReasonerLinks.insert({ _subreasoner, _options });



}

std::vector<std::string> AIConstructor_DT::GetOptionNamesForKey(std::string _subreasoner)
{

	std::vector<std::string> _matches;
	for (std::multimap<std::string, std::string>::iterator it = subReasonerLinks.begin(); it != subReasonerLinks.end(); ++it) {

		if (it->first == _subreasoner)
		{
			_matches.push_back(it->second);
		}
	}

	return _matches;

}


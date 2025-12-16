#include "DT_Reasoner.h"

#include "AIConstructor_DT.h"

bool DT_Reasoner::Init(std::string _id, AIBrainBlackboardBase& _context) {
	actorBlackboard = &_context;
	reasonerID = _id;
	return true;
}


/*
Think is called on demand to choose from the possible Options that AI has.

The function should implement the logic of the AI method to choose one Option as the selectedOption.
*/
void DT_Reasoner::Think()
{
	//for all options, check if they can be Triggered, and select one to Fire
	for (const auto& option : options)
	{


		// get all Consideration for the Option
		std::vector<AIConsiderationBase*> ruleList = option->GetOptionConsiderations();



		//TASK TODO: Complete Think() logic
		//  Check if all Considerations in the ruleList are TRUE
		//  Call Calculate() on each Consideration to get a boolean true/false return value if the condition is met
		//  Pass the blackboard to the Calculate function  e.g. Calculate(*actorBlackboard))
		//		
		// 
		//  If a rule's conditions are all met, then it may be selected


		bool conditionsMet = true;
		for (AIConsiderationBase* c : ruleList)
		{
			bool ruleResult = c->Calculate(*actorBlackboard);

			if (!ruleResult)
			{
				conditionsMet = false;
			}
		}

		if (conditionsMet)
		{
			// select this branch to explore
			selectedOption = option;

			break;
		}

	}



	
	// TASK TODO - By the end of the function,  'selectedOption' should be set to the option that is to be Fired
	// (the Firing will happen automatically later)


}






/*
---- AI Framework Function Overrides ----
*/

/*
SetOptions will create and initalise the Options and Considerations as defeined in the Constuctor
This includes creating and initalising any Sub Reasoners

*/


void DT_Reasoner::SetOptions(AIConstructorBase& _constructor)
{
	AIConstructor_DT* dtConstructor = static_cast<AIConstructor_DT*>(&_constructor);

	std::vector<std::string> _reasonerOptions = dtConstructor->GetOptionNamesForKey(reasonerID);

	std::vector<AIOptionBase*> _options = _constructor.GetOptionVector(_reasonerOptions);

	for (int i = 0; i < _options.size(); i++)
	{
		AIOption* o = new AIOption();


		std::string on = typeid(*_options[i]->GetOptionAction()).name();
		std::string::size_type n = on.find("AIActionSubReasoner");

	


		if (n!= std::string::npos)
		{

			// if the action for the option is an AIActionSubReasoner, we need to create the SubReasoner, and add its options
			AIActionSubReasoner* actionReasoner = new AIActionSubReasoner();
			actionReasoner->Init(on, nullptr);

			
			DT_Reasoner* subReasoner = new DT_Reasoner();
			std::string _optionName = _options[i]->GetOptionID();

			subReasoner->Init(_optionName, *actorBlackboard);

			std::vector<std::string> _suboptions = dtConstructor->GetOptionNamesForKey(_optionName);

			// call the SetOptions function to add options associated with this option
			// will also crate and initalise any sub reasoner actions within them
			subReasoner->SetOptions(_constructor);

			actionReasoner->SetChildReasoner(subReasoner);

			o->Init(_options[i]->GetOptionID(), actionReasoner, _options[i]->GetPriority());
			
		}
		else
		{
			o->Init(_options[i]->GetOptionID(), _options[i]->GetOptionAction(), _options[i]->GetPriority());

		}


		for (int j = 0; j < _options[i]->GetOptionConsiderations().size(); j++)
		{
			AIConsideration* c = new AIConsideration();
			c->Init(_options[i]->GetOptionConsiderations()[j]->GetID(), _options[i]->GetOptionConsiderations()[j]->GetRule());
			o->AddOptionConsideration(c);
		}

		options.push_back(o);
	}

}
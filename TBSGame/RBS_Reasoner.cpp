#include "RBS_Reasoner.h"



RBS_Reasoner::~RBS_Reasoner()
{


}


bool RBS_Reasoner::Init(std::string _id, AIBrainBlackboardBase& _context) {
	actorBlackboard = &_context;
	reasonerID = _id;
	return true;
}



/*
Think is called on demand to choose from the possible Options that AI has.

The function should implement the logic of the AI method to choose one Option as the selectedOption.
*/
void RBS_Reasoner::Think()
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
	

	




	}





	// TASK TODO - By the end of the function,  'selectedOption' should be set to the option that is to be Fired
	// (the Firing will happen automatically later)


}





/*
---- AI Framework Function Overrides ----
*/

/*
SetOptions will create and initalise the Options and Considerations as defeined in the Constuctor

*/


void RBS_Reasoner::SetOptions(AIConstructorBase& _constructor)
{

	std::vector<AIOptionBase*> _options = _constructor.GetOptionVector();
	
	for (int i = 0; i < _options.size(); i++)
	{
		AIOption* o = new AIOption();
		o->Init(_options[i]->GetOptionID(), _options[i]->GetOptionAction(), _options[i]->GetPriority());

		for (int j = 0; j < _options[i]->GetOptionConsiderations().size(); j++)
		{
			AIConsideration* c = new AIConsideration();
			c->Init(_options[i]->GetOptionConsiderations()[j]->GetID(), _options[i]->GetOptionConsiderations()[j]->GetRule());
			o->AddOptionConsideration(c);
		}

		options.push_back(o);
	}

}
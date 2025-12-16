#pragma once
#include "../../AI_Framework/framework.h"

class AIConstructor_RBS :
    public AIConstructorBase
{
public:
	bool Init() override;
	void DefineActions() override;
	void DefineConsiderations() override;
	void DefineOptions() override;


private:
	void AddOptionByName(std::string _optionName, std::string _actionName, int _priority = 0);
	void AddConsiderationByName(std::string _actionName, std::function<bool(AIBrainBlackboardBase&)> _function);
	void AddActionByName(std::string _actionName, std::function<ActionStatus(AIBrainBlackboardBase&)> _action);
	void AddOptionConsideration(std::string _optionName, std::string _considerationName);


};


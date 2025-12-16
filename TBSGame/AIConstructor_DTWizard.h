#pragma once
#include "../../AI_Framework/framework.h"
class AIConstructor_DTWizard :
    public AIConstructorBase
{
public:
	bool Init() override;
	void DefineActions() override;
	void DefineConsiderations() override;
	void DefineOptions() override;

	AIOptionBase* GetOptionByName(std::string _optionName);


	std::vector<std::string> GetOptionNamesForKey(std::string _subreasoner);

private:
	void AddOptionByName(std::string _optionName, std::string _actionName, int _priority = 0);
	void AddConsiderationByName(std::string _actionName, std::function<bool(AIBrainBlackboardBase&)> _function);
	void AddActionByName(std::string _actionName, std::function<ActionStatus(AIBrainBlackboardBase&)> _action);
	void AddOptionConsideration(std::string _optionName, std::string _considerationName);

	void AddSubReasonerOptionByName(std::string _subReasonerName, int _priority = 0);
	void AddOptionsToSubReasoner(std::string _subreasoner, std::string _options);

	std::multimap<std::string, std::string> subReasonerLinks;
};


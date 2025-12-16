#pragma once
#include "../../AI_Framework/framework.h"
class DT_Reasoner :
    public AIReasonerBase
{
public:
    // AIReasonerBase overrides
    bool Init(std::string _id, AIBrainBlackboardBase& _context) override;
    void Think() override;
    void SetOptions(AIConstructorBase& _constructor) override;
};


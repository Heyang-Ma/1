#pragma once
#include "../../AI_Framework/framework.h"
class RBS_Reasoner :
    public AIReasonerBase
{
public:

    ~RBS_Reasoner();

    // AIReasonerBase overrides
    bool Init(std::string _id, AIBrainBlackboardBase& _context) override;
    void Think() override;
    void SetOptions(AIConstructorBase& _constructor) override;
};


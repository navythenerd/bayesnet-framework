/// @file
/// @brief Example that demonstrates how to use the fuzzy logic to infer a CPT. Have a look at tools/cpt_inference.cpp, to see how to infer CPTs using network and rules file.

#include <iostream>
#include <bayesnet/fuzzy.h>

int main() {
    // setup mf
    bayesNet::fuzzyLogic::MembershipFunction *mfFalseSprinkler = new bayesNet::fuzzyLogic::membershipFunctions::Gaussian(0, 0.35);
    bayesNet::fuzzyLogic::MembershipFunction *mfTrueSprinkler = new bayesNet::fuzzyLogic::membershipFunctions::Gaussian(1, 0.35);
    bayesNet::fuzzyLogic::MembershipFunction *mfFalseRainy = new bayesNet::fuzzyLogic::membershipFunctions::Gaussian(0, 0.35);
    bayesNet::fuzzyLogic::MembershipFunction *mfTrueRainy = new bayesNet::fuzzyLogic::membershipFunctions::Gaussian(1, 0.35);

    // setup fuzzy sets
    bayesNet::fuzzyLogic::FuzzySet *fuzzySetSprinkler = new bayesNet::fuzzyLogic::FuzzySet(2);
    fuzzySetSprinkler->setMembershipFunction(bayesNet::state::FALSE, mfFalseSprinkler);
    fuzzySetSprinkler->setMembershipFunction(bayesNet::state::TRUE, mfTrueSprinkler);

    bayesNet::fuzzyLogic::FuzzySet *fuzzySetRainy = new bayesNet::fuzzyLogic::FuzzySet(2);
    fuzzySetRainy->setMembershipFunction(bayesNet::state::FALSE, mfFalseRainy);
    fuzzySetRainy->setMembershipFunction(bayesNet::state::TRUE, mfTrueRainy);

    std::vector<bayesNet::fuzzyLogic::FuzzySet *> wetGrass;
    wetGrass.push_back(fuzzySetSprinkler);
    wetGrass.push_back(fuzzySetRainy);

    // define states
    bayesNet::fuzzyLogic::RuleState *stateTrue = FUZZY_STATE(TRUE);
    bayesNet::fuzzyLogic::RuleState *stateFalse = FUZZY_STATE(FALSE);

    // define rules

    // S=0, R=0 => W=0
    std::vector<bayesNet::fuzzyLogic::RuleState *> states_s_F_R_F;
    states_s_F_R_F.push_back(stateFalse);
    states_s_F_R_F.push_back(stateFalse);

    bayesNet::fuzzyLogic::Rule *rule_s_F_R_F = new bayesNet::fuzzyLogic::Rule(states_s_F_R_F, stateFalse);

    // S=1, R=0 => W=1
    std::vector<bayesNet::fuzzyLogic::RuleState *> states_s_T_R_F;
    states_s_T_R_F.push_back(stateTrue);
    states_s_T_R_F.push_back(stateFalse);

    bayesNet::fuzzyLogic::Rule *rule_s_T_R_F = new bayesNet::fuzzyLogic::Rule(states_s_T_R_F, stateTrue);

    // S=0, R=1 => W=1
    std::vector<bayesNet::fuzzyLogic::RuleState *> states_s_F_R_T;
    states_s_F_R_T.push_back(stateFalse);
    states_s_F_R_T.push_back(stateTrue);

    bayesNet::fuzzyLogic::Rule *rule_s_F_R_T = new bayesNet::fuzzyLogic::Rule(states_s_F_R_T, stateTrue);

    // S=1, R=0 => W=1
    std::vector<bayesNet::fuzzyLogic::RuleState *> states_s_T_R_T;
    states_s_T_R_T.push_back(stateTrue);
    states_s_T_R_T.push_back(stateTrue);

    bayesNet::fuzzyLogic::Rule *rule_s_T_R_T = new bayesNet::fuzzyLogic::Rule(states_s_T_R_T, stateTrue);

    // combine rules
    std::vector<bayesNet::fuzzyLogic::Rule *> rulesWetGrass;
    rulesWetGrass.push_back(rule_s_F_R_F);
    rulesWetGrass.push_back(rule_s_T_R_F);
    rulesWetGrass.push_back(rule_s_F_R_T);
    rulesWetGrass.push_back(rule_s_T_R_T);

    bayesNet::fuzzyLogic::RuleSet *rules = new bayesNet::fuzzyLogic::RuleSet(rulesWetGrass);

    // create controller from fuzzy sets and rules
    bayesNet::fuzzyLogic::Controller *wetGrassCtrl = new bayesNet::fuzzyLogic::Controller(wetGrass, rules, 0.01);

    // infer cpts
    bayesNet::CPT wetGrassCPT = wetGrassCtrl->inferCPT();

    for (size_t i = 0; i < wetGrassCPT.size(); ++i) {
        std::cout << wetGrassCPT.get(i) << std::endl;
    }

    return 0;
}

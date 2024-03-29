#include <bayesnet/node.h>
#include <bayesnet/state.h>
#include <bayesnet/util.h>
#include <bayesnet/exception.h>


namespace bayesNet {

    Node::Node(const std::string &name, size_t label, size_t states) : _name(name), _factor(Factor(states)),
                                                                       _factorGraphIndex(0), _fuzzySet(states) {
        _discrete = dai::Var(label, states);
        _conditionalDiscrete = dai::VarSet(_discrete);
    }

    Node::~Node() {}

    void Node::addChild(Node *node) {
        _children.push_back(node);
        node->_conditionalDiscrete |= _discrete;
    }

    Factor &Node::getFactor() {
        if (_factor.vars() != _conditionalDiscrete) {
            _factor = Factor(_conditionalDiscrete, _discrete.states());
        }

        return _factor;
    }

    void Node::setEvidence(size_t state) {
        getFactor().setEvidence(state);
    }

    void Node::clearEvidence() {
        getFactor().clearEvidence();
    }

    void Node::setCPT(const CPT &cpt) {
        _cpt = cpt;
        Factor &factor = getFactor();

        for (size_t i = 0; i < cpt.size(); ++i) {
            factor.set(i, dai::Real(cpt.get(i)));
        }
    }

    const std::string &Node::getName() const {
        return _name;
    }

    dai::Var &Node::getDiscrete() {
        return _discrete;
    }

    dai::Var Node::getDiscrete() const {
        return _discrete;
    }

    dai::VarSet &Node::getConditionalDiscrete() {
        return _conditionalDiscrete;
    }

    dai::VarSet Node::getConditionalDiscrete() const {
        return _conditionalDiscrete;
    }

    size_t Node::nrStates() const {
        return _discrete.states();
    }

    std::vector<Node *> &Node::getChildren() {
        return _children;
    }

    void Node::setMembershipFunction(size_t state, fuzzyLogic::MembershipFunction *mf) {
        _fuzzySet.setMembershipFunction(state, mf);
    }

    void Node::setFuzzyRules(const fuzzyLogic::RuleSet &rules) {
        _fuzzyRules = rules;
    }

    void Node::addFuzzyRule(fuzzyLogic::Rule *rule) {
        _fuzzyRules.addRule(rule);
    }

    fuzzyLogic::FuzzySet &Node::getFuzzySet() {
        return _fuzzySet;
    }

    fuzzyLogic::RuleSet &Node::getFuzzyRules() {
        return _fuzzyRules;
    }

    CPT &Node::getCPT() {
        return _cpt;
    }

    void Node::setFactorGraphIndex(size_t index) {
        _factorGraphIndex = index;
    }

    size_t Node::getFactorGraphIndex() const {
        return _factorGraphIndex;
    }

    bool Node::isBinary() const {
        return _discrete.states() == 2;
    }

    bool Node::isEvidence() const {
        return _factor.isEvidence();
    }

    size_t Node::evidenceState() const {
        return _factor.evidenceState();
    }

    std::ostream &operator<<(std::ostream &os, Node &node) {
        os << node.getDiscrete() << " - " << node.getConditionalDiscrete() << " " << node.getFactor();

        return os;
    }

    SensorNode::SensorNode(const std::string &name, size_t label, size_t states) : Node(name, label, states) {}

    SensorNode::~SensorNode() {}

    void SensorNode::observe(double x) {
        // get state strenth from fuzzy set
        std::vector<double> strength = getFuzzySet().getStrength(x);

        for (size_t i = 0; i < strength.size(); i++) {
            double belief = strength[i];
            int trunc = static_cast<int>(belief * 100);
            strength[i] = trunc / 100.0;
        }

        utils::vectorNormalize(strength);        

        // set cpt for node
        CPT cpt(strength);
        setCPT(cpt);
    }
}

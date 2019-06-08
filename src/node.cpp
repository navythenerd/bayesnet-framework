//
// Created by Cedrik Kaufmann on 2019-05-28.
//

#include <bayesnet/node.h>
#include <bayesnet/state.h>

namespace BayesNet {

    Node::Node(size_t label, size_t states) : _factorGraphIndex(0) {
        this->_discrete = dai::Var(label, states);
        this->_conditionalDiscrete = dai::VarSet(this->_discrete);
    }

    void Node::addChild(Node *node) {
        this->_children.push_back(node);
        node->_conditionalDiscrete |= this->_discrete;
    }

    Factor &Node::factor() {
        if (this->_factor.vars() != this->_conditionalDiscrete) {
            this->_factor = Factor(this->_conditionalDiscrete);
        }

        return this->_factor;
    }

    void Node::setEvidence(size_t state) {
        this->factor().setEvidence(state);
    }

    void Node::clearEvidence() {
        this->factor().clearEvidence();
    }
}
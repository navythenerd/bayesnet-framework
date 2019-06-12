//
// Created by Cedrik Kaufmann on 2019-06-12.
//

#include <bayesnet/exception.h>

namespace BayesNet {

    const char* NotInitializedException::what() const noexcept {
        return "Bayesian network not initialized";
    }

    const char* BayesNodeAlreadyDefinedException::what() const noexcept {
        return "Bayes node with the same identifier already registered";
    }

    const char* BayesNodeNotFoundException::what() const noexcept {
        return "Bayes node with given identifier does not exist";
    }
}
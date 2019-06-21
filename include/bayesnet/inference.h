//
// Created by Cedrik Kaufmann on 2019-06-12.
//

#ifndef BAYESNET_FRAMEWORK_INFERENCE_H
#define BAYESNET_FRAMEWORK_INFERENCE_H

#include <dai/properties.h>

namespace BayesNet {

    enum InferenceProperties {
        LOOPY_BELIEF_PROPAGATION_SUMPROD,
        LOOPY_BELIEF_PROPAGATION_MAXPROD,
        CONDITIONED_BELIEF_PROPAGATION,
        FRACTIONAL_BELIEF_PROPAGATION
    };
}

// DEFAULT
#define CONFIG_INFERENCE_MAXIMUM_ITERATIONS 1000
#define CONFIG_INFERENCE_TOLERANCE 1e-9
#define CONFIG_INFERENCE_VERBOSE 1

// LOOPY BELIEF PROPAGATION SUMPROD
#define CONFIG_LOOPY_BELIEF_PROPAGATION_SUMPROD_UPDATES "SEQRND"
#define CONFIG_LOOPY_BELIEF_PROPAGATION_SUMPROD_INFERENCE "SUMPROD"
#define CONFIG_LOOPY_BELIEF_PROPAGATION_SUMPROD_LOGDOMAIN false

// LOOPY BELIEF PROPAGATION MAXPROD
#define CONFIG_LOOPY_BELIEF_PROPAGATION_MAXPROD_UPDATES "SEQRND"
#define CONFIG_LOOPY_BELIEF_PROPAGATION_MAXPROD_INFERENCE "MAXPROD"
#define CONFIG_LOOPY_BELIEF_PROPAGATION_MAXPROD_LOGDOMAIN false

// CONDITIONED BELIEF PROPAGATION
#define CONFIG_CONDITIONED_BELIEF_PROPAGATION_UPDATES "SEQRND"
#define CONFIG_CONDITIONED_BELIEF_PROPAGATION_REC_TOL 1e-9
#define CONFIG_CONDITIONED_BELIEF_PROPAGATION_MIN_MAX_ADJ 10
#define CONFIG_CONDITIONED_BELIEF_PROPAGATION_CHOOSE "CHOOSE_BBP"
#define CONFIG_CONDITIONED_BELIEF_PROPAGATION_RECURSION "REC_LOGZ"
#define CONFIG_CONDITIONED_BELIEF_PROPAGATION_CLAMP "CLAMP_VAR"
#define CONFIG_CONDITIONED_BELIEF_PROPAGATION_CHOOSE "CHOOSE_BBP"
#define CONFIG_CONDITIONED_BELIEF_PROPAGATION_BBP_CFN "CFN_GIBBS_B"
#define CONFIG_CONDITIONED_BELIEF_PROPAGATION_BBP_UPDATES "SEQ_BP_REV"
#define CONFIG_CONDITIONED_BELIEF_PROPAGATION_BBP_DAMPING 0.1

// FRACTIONAL BELIEF PROPAGATION
#define CONFIG_FRACTIONAL_BELIEF_PROPAGATION_UPDATES "SEQRND"
#define CONFIG_FRACTIONAL_BELIEF_PROPAGATION_INFERENCE "SUMPROD"
#define CONFIG_FRACTIONAL_BELIEF_PROPAGATION_LOGDOMAIN false

namespace BayesNet {
    dai::PropertySet getInferenceProperties(InferenceProperties inf);
}

#endif //BAYESNET_FRAMEWORK_INFERENCE_H

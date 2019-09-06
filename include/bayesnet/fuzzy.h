/*  This file is part of libBayesNet
 *
 *  Copyright (c) 2019, The libBayesNet authors. All rights reserved.
 */


/// @file
/// @brief Defines fuzzy logic toolbox containing membership functions, fuzzy sets and fuzzy rules. This is used for fuzzy inference and continuous/discrete variable mapping.
/// @author Cedrik Kaufmann
/// @version 1.0


#ifndef BAYESNET_FRAMEWORK_FUZZY_H
#define BAYESNET_FRAMEWORK_FUZZY_H


#include <vector>

#include <bayesnet/cpt.h>
#include <bayesnet/state.h>


/// Macro that simplifies getting an fuzzy rule state.
/** @param code Corresponds to one of the enum values of bayesNet::states::State or bayesNet::states::StateBinary
 *
 *  @par Example:
 *  @code
 *  FUZZY_STATE(TRUE);
 *  FUZZY_STATE(GOOD);
 *  @endcode
 */
#define FUZZY_STATE(s) bayesNet::fuzzyLogic::states::get(bayesNet::state::s)


namespace bayesNet {

    namespace fuzzyLogic {

        /// Represents the base class for membership functions used by fuzzy logic
        /** A membership function represents the strength a discrete/state occuring 
         *  based on a continuous overserved value.
         */
        class MembershipFunction {
        public:
        /// @name Constructors and Destructors
        //@{
            /// Constructor
            MembershipFunction();

            /// Destructor
            virtual ~MembershipFunction();
        //@}

            /// Returns strength value based on value @a x
            /// @return strength value
            virtual double fx(double x) const = 0;

            /// Returns the position of the maximum of the function
            /// @return maximum position
            virtual double findMaximum() const = 0;

            /// Returns string representation of membership function
            /// @return string representation
            virtual std::string toString() const = 0;
        };

        /// Stream operator to write string reperesentation of membership function @a mf to iostream @a os 
        std::ostream &operator<<(std::ostream &os, MembershipFunction &mf);

        /// Represents a set of membership function, mapping them to a state. This is called fuzzy set.
        /** Each state/event has an own membership function mapped to it. Those can overlap, so
         *  a certain amount of strength a state ocurring corresponding to multiple states can be 
         *  calculated based on a continuous observed value.
         */
        class Set {
        public:
        /// @name Constructors and Destructors
        //@{
            /// Constructs a fuzzy set using @a states and tolerance value @a tol
            explicit Set(size_t states, double tol = 0);

            /// Destructor
            virtual ~Set();
        //@}

            /// Sets a membership function @a mf for @a state
            void setMembershipFunction(size_t state, MembershipFunction *mf);

            /// Returns a membership function for @a state
            /// @return membership function
            MembershipFunction *getMembershipFunction(size_t state);

            /// Returns maximum position for membership function for @a state 
            /// @return maximum position
            double findMaximum(size_t state) const;

            /// Returns the strength vector for continuous value @a x
            /// @return vector of strength
            std::vector<double> getStrength(double x) const;

            /// Returns the strength for continuous value @a x and memebership function for @a state
            /// @return strength
            double getStrength(double x, size_t state) const;

            /// Returns the number of states
            /// @return number of states
            size_t nrStates() const;

        private:
            /// Stores the null belief tolerance, which is used in case of belief null instead
            double _nullBeliefTolerance;

            /// Stores the membership functions
            std::vector<MembershipFunction *> _mf;
        };

        /// Represents a state and is used to express fuzzy rules.
        /** Each fuzzy state consist of its proper states and a flag if its representing binary set or not
         */
        class RuleState {
        public:
        /// @name Constructors and Destructors
        //@{
            /// Constructs a new rule state corresponding to @a state and @a binary
            explicit RuleState(size_t state, bool binary = false);

            /// Destructor
            virtual ~RuleState();
        //@}

            /// Returns if rule state is binary or not
            /// @return binary flag
            bool isBinary() const;

            /// Returns the actual state
            /// @return state
            size_t getState() const;

        private:
            /// Stores the state
            size_t _state;

            /// Stores the binary flag
            bool _binary;
        };

        namespace states {

            /// Returns a pointer to a fuzzy rule with corresponding @a state
            /// @return fuzzy rule state
            RuleState *get(const state::State &state);

            /// Returns a pointer to a binary fuzzy rule with corresponding @a state
            /// @return binary fuzzy rule state
            RuleState *get(const state::StateBinary &state);
        }

        /// Represents a fuzzy rule, which consists of a set of fuzzy rule states for its corresponding conditionals.
        /** A fuzzy rule state is mapped to its conditionals, thus represents a mathematical boolean expression of 
         *  the form \f$if 0=TRUE & 1=TRUE then TRUE\f$
         */
        class Rule {
        public:
        /// @name Constructors and Destructors
        //@{
            /// Constructs a fuzzy rule corresponding to the conditionals @a parentStates and @a state
            explicit Rule(const std::vector<RuleState *> &parentStates, RuleState *state);

            /// Destructor
            virtual ~Rule();
        //@}

            /// Returns the conditional rule states
            /// @return vector rule states
            std::vector<RuleState *> &getParentStates();

            /// Returns the resulting rule state
            /// @return rule state
            RuleState &getChildState();

            /// Returns the number of joint states
            /// @return number states
            size_t nrJointStates() const;

        private:
            /// Stores the conditional rule states
            std::vector<RuleState *> _parentStates;

            /// Stores the resulting rule state
            RuleState &_state;
        };

        //// Represents a set of fuzzy rules.
        /** A fuzzy rule expresses a whole system of state rules and therefore can be used to apply fuzzy inference on them,
         *  to infer a CPT corresponding to the set of rule states.
         */
        class RuleSet {
        public:
        /// @name Constructors and Destructors
        //@{
            /// Constructor
            RuleSet();

            /// Constructs a set of rules using the rule vector @a rules
            explicit RuleSet(const std::vector<Rule *> &rules);

            /// Destructor
            virtual ~RuleSet();
        //@}

            /// Adds a new @a rule to the set
            void addRule(Rule *rule);

            /// Sets the rules using the rule vector @a rules
            void setRules(const std::vector<Rule *> &rules);

            /// Returns all fuzzy rules
            /// @return vector of rules
            std::vector<Rule *> &getRules();

            /// Returns the number of joint states
            /// @return number of states
            size_t nrJointStates() const;

        private:
            /// Stores the rules
            std::vector<Rule *> _rules;
        };

        //// Represents a fuzzy controller. 
        /** A fuzzy controller uses a set of membership function and a set of fuzzy rules to apply fuzzy inference on them
         *  and calculate a corresponding CPT.
         */
        class Controller {
        public:
        /// @name Constructors and Destructors
        //@{
            /// Constructs a controller using a fuzzy @a set, fuzzy @a rules and the null belief @a tolerance
            Controller(const std::vector<Set *> &set, RuleSet *rules, double tolerance = 0);

            /// Destructor
            virtual ~Controller();
        //@}

            /// Returns a CPT inferred applying fuzzy inference on the corresponding fuzzy sets/-rules
            /// @return CPT
            CPT inferCPT();

        private:
            /// Stores the fuzzy rules
            RuleSet *_rules;

            /// Stores the fuzzy set
            std::vector<Set *> _fuzzySet;

            /// Stores the null belief tolerance
            double _nullBeliefTolerance;

            /// Returns a inferred partial set of probabilities based on @a states
            /// @return vector of probabilities
            std::vector<double> infer(const std::vector<size_t> &states);
        };

        namespace membershipFunctions {

            /// Returns a memebership function based on string @a s
            /// @return membership function
            MembershipFunction *fromString(std::string s);

            //// Represents a linear membership function. 
            class Linear : public MembershipFunction {
            public:
                /// Constructs a linear membership function based on @a fxMin and @a fxMax
                Linear(double fxMin, double fxMax);

                /// Returns strength value based on value @a x
                /// @return strength value
                virtual double fx(double x) const;

                /// Returns the position of the maximum of the function
                /// @return maximum position
                virtual double findMaximum() const;

                /// Returns string representation of membership function
                /// @return string representation
                virtual std::string toString() const;

            private:
                /// Stores slope
                double _m;

                /// Stores x-intercept
                double _b;

                /// Store minimum value x-position
                double _fxMin;

                /// Store maximum value x-position
                double _fxMax;
            };

            //// Represents a triangular membership function. 
            class Triangle : public MembershipFunction {
            public:
                Triangle(double begin, double max, double end);

                /// Returns strength value based on value @a x
                /// @return strength value
                virtual double fx(double x) const;

                /// Returns the position of the maximum of the function
                /// @return maximum position
                virtual double findMaximum() const;

                /// Returns string representation of membership function
                /// @return string representation
                virtual std::string toString() const;

            private:
                /// Stores begin of increasing slope
                double _begin;

                /// Stores maximum
                double _max;

                /// Stores end of decreasing slope
                double _end;

                /// Stores increasing linear
                Linear _increasing;

                /// Stores decreasing linear
                Linear _decreasing;
            };

            //// Represents a trapezoidal membership function. 
            class Trapezoid : public MembershipFunction {
            public:
                Trapezoid(double x1, double x2, double x3, double x4);

                /// Returns strength value based on value @a x
                /// @return strength value
                virtual double fx(double x) const;

                /// Returns the position of the maximum of the function
                /// @return maximum position
                virtual double findMaximum() const;

                /// Returns string representation of membership function
                /// @return string representation
                virtual std::string toString() const;

            private:
                /// Stores begin of increasing slope
                double _increasingBegin;

                /// Stores end of increasing slope
                double _increasingEnd;

                /// Stores begin of decreasing slope
                double _decreasingBegin;

                /// Stores end of decreasing slope
                double _decreasingEnd;

                /// Stores increasing linear
                Linear _increasingLinear;

                /// Stores decreasing slope
                Linear _decreasingLinear;
            };

            //// Represents a s-shaped membership function. 
            class SShape : public MembershipFunction {
            public:
                SShape(double a, double b);

                /// Returns strength value based on value @a x
                /// @return strength value
                virtual double fx(double x) const;

                
                double getMinPos() const;

                double getMaxPos() const;

                /// Returns the position of the maximum of the function
                /// @return maximum position
                virtual double findMaximum() const;

                /// Returns string representation of membership function
                /// @return string representation
                virtual std::string toString() const;

            private:
                double _a;
                double _b;
            };

            //// Represents a z-shaped membership function. 
            class ZShape : public MembershipFunction {
            public:
                ZShape(double a, double b);

                /// Returns strength value based on value @a x
                /// @return strength value
                virtual double fx(double x) const;

                double getMaxPos() const;

                double getMinPos() const;

                /// Returns the position of the maximum of the function
                /// @return maximum position
                virtual double findMaximum() const;

                /// Returns string representation of membership function
                /// @return string representation
                virtual std::string toString() const;

            private:
                SShape _sShape;
            };

            //// Represents a pi-shaped membership function. 
            class PiShape : public MembershipFunction {
            public:
                PiShape(double a, double b, double c, double d);

                /// Returns strength value based on value @a x
                /// @return strength value
                virtual double fx(double x) const;

                /// Returns the position of the maximum of the function
                /// @return maximum position
                virtual double findMaximum() const;

                /// Returns string representation of membership function
                /// @return string representation
                virtual std::string toString() const;

            private:
                SShape _sShape;
                ZShape _zShape;
            };

            //// Represents a sigmoidal membership function. 
            class Sigmoid : public MembershipFunction {
            public:
                Sigmoid(double a, double c);

                /// Returns strength value based on value @a x
                /// @return strength value
                virtual double fx(double x) const;

                /// Returns the position of the maximum of the function
                /// @return maximum position
                virtual double findMaximum() const;

                /// Returns string representation of membership function
                /// @return string representation
                virtual std::string toString() const;

            private:
                double _a;
                double _c;
            };

            //// Represents a bell curve membership function. 
            class Bell : public MembershipFunction {
            public:
                Bell(double a, double b, double c);

                /// Returns strength value based on value @a x
                /// @return strength value
                virtual double fx(double x) const;

                /// Returns the position of the maximum of the function
                /// @return maximum position
                virtual double findMaximum() const;

                /// Returns string representation of membership function
                /// @return string representation
                virtual std::string toString() const;

            private:
                double _a;
                double _b;
                double _c;
            };

            //// Represents a gaussian membership function.
            class Gaussian : public MembershipFunction {
            public:
                Gaussian(double mean, double deviation);

                /// Returns strength value based on value @a x
                /// @return strength value
                virtual double fx(double x) const;

                double getMean() const;

                double getDeviation() const;

                /// Returns the position of the maximum of the function
                /// @return maximum position
                virtual double findMaximum() const;

                /// Returns string representation of membership function
                /// @return string representation
                virtual std::string toString() const;

            private:
                double _mean;
                double _deviation;
            };

            //// Represents a membership function resulting from two combined gaussians.
            class Gaussian2 : public MembershipFunction {
            public:
                Gaussian2(double meanLeft, double deviationLeft, double meanRight, double deviationRight);

                /// Returns strength value based on value @a x
                /// @return strength value
                virtual double fx(double x) const;

                /// Returns the position of the maximum of the function
                /// @return maximum position
                virtual double findMaximum() const;

                /// Returns string representation of membership function
                /// @return string representation
                virtual std::string toString() const;

            private:
                Gaussian _left;
                Gaussian _right;
            };
        }
    }
}


#endif //BAYESNET_FRAMEWORK_FUZZY_H

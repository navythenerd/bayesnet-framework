#include <math.h>
#include <limits>
#include <sstream>
#include <regex>
#include <memory>

#include <bayesnet/fuzzy.h>
#include <bayesnet/util.h>
#include <bayesnet/exception.h>


namespace bayesNet {

    namespace fuzzyLogic {

        namespace membershipFunctions {

            Linear::Linear(double fxMin, double fxMax) : _fxMin(fxMin), _fxMax(fxMax) {
                _m = 1 / (fxMax - fxMin);
                _b = fxMin;
            }

            double Linear::fx(double x) const {
                if (_fxMin < _fxMax) {
                    if (x <= _fxMin) {
                        return 0;
                    }

                    if (x >= _fxMax) {
                        return 1;
                    }
                } else {
                    if (x <= _fxMax) {
                        return 1;
                    }

                    if (x >= _fxMin) {
                        return 0;
                    }
                }

                return (_m * (x - _b));
            }

            double Linear::findMaximum() const {
                return _fxMax;
            }

            const std::string Linear::toString() const {
                std::stringstream ss;
                ss << "\"linear\": " << "[" << _fxMin << ", " << _fxMax << "]";
                return ss.str();
            }

            Triangle::Triangle(double begin, double max, double end) : _begin(begin), _max(max), _end(end),
                                                                       _increasing(_begin, _max),
                                                                       _decreasing(_end, _max) {

            }

            double Triangle::fx(double x) const {
                if (x <= _begin || x >= _end) {
                    return 0;
                }

                if (x > _begin && x < _max) {
                    return _increasing.fx(x);
                }

                if (x > _max && x < _end) {
                    return _decreasing.fx(x);
                }

                return 1;
            }

            double Triangle::findMaximum() const {
                return _max;
            }

            const std::string Triangle::toString() const {
                std::stringstream ss;
                ss << "\"triangle\": " << "[" << _begin << ", " << _max << ", " << _end << "]";
                return ss.str();
            }

            Trapezoid::Trapezoid(double x1, double x2, double x3, double x4) : _increasingBegin(x1), _increasingEnd(x2), 
                                                                               _decreasingBegin(x3), _decreasingEnd(x4),
                                                                               _increasingLinear(x1, x2), _decreasingLinear(x4, x3) {}

            double Trapezoid::fx(double x) const {
                if (x <= _increasingBegin || x >= _decreasingEnd) {
                    return 0;
                }

                if (x >= _increasingEnd && x <= _decreasingBegin) {
                    return 1;
                }

                if (x > _increasingBegin && x < _increasingEnd) {
                    return _increasingLinear.fx(x);
                }

                return _decreasingLinear.fx(x);
            }

            double Trapezoid::findMaximum() const {
                return (_decreasingBegin - _increasingEnd) / 2;
            }

            const std::string Trapezoid::toString() const {
                std::stringstream ss;
                ss << "\"trapezoid\": " << "[" << _increasingBegin << ", " << _increasingEnd << ", " << _decreasingBegin << ", " << _decreasingEnd << "]";
                return ss.str();
            }

            SShape::SShape(double a, double b) : _a(a), _b(b) {}

            double SShape::fx(double x) const {
                if (x <= _a) {
                    return 0;
                }

                if (x >= _b) {
                    return 1;
                }

                if (x > _a && x <= (_a + _b) / 2) {
                    return 2 * std::pow((x - _a) / (_b - _a), 2);
                }

                return 1 - 2 * std::pow((x - _b) / (_b - _a), 2);
            }

            double SShape::getMinPos() const {
                return _a;
            }

            double SShape::getMaxPos() const {
                return _b;
            }

            double SShape::findMaximum() const {
                return _b;
            }

            const std::string SShape::toString() const{
                std::stringstream ss;
                ss << "\"sshape\": " << "[" << _a << ", " << _b << "]";
                return ss.str();
            }

            ZShape::ZShape(double a, double b) : _sShape(a, b) {}

            double ZShape::fx(double x) const {
                return 1 - _sShape.fx(x);
            }

            double ZShape::getMaxPos() const {
                return _sShape.getMaxPos();
            }

            double ZShape::getMinPos() const {
                return _sShape.getMinPos();
            }

            double ZShape::findMaximum() const {
                return _sShape.getMinPos();
            }

            const std::string ZShape::toString() const {
                std::stringstream ss;
                ss << "\"zshape\": " << "[" << getMaxPos() << ", " << getMinPos() << "]";
                return ss.str();
            }

            Bell::Bell(double a, double b, double c) : _a(a), _b(b), _c(c) {}

            double Bell::fx(double x) const {
                return 1 / (1 + std::pow(std::abs((x - _c) / _a), 2 * _b));
            }

            double Bell::findMaximum() const {
                return _c;
            }

            const std::string Bell::toString() const {
                std::stringstream ss;
                ss << "\"linear\": " << "[" << _a << ", " << _b << ", " << _c << "]";
                return ss.str();
            }

            double Gaussian::fx(double x) const {
                return std::exp(-std::pow(x - _mean, 2) / (2 * std::pow(_deviation, 2)));
            }

            Gaussian::Gaussian(double mean, double deviation) : _mean(mean), _deviation(deviation) {}

            double Gaussian::getMean() const {
                return _mean;
            }

            double Gaussian::getDeviation() const {
                return _deviation;
            }

            double Gaussian::findMaximum() const {
                return _mean;
            }

            const std::string Gaussian::toString() const {
                std::stringstream ss;
                ss << "\"gaussian\": " << "[" << _mean << ", " << _deviation << "]";
                return ss.str();
            }

            Gaussian2::Gaussian2(double meanLeft, double deviationLeft, double meanRight, double deviationRight) : _left(meanLeft, deviationLeft), 
                                                                                                                   _right(meanRight, deviationRight) {}

            double Gaussian2::fx(double x) const {
                double gaussianLeft;
                double gaussianRight;

                if (x >= _left.getMean()) {
                    gaussianLeft = 1;
                } else {
                    gaussianLeft = _left.fx(x);
                }

                if (x <= _right.getMean()) {
                    gaussianRight = 1;
                } else {
                    gaussianRight = _right.fx(x);
                }

                return gaussianLeft * gaussianRight;
            }

            double Gaussian2::findMaximum() const {
                return (_left.getMean() + _right.getMean()) / 2;
            }

            const std::string Gaussian2::toString() const{
                std::stringstream ss;
                ss << "\"gaussian2\": " << "[" << _left.getMean() << ", " << _left.getDeviation() << ", " << _right.getMean() << ", " << _right.getDeviation() << "]";
                return ss.str();
            }

            PiShape::PiShape(double a, double b, double c, double d) : _sShape(a, b), _zShape(c, d) {}

            double PiShape::fx(double x) const {
                if (x <= _zShape.getMaxPos()) {
                    return _sShape.fx(x);
                }

                return _zShape.fx(x);
            }

            double PiShape::findMaximum() const {
                return (_sShape.getMaxPos() + _zShape.getMaxPos()) / 2;
            }

            const std::string PiShape::toString() const {
                std::stringstream ss;
                ss << "\"pishape\": " << "[" << _sShape.getMinPos() << ", " << _sShape.getMaxPos() << ", " << _zShape.getMaxPos() << ", " << _zShape.getMinPos() << "]";
                return ss.str();
            }

            Sigmoid::Sigmoid(double a, double c) : _a(a), _c(c) {}

            double Sigmoid::fx(double x) const {
                return 1 / (1 + std::exp(-1 * _a * (x - _c)));
            }

            double Sigmoid::findMaximum() const {
                double pos = _c;
                double step = 0.1;

                double val = fx(_c);
                double valBefore = val - 1;

                while (std::abs(val - valBefore) > 0.01) {

                    pos += step;
                    valBefore = val;
                    val = fx(pos);
                }

                return pos;
            }

            const std::string Sigmoid::toString() const {
                std::stringstream ss;
                ss << "\"sigmoid\": " << "[" << _a << ", " << _c << "]";
                return ss.str();
            }

            MembershipFunction *fromString(std::string s) {
                // make sure that string to double conversion through std::stod works correctly
                setlocale(LC_ALL, "C/de_DE.UTF-8/en_US.UTF-8/C/C/C/C");

                // regex for curve string processing and matching
                std::regex curveRegEx("^\\s*\"([a-zA-Z0-9_]+)\"\\s*:\\s*\\[((\\s*([0-9]+\\.?)\\s*,?)*)\\]$");
                std::smatch match;

                // match string against regex
                if (std::regex_match(s, match, curveRegEx)) {
                    std::vector<std::string> valuesStr = utils::split(match.str(2), ',');
                    std::vector<double> values;

                    for (size_t i = 0; i < valuesStr.size(); i++) {
                        values.push_back(std::stod(valuesStr[i]));
                    }
                    
                    MembershipFunction *mf = nullptr;

                    std::string curveName = match.str(1);

                    if (curveName == "linear") {
                        mf = new Linear(values[0], values[1]);
                        goto factoryReturn;
                    }

                    if (curveName == "triangle") {
                        mf = new Triangle(values[0], values[1], values[2]);
                        goto factoryReturn;
                    }

                    if (curveName == "trapezoid") {
                        mf = new Trapezoid(values[0], values[1], values[2], values[3]);
                        goto factoryReturn;
                    }

                    if (curveName == "sshape") {
                        mf = new SShape(values[0], values[1]);
                        goto factoryReturn;
                    }

                    if (curveName == "zshape") {
                        mf = new ZShape(values[0], values[1]);
                        goto factoryReturn;
                    }

                    if (curveName == "pishape") {
                        mf = new PiShape(values[0], values[1], values[2], values[3]);
                        goto factoryReturn;
                    }

                    if (curveName == "sigmoid") {
                        mf = new Sigmoid(values[0], values[1]);
                        goto factoryReturn;
                    }

                    if (curveName == "bell") {
                        mf = new Bell(values[0], values[1], values[2]);
                        goto factoryReturn;
                    }

                    if (curveName == "gaussian") {
                        mf = new Gaussian(values[0], values[1]);
                        goto factoryReturn;
                    }

                    if (curveName == "gaussian2") {
                        mf = new Gaussian2(values[0], values[1], values[2], values[3]);
                        goto factoryReturn;
                    }

                    BAYESNET_THROWE(INVALID_MF_STRING, s);

                    factoryReturn:
                        return mf;  
                }

                BAYESNET_THROWE(INVALID_MF_STRING, s);
            }
        }

        MembershipFunction::MembershipFunction() {}

        MembershipFunction::~MembershipFunction() {}

        std::ostream &operator<<(std::ostream &os, MembershipFunction &mf) {
            os << mf.toString();
            return os;
        }

        FuzzySet::FuzzySet(size_t states) : _mf(states) {
            for (size_t i = 0; i < states; i++) {
                _mf[i] = nullptr;
            }
        }

        FuzzySet::~FuzzySet() {
            // free memory for membership functions
            for (auto mf : _mf) {
                if (mf != nullptr) {
                    delete mf;
                }
            }
        }

        MembershipFunction *FuzzySet::getMembershipFunction(size_t state) {
            return _mf[state];
        }

        std::vector<double> FuzzySet::getStrength(double x) const {
            size_t states = _mf.size();
            std::vector<double> beliefs(states);

            for (size_t i = 0; i < states; ++i) {
                beliefs[i] = _mf[i]->fx(x);
            }

            return beliefs;
        }

        double FuzzySet::getStrength(double x, size_t state) const {
            if (state >= _mf.size()) {
                BAYESNET_THROW(INDEX_OUT_OF_BOUNDS);
            }

            return _mf[state]->fx(x);
        }

        double FuzzySet::findMaximum(size_t state) const {
            return _mf[state]->findMaximum();
        }

        void FuzzySet::setMembershipFunction(size_t state, MembershipFunction *mf) {
            _mf[state] = mf;
        }

        size_t FuzzySet::nrStates() const {
            return _mf.size();
        }

        Rule::Rule(const std::vector<RuleState *> &parentStates, RuleState *state) : _state(*state) {
            _parentStates = parentStates;
        }

        std::vector<RuleState *> &Rule::getParentStates() {
            return _parentStates;
        }

        RuleState &Rule::getChildState() {
            return _state;
        }

        size_t Rule::nrJointStates() const {
            size_t jointStates = 1;

            if (_state.isBinary()) {
                jointStates *= 2;
            } else {
                jointStates *= 4;
            }

            for (size_t i = 0; i < _parentStates.size(); ++i) {
                if (_parentStates[i]->isBinary()) {
                    jointStates *= 2;
                } else {
                    jointStates *= 4;
                }
            }

            return jointStates;
        }

        Rule::~Rule() {}

        void RuleSet::addRule(Rule *rule) {
            _rules.push_back(rule);
        }

        RuleSet::RuleSet() {}

        RuleSet::RuleSet(const std::vector<Rule *> &rules) {
            _rules = rules;
        }

        RuleSet::~RuleSet() {}

        std::vector<Rule *> &RuleSet::getRules() {
            return _rules;
        }

        size_t RuleSet::nrJointStates() const {
            return _rules.size() * _rules[0]->nrJointStates();
        }

        Controller::Controller(const std::vector<FuzzySet *> &set, RuleSet *rules, double tolerance) : _rules(rules), _fuzzySet(set),
                                                                                                       _nullBeliefTolerance(tolerance) {

        }

        Controller::~Controller() {}

        CPT Controller::inferCPT() {
            // get rules
            std::vector<Rule *> &fuzzyRules = _rules->getRules();
            // create cpt
            size_t jointStates = fuzzyRules[0]->nrJointStates();
            CPT cpt(jointStates);

            // get states
            std::vector<RuleState *> &parentStates = fuzzyRules[0]->getParentStates();
            std::vector<size_t> maxStates(parentStates.size());

            for (size_t j = 0; j < parentStates.size(); ++j) {
                if (parentStates[j]->isBinary()) {
                    maxStates[j] = 2;
                } else {
                    maxStates[j] = 4;
                }
            }
            
            // init state counter
            utils::Counter stateCounter(parentStates.size(), maxStates);

            // iterate over every possible parental state and infer partial cpt
            do {
                std::vector<double> inferred = infer(stateCounter.getCount());
                size_t maxIncrement = stateCounter.getMaximumIncrement();

                // apply partial cpt
                for (size_t i = 0; i < inferred.size(); i++) {
                    cpt.set((i * maxIncrement) + stateCounter.getIncrement(), inferred[i]);
                }
            } while (stateCounter.countUp());

            // return cpt
            return cpt;
        }

        std::vector<double> Controller::infer(const std::vector<size_t> &states) {
            std::vector<double> max(states.size());

            // find maximum for given states from fuzzy sets
            for (size_t i = 0; i < states.size(); ++i) {
                max[i] = _fuzzySet[i]->findMaximum(states[i]);
            }

            // get rules
            std::vector<Rule *> &rules = _rules->getRules();
            std::vector<double> conclusions(rules.size());

            // iterate over rules and appyl Mamdani fuzzy inference using Product as tnorm
            for (size_t i = 0; i < rules.size(); ++i) {
                double tNorm = 1;

                for (size_t j = 0; j < states.size(); ++j) {
                    tNorm *= _fuzzySet[j]->getStrength(max[j], rules[i]->getParentStates()[j]->getState());
                }

                // truncate
                int trunc = static_cast<int>(tNorm * 100);
                tNorm = trunc / 100.0;

                conclusions[i] = tNorm;
            }

            // apply inferred values to partial cpt
            size_t nrStates;

            if (_rules->getRules()[0]->getChildState().isBinary()) {
                nrStates = 2;
            } else {
                nrStates = 4;
            }

            std::vector<double> inferredBeliefs(nrStates);

            for (size_t i = 0; i < conclusions.size(); ++i) {
                size_t state = rules[i]->getChildState().getState();

                if (conclusions[i] > inferredBeliefs[state]) {
                    inferredBeliefs[state] = conclusions[i];
                }
            }

            for (size_t i = 0; i < inferredBeliefs.size(); ++i) {
                if (inferredBeliefs[i] < _nullBeliefTolerance) {
                    inferredBeliefs[i] = _nullBeliefTolerance;
                }
            }

            // normalize inferred table
            utils::vectorNormalize(inferredBeliefs);

            // return partial cpt
            return inferredBeliefs;
        }

        RuleState::RuleState(size_t state, bool binary) : _state(state), _binary(binary) {}

        RuleState::~RuleState() {}

        bool RuleState::isBinary() const {
            return _binary;
        }

        size_t RuleState::getState() const {
            return _state;
        }

        namespace states {

            RuleState *get(const state::State &state) {
                switch (state) {
                    case state::GOOD: {
                        static std::unique_ptr<RuleState> rule = std::make_unique<RuleState>(state::GOOD);
                        return rule.get();
                    }

                    case state::PROBABLY_GOOD: {
                        static std::unique_ptr<RuleState> rule = std::make_unique<RuleState>(state::PROBABLY_GOOD);
                        return rule.get();
                    }

                    case state::PROBABLY_BAD: {
                        static std::unique_ptr<RuleState> rule = std::make_unique<RuleState>(state::PROBABLY_BAD);
                        return rule.get();
                    }

                    case state::BAD: {
                        static std::unique_ptr<RuleState> rule = std::make_unique<RuleState>(state::BAD);
                        return rule.get();
                    }

                    default:
                        // should never happen
                        BAYESNET_THROWE(Exception::INVALID_RULE_STATE, std::to_string(state));
                }
            }

            RuleState *get(const state::StateBinary &state) {
                switch (state) {
                    case state::TRUE: {
                        static std::unique_ptr<RuleState> rule = std::make_unique<RuleState>(state::TRUE, true);
                        return rule.get();
                    }

                    case state::FALSE: {
                        static std::unique_ptr<RuleState> rule = std::make_unique<RuleState>(state::FALSE, true);
                        return rule.get();
                    }

                    default:
                        // should never happen
                        BAYESNET_THROWE(Exception::INVALID_RULE_STATE, std::to_string(state));
                }
            }
        }
    }
}

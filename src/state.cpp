#include <sstream>

#include <bayesnet/state.h>
#include <bayesnet/exception.h>


namespace bayesNet {

    namespace state {

        size_t fromString(const std::string &state) {
            if (state == "false" || state == "False" || state == "FALSE") {
                return 0;
            }

            if (state == "true" || state == "True" || state == "TRUE") {
                return 1;
            }

            if (state == "good" || state == "Good" || state == "GOOD") {
                return 0;
            }

            if (state == "probably_good" || state == "Probably_Good" || state == "PROBABLY_GOOD") {
                return 1;
            }

            if (state == "probably_bad" || state == "Probably_Bad" || state == "PROBABLY_BAD") {
                return 2;
            }

            if (state == "bad" || state == "Bad" || state == "BAD") {
                return 3;
            }

            BAYESNET_THROWE(UNKNOWN_STATE_VALUE, state);
        }

        BayesBelief::BayesBelief(bool binary) : _binary(binary) {
            if (binary) {
                _beliefs = std::vector<double>(2, 0);
            } else {
                _beliefs = std::vector<double>(4, 0);
            }
        }

        BayesBelief::~BayesBelief() {}

        bool BayesBelief::isBinary() const {
            return _binary;
        }

        size_t BayesBelief::nrStates() const {
            return _beliefs.size();
        }

        void BayesBelief::set(size_t state, double belief) {
            // check if index is in bounds
            if (state >= _beliefs.size()) {
                BAYESNET_THROW(INDEX_OUT_OF_BOUNDS);
            }

            // set belief
            _beliefs[state] = belief;
        }

        double BayesBelief::get(size_t state) const {
            // check if index is in bounds
            if (state >= _beliefs.size()) {
                BAYESNET_THROW(INDEX_OUT_OF_BOUNDS);
            }

            // return belief
            return _beliefs[state];
        }

        std::string BayesBelief::toString() const {
            std::stringstream ss;
            ss << *this;
            return ss.str();
        }

        double &BayesBelief::operator[](size_t index) {
            if (index >= _beliefs.size()) {
                BAYESNET_THROW(INDEX_OUT_OF_BOUNDS);
            }

            return _beliefs[index];
        }

        std::ostream &operator<<(std::ostream &os, const State &state) {
            switch (state) {
                case GOOD: {
                    os << "GOOD";
                    break;
                }

                case PROBABLY_GOOD: {
                    os << "PROBABLY_GOOD";
                    break;
                }

                case PROBABLY_BAD: {
                    os << "PROBABLY_BAD";
                    break;
                }

                case BAD: {
                    os << "BAD";
                    break;
                }
            }

            return os;
        }

        std::ostream &operator<<(std::ostream &os, const StateBinary &state) {
            switch (state) {
                case TRUE: {
                    os << "TRUE";
                    break;
                }

                case FALSE: {
                    os << "FALSE";
                    break;
                }
            }

            return os;
        }

        std::ostream &operator<<(std::ostream &os, const BayesBelief &bayesBelief) {
            os << "{";

            // check if belief is binary
            if (bayesBelief.isBinary()) {
                // iterate over belief states
                for (size_t i = 0; i < 2; ++i) {
                    switch (i) {
                        case TRUE: {
                            os << "TRUE: ";
                            break;
                        }

                        case FALSE: {
                            os << "FALSE: ";
                            break;
                        }
                    }

                    os << bayesBelief.get(i);

                    if (i < 1) {
                        os << "; ";
                    }
                }
            } else {
                // iterate over belief states
                for (size_t i = 0; i < 4; ++i) {
                    switch (i) {
                        case GOOD: {
                            os << "GOOD: ";
                            break;
                        }

                        case PROBABLY_GOOD: {
                            os << "PROBABLY GOOD: ";
                            break;
                        }

                        case PROBABLY_BAD: {
                            os << "PROBABLY BAD: ";
                            break;
                        }

                        case BAD: {
                            os << "BAD: ";
                            break;
                        }
                    }

                    os << bayesBelief.get(i);

                    if (i < 3) {
                        os << "; ";
                    }
                }
            }

            os << "}";

            return os;
        }
    }
}

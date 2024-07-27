#include <wheel/random.hpp>

namespace wheel {

void Random::set_seed(unsigned int seed) {
    generator_.seed(seed);
}

}  // namespace wheel

#include <wheel/id.hpp>

namespace wheel {

ID::ID(const std::string& str) {
    val = detail::id_of(str.c_str(), str.size());
}

}  // namespace wheel

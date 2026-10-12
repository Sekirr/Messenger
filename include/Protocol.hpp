#pragma once

#include <cstddef>
#include <system_error>

namespace protocol {
inline constexpr std::size_t MaxMessageSize = 64 * 1024;
}

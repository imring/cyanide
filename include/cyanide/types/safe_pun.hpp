#ifndef CYANIDE_TYPES_SAFE_PUN_HPP_
#define CYANIDE_TYPES_SAFE_PUN_HPP_

#include <cyanide/misc/defs.hpp>

#include <bit> // std::bit_cast
#include <cstddef>
#include <cstring>     // std::memcpy
#include <type_traits> // std::is_trivially_copyable, std::decay

namespace cyanide::types {

/**
 * @brief Safe type-punning of types of not necessarily the same size.
 *
 * @tparam To Type to cast to.
 * @tparam from_size Size of initial (source) value.
 *
 * @param data Pointer to initial (source) value.
 *
 * @return Casted value
 */
template <typename To, std::size_t from_size = sizeof(To)>
To safe_pun(const cyanide::byte_t *data)
{
    static_assert(sizeof(To) == from_size);
    static_assert(std::is_trivially_copyable_v<To>);

    struct BytesArray {
        cyanide::byte_t arr[sizeof(To)]{};
    };

    BytesArray bytes;

    // Explicitly decay array to pointer to avoid warnings
    std::memcpy(
        static_cast<std::decay_t<decltype(bytes.arr)>>(bytes.arr),
        data,
        from_size);

    return std::bit_cast<To>(bytes);
}

} // namespace cyanide::types

#endif // !CYANIDE_TYPES_SAFE_PUN_HPP_
//
// Created by Kaveh Fayyazi on 8/12/25.
//

#ifndef TEMPO_TYPEHELPERS_H
#define TEMPO_TYPEHELPERS_H

#include <type_traits>

template <class E>
constexpr auto to_u(E e) noexcept -> std::underlying_type_t<E> {
    static_assert(std::is_enum_v<E>, "to_u requires an enum type");
    return static_cast<std::underlying_type_t<E>>(e);
}

#endif //TEMPO_TYPEHELPERS_H

//
// Created by chrku on 07.08.2021.
//

#ifndef LOX_UTILS_H
#define LOX_UTILS_H

// These are for std::visit
template<class... Ts> struct overload : Ts... { using Ts::operator()...; };
template<class... Ts> overload(Ts...) -> overload<Ts...>;

#endif //LOX_UTILS_H

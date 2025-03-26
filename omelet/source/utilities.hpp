#pragma once

namespace omelet
{
template<typename T>
struct Point2D
{
    T x;
    T y;
};

template<typename T>
struct Size2D
{
    T width;
    T height;
};

template<class... Ts>
struct overloaded : Ts...  // NOLINT
{
    using Ts::operator()...;
};
}  // namespace omelet

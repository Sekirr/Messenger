#pragma once
#include <variant>
#include <utility>

template <typename V, typename E>
class Result
{
private:
    std::variant<V, E> result_;
    struct ErrorTag
    {
    };
    struct ValueTag
    {
    };
    Result(ValueTag, V &&value)
        : result_(std::in_place_index<0>, std::move(value)) {};
    Result(ErrorTag, E &&error)
        : result_(std::in_place_index<1>, std::move(error)) {};

public:
    static Result returnValue(V value)
    {
        return Result(ValueTag{}, std::move(value));
    }
    static Result failure(E error)
    {
        return Result(ErrorTag{}, std::move(error));
    }
    V &getValue() &
    {
        return std::get<0>(result_);
    }
    E &getError() &
    {
        return std::get<1>(result_);
    }
    const V &getValue() const &
    {
        return std::get<0>(result_);
    }
    const E &getError() const &
    {
        return std::get<1>(result_);
    }
    // true if don't a error
    bool hasValue() const noexcept
    {
        return result_.index() == 0;
    }
};

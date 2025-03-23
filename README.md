# QuickCheck

A simple QuickCheck implementation for modern C++.

```cpp
import QuickCheck;

auto main() -> int {
    quick::check<ID>("ID has count 10", [](ID const& id) {
        quick::assert(id.length() == 10);
    });

    quick::check<ID>("ID equality is reflexive", [](ID const& id) {
        quick::assert(id == id);
    });

    quick::check<ID>("ID has count 11", [](ID const& id) {
        quick::assert(id.length() == 11);
    });
}
```
```
quick::check property: ID has count 10 -> success
quick::check property: ID equality is reflexive -> success
quick::check property: ID has count 11 -> failure where
    argument 1 was VVae<6<:@n
```


<details>
<summary>Class being tested</summary>
  
```cpp
#include <string>
#include <format>
#include <ranges>

/// A random string ID of length 10.
class ID final {
    std::string id;

public:
    [[nodiscard]] auto length() const -> size_t { return this->id.length(); }

    constexpr ID() {
        for (const auto i : std::views::iota(0, 10))
            id += quick::rng::random<char>(32, 126);
    }

    [[nodiscard]] auto operator == (ID const& other) const noexcept -> bool {
        return this->id == other.id;
    }

    friend struct std::formatter<ID>;
};

template <> struct quick::impl::Arbitrary<ID> {
    static auto arbitrary(quick::rng::RandomNumberGenerator auto& gen) -> ID {
        return ID();
    }
};

template <> struct std::formatter<ID> {
    constexpr auto parse(std::format_parse_context& ctx) const { return ctx.begin(); }

    auto format(const ID& id, std::format_context& ctx) const {
        return std::format_to(ctx.out(), "{}", id.id);
    }
};
```
</details>


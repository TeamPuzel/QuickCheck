// Created by Lua (TeamPuzel) on 23.03.2025.
// Copyright (c) 2025 All rights reserved.
module;
#include <print>
#include <tuple>
#include <concepts>
#include <random>
#include <format>
export module QuickCheck;

export namespace quick::rng {
    template <typename T> concept RandomNumberGenerator = requires(T& a) {
        // Returns a value from a uniform, independent distribution of binary data.
        { a.next() } -> std::same_as<size_t>;
    };

    class SystemRandomNumberGenerator final {
    public:
        [[nodiscard]] auto next(this SystemRandomNumberGenerator& self) -> size_t {
            static std::random_device rd;
            static std::mt19937 gen(rd());
            return gen();
        }

        SystemRandomNumberGenerator() noexcept = default;
    };

    using DefaultRandomNumberGenerator = SystemRandomNumberGenerator;
    auto defaultRandomNumberGenerator() -> DefaultRandomNumberGenerator& {
        static auto rng = DefaultRandomNumberGenerator();
        return rng;
    }

    template <std::integral T, RandomNumberGenerator R = DefaultRandomNumberGenerator> constexpr auto random(
       T lower, T upper, R& rng = defaultRandomNumberGenerator()
    ) noexcept(noexcept(rng.next())) -> T {
        const auto delta = upper - lower;
        return lower + rng.next() % delta;
    }

    template <std::floating_point T, RandomNumberGenerator R = DefaultRandomNumberGenerator> constexpr auto random(
        T lower, T upper, R& rng = defaultRandomNumberGenerator()
    ) noexcept(noexcept(rng.next())) -> T {
        const auto normalized = static_cast<T>(rng.next()) / static_cast<T>(std::numeric_limits<size_t>::max());
        return lower + normalized * (upper - lower);
    }
}

export namespace quick::impl {
    template <typename T> struct Arbitrary final {};
}

template <size_t I, size_t N, typename Fn> constexpr auto constexpr_for(Fn&& body) -> void {
    if constexpr (I < N) {
        body.template operator()<I>();
        constexpr_for<I + 1, N>(std::forward<Fn>(body));
    }
}

export namespace quick {
    template <typename T, typename Gen = rng::DefaultRandomNumberGenerator>
    concept Arbitrary = rng::RandomNumberGenerator<Gen> and requires(T a, Gen& gen) {
        { impl::Arbitrary<T>::arbitrary(gen) } -> std::same_as<T>;
    };

    struct CheckError final: std::exception {
        [[nodiscard]] auto what() const noexcept -> char const* override { return "quickcheck check error"; }
    };

    template <Arbitrary... Values, std::invocable<Values...> Fn> auto check(char const* property, Fn&& body) noexcept -> void {
        constexpr auto count = 1024 * 32;
        bool success = true;

        std::print("quick::check checking property: {}", property);

        for (auto repetition = 0; repetition < count; repetition += 1) {
            std::tuple<Values...> values { impl::Arbitrary<Values>::arbitrary(rng::defaultRandomNumberGenerator())... };

            try {
                std::apply(body, values);
            } catch (CheckError const& e) {
                std::println(" -> failure where");

                constexpr_for<0, sizeof...(Values)>([&]<size_t I> {
                    std::println("    argument {} was {}", I + 1, std::get<I>(values));
                });

                success = false;
                break;
            }
        }
        if (success) { std::println(" -> success"); }
    }

    auto assert(bool condition) throw(CheckError) -> void { if (not condition) throw CheckError(); }
}

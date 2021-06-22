#ifndef GEO_DETAIL_BEZIER_H
#define GEO_DETAIL_BEZIER_H

#include <cmath>

namespace geo {

namespace detail {

template <std::size_t... I>
consteval std::size_t
factorial_impl(std::index_sequence<I...>) noexcept {
  return (1 * ... * (I + 1));
}

template <std::size_t N>
consteval std::size_t
factorial() noexcept {
  return factorial_impl(std::make_index_sequence<N>{});
}

template <std::size_t N, std::size_t K>
consteval std::size_t
binom_coeff() noexcept {
  return factorial<N>() / (factorial<K>() * factorial<N - K>());
}

template <std::size_t N, std::size_t... I>
consteval std::array<std::size_t, N>
init_binom_coeffs_impl(std::index_sequence<I...>) noexcept {
  std::array<std::size_t, N> retval {};
  auto helper = [&](std::size_t i, std::size_t result) {
    retval[i] = result;
  };
  (..., helper(I, binom_coeff<N - 1, I>()));
  return retval;
}

template <std::size_t N>
consteval std::array<std::size_t, N>
init_binom_coeffs() noexcept {
  return init_binom_coeffs_impl<N>(std::make_index_sequence<N>{});
}

template <std::floating_point T, std::size_t... I>
constexpr T
pow_impl(T base, std::index_sequence<I...>) noexcept {
  T result = base;
  [[maybe_unused]] auto pow_helper = [&result, base](T) {
    result *= base;
  };
  (..., pow_helper(I));
  return result;
}

template <std::floating_point T, std::size_t N>
constexpr T
pow(T base) noexcept {
  if constexpr (N == 0) {
    return 1;
  } else {
    return detail::pow_impl(base, std::make_index_sequence<N - 1>());
  }
}

template <std::floating_point T, std::forward_iterator Iter>
struct bernstein {
  template <std::size_t... I>
  static constexpr std::iterator_traits<Iter>::value_type
  evaluateAt(Iter first, T t, std::index_sequence<I...> seq) noexcept {
    return (... + (binom_coeffs[I]
                    * pow<T, seq.size() - I - 1>(T(1.0) - t)
                    * pow<T, I>(t)
                    * *std::next(first, I)));
  }

  inline static constexpr std::array<std::size_t, 4> binom_coeffs = init_binom_coeffs<4>();
};

} // namespace detail

} // namespace geo

#endif
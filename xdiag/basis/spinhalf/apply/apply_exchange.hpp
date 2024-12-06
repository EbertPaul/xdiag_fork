#pragma once

#include <functional>

#include <xdiag/basis/spinhalf/apply/apply_term_offdiag_no_sym.hpp>
#include <xdiag/basis/spinhalf/apply/apply_term_offdiag_sym.hpp>
#include <xdiag/bits/bitops.hpp>
#include <xdiag/common.hpp>

namespace xdiag::basis::spinhalf {

// Exchange term: J/2 (S^+_i S^-_j + S^-_i S^+_j)

template <typename bit_t, typename coeff_t, bool symmetric, class BasisIn,
          class BasisOut, class Fill>
void apply_exchange(Coupling const &cpl, Op const &op, BasisIn &&basis_in,
                    BasisOut &&basis_out, Fill &&fill) try {
  coeff_t J = cpl.scalar().as<coeff_t>();
  int64_t s1 = op[0];
  int64_t s2 = op[1];
  bit_t flipmask = ((bit_t)1 << s1) | ((bit_t)1 << s2);

  // Define actions of op
  auto non_zero_term = [&flipmask](bit_t spins) -> bool {
    return bits::popcnt(spins & flipmask) & 1;
  };
  std::function<std::pair<bit_t, coeff_t>(bit_t)> term_action;

  coeff_t Jhalf = J / 2.0;
  coeff_t Jhalf_conj = xdiag::conj(Jhalf);

  if constexpr (isreal<coeff_t>()) {
    (void)Jhalf_conj;
    term_action = [&flipmask,
                   &Jhalf](bit_t spins) -> std::pair<bit_t, coeff_t> {
      bit_t spins_flip = spins ^ flipmask;
      return {spins_flip, Jhalf};
    };
  } else {
    term_action = [&flipmask, &s1, &Jhalf,
                   &Jhalf_conj](bit_t spins) -> std::pair<bit_t, coeff_t> {
      bit_t spins_flip = spins ^ flipmask;
      return {spins_flip, bits::gbit(spins, s1) ? Jhalf : Jhalf_conj};
    };
  }

  // Dispatch either symmetric of unsymmetric term application
  if constexpr (symmetric) {
    spinhalf::apply_term_offdiag_sym<bit_t, coeff_t>(
        basis_in, basis_out, non_zero_term, term_action, fill);
  } else {
    spinhalf::apply_term_offdiag_no_sym<bit_t, coeff_t>(
        basis_in, basis_out, non_zero_term, term_action, fill);
  }
} catch (Error const &e) {
  XDIAG_RETHROW(e);
}

} // namespace xdiag::basis::spinhalf

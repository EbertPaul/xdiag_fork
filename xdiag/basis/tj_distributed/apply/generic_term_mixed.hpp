#pragma once

#include <vector>

#include <xdiag/bits/bitops.hpp>

namespace xdiag::basis::tj_distributed {

template <typename bit_t, typename coeff_t, class BasisIn, class BasisOut,
          class NonZeroTermUps, class NonZeroTermDns, class TermActionUps,
          class TermActionDns, class Fill>
void generic_term_mixed(BasisIn &&basis_in, BasisOut &&basis_out,
                        NonZeroTermUps &&non_zero_term_ups,
                        NonZeroTermDns &&non_zero_term_dns,
                        TermActionUps &&term_actionups,
                        TermActionDns &&term_actiondns, Fill &&fill) {

  (void)basis_in;
  (void)basis_out;
  (void)non_zero_term_ups;
  (void)non_zero_term_dns;
  (void)term_actionups;
  (void)term_actiondns;
  (void)fill;
}

} // namespace xdiag::basis::tj_distributed

#pragma once

#include <xdiag/basis/tj/apply/apply_exchange.hpp>
#include <xdiag/basis/tj/apply/apply_hopping.hpp>
#include <xdiag/basis/tj/apply/apply_szsz.hpp>
#include <xdiag/basis/tj/apply/apply_number.hpp>
#include <xdiag/basis/tj/apply/apply_raise_lower.hpp>
#include <xdiag/common.hpp>

namespace xdiag::basis::tj {

template <typename bit_t, typename coeff_t, bool symmetric, class BasisIn,
          class BasisOut, class Fill>
void apply_terms(OpSum const &ops, BasisIn const &basis_in,
                 BasisOut const &basis_out, Fill &fill) try {

  for (auto const &[cpl, op] : ops) {
    std::string type = op.type();
    if ((type == "SZSZ") || (type == "TJSZSZ")) {
      apply_szsz<bit_t, coeff_t, symmetric>(cpl, op, basis_in, fill);
    } else if ((type == "NUP") || (type == "NDN")) {
      apply_number<bit_t, coeff_t, symmetric>(cpl, op, basis_in, fill);
    } else if (type == "EXCHANGE") {
      apply_exchange<bit_t, coeff_t, symmetric>(cpl, op, basis_in, fill);
    } else if ((type == "HOPUP") || (type == "HOPDN")) {
      apply_hopping<bit_t, coeff_t, symmetric>(cpl, op, basis_in, fill);
    } else if ((type == "CDAGUP") || (type == "CUP") || (type == "CDAGDN") ||
               (type == "CDN")) {
      apply_raise_lower<bit_t, coeff_t, symmetric>(cpl, op, basis_in, basis_out,
                                                   fill);
    }
  }
} catch (Error const &e) {
  XDIAG_RETHROW(e);
}

} // namespace xdiag::basis::tj

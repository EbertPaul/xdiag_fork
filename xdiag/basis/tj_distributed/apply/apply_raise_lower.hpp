#pragma once

#include <string>

#include <xdiag/basis/tj_distributed/apply/generic_term_dns.hpp>
#include <xdiag/basis/tj_distributed/apply/generic_term_ups.hpp>
#include <xdiag/bits/bitops.hpp>

namespace xdiag::basis::tj_distributed {

template <typename coeff_t, class basis_t>
void apply_raise_lower(Coupling const &cpl, Op const &op,
                       basis_t const &basis_in, const coeff_t *vec_in,
                       basis_t const &basis_out, coeff_t *vec_out) {
  using bit_t = typename basis_t::bit_t;

  coeff_t c = cpl.scalar().as<coeff_t>();
  std::string type = op.type();
  int64_t s = op[0];

  bit_t site_mask = (bit_t)1 << s;
  bit_t fermi_mask = site_mask - 1;

  // Raising operators
  if ((type == "Cdagup") || (type == "Cdagdn")) {

    auto non_zero_term_ups = [&](bit_t ups) -> bool {
      return (ups & site_mask) == 0;
    };
    auto non_zero_term_dns = [&](bit_t dns) -> bool {
      return (dns & site_mask) == 0;
    };
    auto term_action = [&](bit_t spins) -> std::pair<bit_t, coeff_t> {
      bool fermi = bits::popcnt(spins & fermi_mask) & 1;
      return {spins ^ site_mask, fermi ? -c : c};
    };

    if (type == "Cdagup") {
      tj_distributed::generic_term_ups<coeff_t>(
          basis_in, basis_out, non_zero_term_ups, non_zero_term_dns,
          term_action, vec_in, vec_out);
    } else if (type == "Cdagdn") {
      tj_distributed::generic_term_dns<coeff_t, true>(
          basis_in, basis_out, non_zero_term_ups, non_zero_term_dns,
          term_action, vec_in, vec_out);
    }

    // Lowering operators
  } else if ((type == "Cup") || (type == "Cdn")) {

    auto term_action = [&](bit_t spins) -> std::pair<bit_t, coeff_t> {
      bool fermi = bits::popcnt(spins & fermi_mask) & 1;
      return {spins ^ site_mask, fermi ? -c : c};
    };

    if (type == "Cup") {
      auto non_zero_term_ups = [&](bit_t ups) -> bool {
        return (ups & site_mask);
      };
      auto non_zero_term_dns = [&](bit_t dns) -> bool {
        return (dns & site_mask) == 0;
      };
      tj_distributed::generic_term_ups<coeff_t>(
          basis_in, basis_out, non_zero_term_ups, non_zero_term_dns,
          term_action, vec_in, vec_out);
    } else if (type == "Cdn") {
      auto non_zero_term_ups = [&](bit_t ups) -> bool {
        return (ups & site_mask) == 0;
      };
      auto non_zero_term_dns = [&](bit_t dns) -> bool {
        return (dns & site_mask);
      };
      tj_distributed::generic_term_dns<coeff_t, true>(
          basis_in, basis_out, non_zero_term_ups, non_zero_term_dns,
          term_action, vec_in, vec_out);
    }
  }
}
} // namespace xdiag::basis::tj_distributed

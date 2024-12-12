#include "../../catch.hpp"

#include <xdiag/algebra/algebra.hpp>
#include <xdiag/algebra/apply.hpp>
#include <xdiag/algebra/matrix.hpp>
#include <xdiag/algorithms/sparse_diag.hpp>
#include <xdiag/utils/close.hpp>

TEST_CASE("electron_raise_lower", "[electron]") try {
  using namespace xdiag;
  using namespace arma;
  std::vector<std::string> op_strs = {"CDAGUP", "CDAGDN", "CUP", "CDN"};

  for (int n_sites = 1; n_sites < 4; ++n_sites) {
    Log("testing electron anticommutation relations (full matrix): N={}",
        n_sites);

    auto block = Electron(n_sites);
    int64_t D = block.size();

    cx_mat id(D, D, fill::eye);

    for (int i = 0; i < n_sites; ++i) {
      for (int j = 0; j < n_sites; ++j) {

        for (auto op_i_str : op_strs) {
          for (auto op_j_str : op_strs) {
            auto op_i = Op(op_i_str, i);
            auto op_j = Op(op_j_str, j);
            auto op_i_m = matrixC(op_i, block);
            auto op_j_m = matrixC(op_j, block);
            cx_mat anti_comm = op_i_m * op_j_m + op_j_m * op_i_m;

            // Log("{} {}; {} {}", op_i_str, i, op_j_str, j);

            if (((op_i_str == "CDAGUP") && (op_j_str == "CUP")) ||
                ((op_i_str == "CDAGDN") && (op_j_str == "CDN")) ||
                ((op_i_str == "CUP") && (op_j_str == "CDAGUP")) ||
                ((op_i_str == "CDN") && (op_j_str == "CDAGDN"))) {
              if (i == j) {
                REQUIRE(norm(anti_comm - id) < 1e-12);
              } else {
                REQUIRE(norm(anti_comm) < 1e-12);
              }
            } else {
              REQUIRE(norm(anti_comm) < 1e-12);
            }
          }
        }
      }
    }
  }

  for (int n_sites = 2; n_sites < 4; ++n_sites) {

    auto block = Electron(n_sites);
    int64_t D = block.size();

    for (int i = 0; i < n_sites; ++i) {

      // Check whether NUP agrees
      {
        auto op = Op("NUP", i);
        auto m = matrix(op, block);
        auto mi1 = matrix(Op("CDAGUP", i), block);
        auto mi2 = matrix(Op("CUP", i), block);
        auto mm = mi1 * mi2;
        REQUIRE(norm(mm - m) < 1e-12);
      }

      // Check whether NDN agrees
      {
        auto op = Op("NDN", i);
        auto m = matrix(op, block);
        auto mi1 = matrix(Op("CDAGDN", i), block);
        auto mi2 = matrix(Op("CDN", i), block);
        auto mm = mi1 * mi2;
        REQUIRE(norm(mm - m) < 1e-12);
      }

      for (int j = 0; j < n_sites; ++j) {
        if (i != j) {

          // Check whether HOPUP agrees
          {
            auto op = Op("HOPUP", {i, j});
            auto m = matrix(op, block);
            auto m1 = matrix(Op("CDAGUP", i), block);
            auto m2 = matrix(Op("CUP", j), block);
            auto m3 = matrix(Op("CDAGUP", j), block);
            auto m4 = matrix(Op("CUP", i), block);
            auto mm = -m1 * m2 - m3 * m4;
            REQUIRE(norm(mm - m) < 1e-12);
          }

          // Check whether HOPDN agrees
          {
            auto op = Op("HOPDN", {i, j});
            auto m = matrix(op, block);
            auto m1 = matrix(Op("CDAGDN", i), block);
            auto m2 = matrix(Op("CDN", j), block);
            auto m3 = matrix(Op("CDAGDN", j), block);
            auto m4 = matrix(Op("CDN", i), block);
            auto mm = -m1 * m2 - m3 * m4;
            REQUIRE(norm(mm - m) < 1e-12);
          }

          // Check whether SZSZ agrees
          {
            auto op = Op("SZSZ", {i, j});
            auto m = matrix(op, block);

            auto mi1 = matrix(Op("CDAGUP", i), block);
            auto mi2 = matrix(Op("CUP", i), block);
            auto mi3 = matrix(Op("CDAGDN", i), block);
            auto mi4 = matrix(Op("CDN", i), block);
            auto szi = 0.5 * mi1 * mi2 - 0.5 * mi3 * mi4;

            auto mj1 = matrix(Op("CDAGUP", j), block);
            auto mj2 = matrix(Op("CUP", j), block);
            auto mj3 = matrix(Op("CDAGDN", j), block);
            auto mj4 = matrix(Op("CDN", j), block);
            auto szj = 0.5 * mj1 * mj2 - 0.5 * mj3 * mj4;

            auto mm = szi * szj;
            REQUIRE(norm(mm - m) < 1e-12);
          }

          // Check whether EXCHANGE agrees
          {
            auto op = Op("EXCHANGE", {i, j});
            auto m = matrix(op, block);

            auto mi1 = matrix(Op("CDAGUP", i), block);
            auto mi2 = matrix(Op("CUP", i), block);
            auto mi3 = matrix(Op("CDAGDN", i), block);
            auto mi4 = matrix(Op("CDN", i), block);
            auto spi = mi1 * mi4;
            auto smi = mi3 * mi2;

            auto mj1 = matrix(Op("CDAGUP", j), block);
            auto mj2 = matrix(Op("CUP", j), block);
            auto mj3 = matrix(Op("CDAGDN", j), block);
            auto mj4 = matrix(Op("CDN", j), block);
            auto spj = mj1 * mj4;
            auto smj = mj3 * mj2;

            auto mm = 0.5 * spi * smj + 0.5 * smi * spj;
            REQUIRE(norm(mm - m) < 1e-12);
          }
        }
      }
    }
  }
} catch (xdiag::Error const &e) {
  error_trace(e);
}

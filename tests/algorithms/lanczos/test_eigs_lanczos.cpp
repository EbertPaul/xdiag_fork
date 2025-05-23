#include "../../catch.hpp"

#include <iostream>

#include "../../blocks/electron/testcases_electron.hpp"

#include <xdiag/algebra/algebra.hpp>
#include <xdiag/algebra/apply.hpp>
#include <xdiag/algebra/matrix.hpp>
#include <xdiag/algorithms/lanczos/eigs_lanczos.hpp>

#include <xdiag/algebra/isapprox.hpp>

using namespace xdiag;

TEST_CASE("eigs_lanczos", "[lanczos]") {
  using namespace xdiag::testcases::electron;

  OpSum ops;

  int nsites = 6;
  int max_num_eigenvalue = 2;

  printf("lanczos_eigenvector_real test ...\n");
  ops = freefermion_alltoall(nsites);
  ops["U"] = 5.0;

  for (int nup = 2; nup <= nsites / 2; ++nup)
    for (int ndn = 2; ndn <= nsites / 2; ++ndn) {

      // Compute exact evals
      auto block = Electron(nsites, nup, ndn);
      auto H = matrix(ops, block, block);
      arma::vec evals_mat;
      arma::eig_sym(evals_mat, H);

      // Compute evec with Lanczos
      auto res = eigs_lanczos(ops, block, max_num_eigenvalue);

      // Compute energy of eigenvector
      for (int num_eigenvalue = 0; num_eigenvalue < max_num_eigenvalue;
           ++num_eigenvalue) {
        auto v = res.eigenvectors.col(num_eigenvalue);
        // XDIAG_SHOW(norm(v));
        REQUIRE(isapprox(norm(v), 1.0));

        auto Hv = v;
        apply(ops, v, Hv);
        double e = dot(v, Hv);

        // XDIAG_SHOW(num_eigenvalue);
        // XDIAG_SHOW(e);
        // XDIAG_SHOW(evals_mat(num_eigenvalue));
        REQUIRE(isapprox(real(e), evals_mat(num_eigenvalue)));
        REQUIRE(isapprox(imag(e), 0.0));
      }
    }
  printf("Done.\n");

  printf("lanczos_eigenvector_cplx test ...\n");
  ops = freefermion_alltoall_complex_updn(nsites);

  for (int nup = 2; nup <= nsites / 3; ++nup)
    for (int ndn = 2; ndn <= nsites / 3; ++ndn) {

      // Create block and matrix for comparison
      auto block = Electron(nsites, nup, ndn);
      auto H = matrixC(ops, block, block);
      arma::vec evals_mat;
      arma::eig_sym(evals_mat, H);

      // Compute evec with Lanczos
      try {
        auto res = eigs_lanczos(ops, block, max_num_eigenvalue, 1e-12,
                                1000, true);

        // Compute energy of eigenvector
        for (int num_eigenvalue = 0; num_eigenvalue < max_num_eigenvalue;
             ++num_eigenvalue) {
          auto v = res.eigenvectors.col(num_eigenvalue);
          REQUIRE(isapprox(norm(v), 1.0));
          auto Hv = v;
          apply(ops, v, Hv);
          auto e = dotC(v, Hv);
          REQUIRE(isapprox(real(e), evals_mat(num_eigenvalue)));
          REQUIRE(isapprox(imag(e), 0.0));
        }
      } catch (Error e) {
        error_trace(e);
      }
    }
  printf("Done.\n");
}

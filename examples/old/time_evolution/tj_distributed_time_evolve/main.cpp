#include <xdiag/all.hpp>

using namespace xdiag;

void measure_density(int nsites, State const &v) {
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  for (int i = 0; i < nsites; ++i) {
    complex sz = innerC(Bond("Ntot", i), v);
    if (rank == 0) {
      printf("%.6f ", std::real(sz));
    }
  }
  if (rank == 0) {
    printf("\n");
  }
}

int main(int argc, char **argv) try {
  MPI_Init(&argc, &argv);

  int L = 6;
  int W = 4;
  double t = 1.0;
  double J = 0.1;
  double mu_0 = 10;

  int nsites = L * W;
  double precision = 1e-12;

  // Create square lattice t-J model
  OpSum ops;
  for (int x = 0; x < L-1; ++x) {
    for (int y = 0; y < W; ++y) {
      int nx = (x + 1) % L;
      int ny = (y + 1) % W;

      int site = x * W + y;
      int right = nx * W + y;
      int top = x * W + ny;
      ops += Op("Hop", "T", {site, right});
      ops += Op("Exchange", "J", {site, right});
      ops += Op("Hop", "T", {site, top});
      ops += Op("Exchange", "J", {site, top});


      
      if (x < L / 2) {
	Log("x {} y {} site {} t {} r {} +", x, y, site, top, right);
        ops += Op("Ntot", "MUPLUS", site);
      } else {
	Log("x {} y {} site {} t {} r {} -", x, y, site, top, right);
        ops += Op("Ntot", "MUNEG", site);
      }
    }
  }
  ops["T"] = t;
  ops["J"] = J;
  ops["MUPLUS"] = mu_0;
  ops["MUNEG"] = mu_0;

  auto block = tJDistributed(nsites, nsites / 2 - 1, nsites / 2 - 1);

  XDIAG_SHOW(block);

  Log.set_verbosity(2);
  tic();
  auto [e0, v] = eig0(ops, block);
  toc("gs");

  ops["MUPLUS"] = 0;
  ops["MUNEG"] = 0;

  measure_density(nsites, v);

  // Do the time evolution with a step size tau
  double tau = 0.1;
  for (int i = 0; i < 40; ++i) {
    tic();
    v = time_evolve(ops, v, tau, precision);
    toc("time evolve");
    tic();
    measure_density(nsites, v);
    toc("measure");
  }

  MPI_Finalize();
  return EXIT_SUCCESS;
} catch (std::exception const &e) {
  traceback(e);
}

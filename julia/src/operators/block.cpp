#include "hc.hpp"

namespace xdiag::julia {

void define_block(jlcxx::Module &mod) {

  mod.method("cxx_block", [](OpSum const &ops, Spinhalf const &b) {
    JULIA_XDIAG_CALL_RETURN(block(ops, b));
  });
  mod.method("cxx_block", [](OpSum const &ops, tJ const &b) {
    JULIA_XDIAG_CALL_RETURN(block(ops, b));
  });
  mod.method("cxx_block", [](OpSum const &ops, Electron const &b) {
    JULIA_XDIAG_CALL_RETURN(block(ops, b));
  });

}

} // namespace xdiag::julia

#include "blop.hh"
#include "blop_impl.cc"

std::map<std::string, BlasOperation *> BlasOperation::NameLU = {};
std::map<int,         BlasOperation *> BlasOperation::IdLU = {};

BlasOperation * BlasOperation::get(int index) {
  auto it = IdLU.find(index);
  if (it == IdLU.end()) {
    return nullptr;
  }
  return it->second;
}

BlasOperation * BlasOperation::find(std::string name) {
  auto it = NameLU.find(name);
  if (it == NameLU.end()) {
    return nullptr;
  }
  return it->second;
}

BlasOperation::BlasOperation(std::string name, cblasfunc_t func, int id, RetTy ret, std::vector<Arg> argv) :
  cblas_name(name), func(func), id(id), ret(ret), argv(argv)
{
  if (NameLU.count(name) > 0 || IdLU.count(id) > 0) {
    std::cerr << name << " (" << id << "): Function already exists\n";
    throw;
  }

  NameLU.insert(std::make_pair(name, this));
  IdLU.insert(std::make_pair(id, this));
}

void BlasOperation::init() {
  // sequentially assign ids (but they need not be so)
  int id = 0;

  // declare new (on heap) so objects don't go out of scope
  new BlasOperation("cblas_dscal",  &_dscal, id++, RetTy::VOID,   { Arg::N, Arg::ALPHA, Arg::X         });
  new BlasOperation("cblas_daxpy",  &_dscal, id++, RetTy::VOID,   { Arg::N, Arg::ALPHA, Arg::Y, Arg::X });
  new BlasOperation("cblas_ddot",   &_dscal, id++, RetTy::DOUBLE, { Arg::N,             Arg::X, Arg::Y });
  new BlasOperation("cblas_dnrm2",  &_dscal, id++, RetTy::DOUBLE, { Arg::N,             Arg::X         });
  new BlasOperation("cblas_dasum",  &_dscal, id++, RetTy::DOUBLE, { Arg::N,             Arg::X         });
  /// BlasOperation("cblas_idamax", &_dscal, id++, RetTy::INT,    { Arg::N,             Arg::X         });
}

#pragma once

#include <cassert>
#include <vector>

#define cC(A) {bool ok = A; assert(ok && #A);}

namespace realn {
  template<class _Cont, class _Pred>
  auto find_if(const _Cont& cont, _Pred pred) {
    return std::find_if(std::begin(cont), std::end(cont), pred);
  }

  template<class _Cont, class _Pred>
  size_t find_index_if(const _Cont& cont, _Pred pred) {
    return find_if(cont, pred) - std::begin(cont);
  }

  template<class _Cont, class _Pred>
  size_t find_index_if(const _Cont& cont, _Pred pred, size_t defValue) {
    auto it = find_if(cont, pred);
    if (it == std::end(cont))
      return defValue;
    return it - std::begin(cont);
  }
}
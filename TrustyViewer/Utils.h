#pragma once

#include <cassert>
#include <vector>
#include <queue>
#include <list>
#include <mutex>
#include <condition_variable>
#include <memory>
#include <type_traits>

#include <QObject>

#define cC(A) {bool ok = A; assert(ok && #A);}

namespace realn {
  template<class _Cont, class _Type>
  auto find(const _Cont& cont, const _Type& val) {
    return std::find(std::begin(cont), std::end(cont), val);
  }

  template<class _Cont, class _Type>
  size_t find_index(const _Cont& cont, const _Type& val) {
    return std::find(std::begin(cont), std::end(cont), val) - std::begin(cont);
  }

  template<class _Cont, class _Pred>
  auto find_if(const _Cont& cont, _Pred pred) {
    return std::find_if(std::begin(cont), std::end(cont), pred);
  }

  template<class _Cont, class _Pred, class _Value>
  _Value& find_value_if(_Cont& cont, _Value& defValue, _Pred pred) {
    auto it = find_if(cont, pred);
    if (it == std::end(cont))
      return defValue;
    return *it;
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

  template<class _Cont, class _Pred>
  auto sort_cont(_Cont& cont, _Pred pred) {
    return std::sort(std::begin(cont), std::end(cont), pred);
  }

  void qt_delete_later(QObject* ptr);

  template<class _Type, class = std::enable_if_t<std::is_base_of_v<QObject, _Type>>>
  using qt_unique_ptr = std::unique_ptr<_Type, decltype(&qt_delete_later)>;

  template<class _Type, class... _Types, class = std::enable_if_t<std::is_base_of_v<QObject, _Type>>>
  qt_unique_ptr<_Type> qt_make_unique(_Types&&... args) {
    return qt_unique_ptr<_Type>(new _Type(std::forward<_Types>(args)...), &qt_delete_later);
  }
}
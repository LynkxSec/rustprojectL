#include "cxx-juce/bridge/cxx_juce.h"
#include <algorithm>
#include <array>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <exception>
#include <initializer_list>
#include <iterator>
#include <memory>
#include <new>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>
#if __cplusplus >= 201703L
#include <string_view>
#endif
#if __cplusplus >= 202002L
#include <ranges>
#endif

#ifdef __GNUC__
#pragma GCC diagnostic ignored "-Wmissing-declarations"
#pragma GCC diagnostic ignored "-Wshadow"
#ifdef __clang__
#pragma clang diagnostic ignored "-Wdollar-in-identifier-extension"
#endif // __clang__
#endif // __GNUC__

namespace rust {
inline namespace cxxbridge1 {
// #include "rust/cxx.h"

#ifndef CXXBRIDGE1_PANIC
#define CXXBRIDGE1_PANIC
template <typename Exception>
void panic [[noreturn]] (const char *msg);
#endif // CXXBRIDGE1_PANIC

struct unsafe_bitcopy_t;

namespace {
template <typename T>
class impl;
} // namespace

template <typename T>
::std::size_t size_of();
template <typename T>
::std::size_t align_of();

#ifndef CXXBRIDGE1_RUST_STRING
#define CXXBRIDGE1_RUST_STRING
class String final {
public:
  String() noexcept;
  String(const String &) noexcept;
  String(String &&) noexcept;
  ~String() noexcept;

  String(const std::string &);
  String(const char *);
  String(const char *, std::size_t);
  String(const char16_t *);
  String(const char16_t *, std::size_t);
#ifdef __cpp_char8_t
  String(const char8_t *s);
  String(const char8_t *s, std::size_t len);
#endif

  static String lossy(const std::string &) noexcept;
  static String lossy(const char *) noexcept;
  static String lossy(const char *, std::size_t) noexcept;
  static String lossy(const char16_t *) noexcept;
  static String lossy(const char16_t *, std::size_t) noexcept;

  String &operator=(const String &) & noexcept;
  String &operator=(String &&) & noexcept;

  explicit operator std::string() const;

  const char *data() const noexcept;
  std::size_t size() const noexcept;
  std::size_t length() const noexcept;
  bool empty() const noexcept;

  const char *c_str() noexcept;

  std::size_t capacity() const noexcept;
  void reserve(size_t new_cap) noexcept;

  using iterator = char *;
  iterator begin() noexcept;
  iterator end() noexcept;

  using const_iterator = const char *;
  const_iterator begin() const noexcept;
  const_iterator end() const noexcept;
  const_iterator cbegin() const noexcept;
  const_iterator cend() const noexcept;

  bool operator==(const String &) const noexcept;
  bool operator!=(const String &) const noexcept;
  bool operator<(const String &) const noexcept;
  bool operator<=(const String &) const noexcept;
  bool operator>(const String &) const noexcept;
  bool operator>=(const String &) const noexcept;

  void swap(String &) noexcept;

  String(unsafe_bitcopy_t, const String &) noexcept;

private:
  struct lossy_t;
  String(lossy_t, const char *, std::size_t) noexcept;
  String(lossy_t, const char16_t *, std::size_t) noexcept;
  friend void swap(String &lhs, String &rhs) noexcept { lhs.swap(rhs); }

  std::array<std::uintptr_t, 3> repr;
};
#endif // CXXBRIDGE1_RUST_STRING

#ifndef CXXBRIDGE1_RUST_STR
#define CXXBRIDGE1_RUST_STR
class Str final {
public:
  Str() noexcept;
  Str(const String &) noexcept;
  Str(const std::string &);
  Str(const char *);
  Str(const char *, std::size_t);

  Str &operator=(const Str &) & noexcept = default;

  explicit operator std::string() const;
#if __cplusplus >= 201703L
  explicit operator std::string_view() const;
#endif

  const char *data() const noexcept;
  std::size_t size() const noexcept;
  std::size_t length() const noexcept;
  bool empty() const noexcept;

  Str(const Str &) noexcept = default;
  ~Str() noexcept = default;

  using iterator = const char *;
  using const_iterator = const char *;
  const_iterator begin() const noexcept;
  const_iterator end() const noexcept;
  const_iterator cbegin() const noexcept;
  const_iterator cend() const noexcept;

  bool operator==(const Str &) const noexcept;
  bool operator!=(const Str &) const noexcept;
  bool operator<(const Str &) const noexcept;
  bool operator<=(const Str &) const noexcept;
  bool operator>(const Str &) const noexcept;
  bool operator>=(const Str &) const noexcept;

  void swap(Str &) noexcept;

private:
  class uninit;
  Str(uninit) noexcept;
  friend impl<Str>;

  std::array<std::uintptr_t, 2> repr;
};
#endif // CXXBRIDGE1_RUST_STR

#ifndef CXXBRIDGE1_RUST_SLICE
#define CXXBRIDGE1_RUST_SLICE
namespace detail {
template <bool>
struct copy_assignable_if {};

template <>
struct copy_assignable_if<false> {
  copy_assignable_if() noexcept = default;
  copy_assignable_if(const copy_assignable_if &) noexcept = default;
  copy_assignable_if &operator=(const copy_assignable_if &) & noexcept = delete;
  copy_assignable_if &operator=(copy_assignable_if &&) & noexcept = default;
};
} // namespace detail

template <typename T>
class Slice final
    : private detail::copy_assignable_if<std::is_const<T>::value> {
public:
  using value_type = T;

  Slice() noexcept;
  Slice(T *, std::size_t count) noexcept;

  template <typename C>
  explicit Slice(C &c) : Slice(c.data(), c.size()) {}

  Slice &operator=(const Slice<T> &) & noexcept = default;
  Slice &operator=(Slice<T> &&) & noexcept = default;

  T *data() const noexcept;
  std::size_t size() const noexcept;
  std::size_t length() const noexcept;
  bool empty() const noexcept;

  T &operator[](std::size_t n) const noexcept;
  T &at(std::size_t n) const;
  T &front() const noexcept;
  T &back() const noexcept;

  Slice(const Slice<T> &) noexcept = default;
  ~Slice() noexcept = default;

  class iterator;
  iterator begin() const noexcept;
  iterator end() const noexcept;

  void swap(Slice &) noexcept;

private:
  class uninit;
  Slice(uninit) noexcept;
  friend impl<Slice>;
  friend void sliceInit(void *, const void *, std::size_t) noexcept;
  friend void *slicePtr(const void *) noexcept;
  friend std::size_t sliceLen(const void *) noexcept;

  std::array<std::uintptr_t, 2> repr;
};

#ifdef __cpp_deduction_guides
template <typename C>
explicit Slice(C &c)
    -> Slice<std::remove_reference_t<decltype(*std::declval<C>().data())>>;
#endif // __cpp_deduction_guides

template <typename T>
class Slice<T>::iterator final {
public:
#if __cplusplus >= 202002L
  using iterator_category = std::contiguous_iterator_tag;
#else
  using iterator_category = std::random_access_iterator_tag;
#endif
  using value_type = T;
  using difference_type = std::ptrdiff_t;
  using pointer = typename std::add_pointer<T>::type;
  using reference = typename std::add_lvalue_reference<T>::type;

  reference operator*() const noexcept;
  pointer operator->() const noexcept;
  reference operator[](difference_type) const noexcept;

  iterator &operator++() noexcept;
  iterator operator++(int) noexcept;
  iterator &operator--() noexcept;
  iterator operator--(int) noexcept;

  iterator &operator+=(difference_type) noexcept;
  iterator &operator-=(difference_type) noexcept;
  iterator operator+(difference_type) const noexcept;
  friend inline iterator operator+(difference_type lhs, iterator rhs) noexcept {
    return rhs + lhs;
  }
  iterator operator-(difference_type) const noexcept;
  difference_type operator-(const iterator &) const noexcept;

  bool operator==(const iterator &) const noexcept;
  bool operator!=(const iterator &) const noexcept;
  bool operator<(const iterator &) const noexcept;
  bool operator<=(const iterator &) const noexcept;
  bool operator>(const iterator &) const noexcept;
  bool operator>=(const iterator &) const noexcept;

private:
  friend class Slice;
  void *pos;
  std::size_t stride;
};

#if __cplusplus >= 202002L
static_assert(std::ranges::contiguous_range<rust::Slice<const uint8_t>>);
static_assert(std::contiguous_iterator<rust::Slice<const uint8_t>::iterator>);
#endif

template <typename T>
Slice<T>::Slice() noexcept {
  sliceInit(this, reinterpret_cast<void *>(align_of<T>()), 0);
}

template <typename T>
Slice<T>::Slice(T *s, std::size_t count) noexcept {
  assert(s != nullptr || count == 0);
  sliceInit(this,
            s == nullptr && count == 0
                ? reinterpret_cast<void *>(align_of<T>())
                : const_cast<typename std::remove_const<T>::type *>(s),
            count);
}

template <typename T>
T *Slice<T>::data() const noexcept {
  return reinterpret_cast<T *>(slicePtr(this));
}

template <typename T>
std::size_t Slice<T>::size() const noexcept {
  return sliceLen(this);
}

template <typename T>
std::size_t Slice<T>::length() const noexcept {
  return this->size();
}

template <typename T>
bool Slice<T>::empty() const noexcept {
  return this->size() == 0;
}

template <typename T>
T &Slice<T>::operator[](std::size_t n) const noexcept {
  assert(n < this->size());
  auto ptr = static_cast<char *>(slicePtr(this)) + size_of<T>() * n;
  return *reinterpret_cast<T *>(ptr);
}

template <typename T>
T &Slice<T>::at(std::size_t n) const {
  if (n >= this->size()) {
    panic<std::out_of_range>("rust::Slice index out of range");
  }
  return (*this)[n];
}

template <typename T>
T &Slice<T>::front() const noexcept {
  assert(!this->empty());
  return (*this)[0];
}

template <typename T>
T &Slice<T>::back() const noexcept {
  assert(!this->empty());
  return (*this)[this->size() - 1];
}

template <typename T>
typename Slice<T>::iterator::reference
Slice<T>::iterator::operator*() const noexcept {
  return *static_cast<T *>(this->pos);
}

template <typename T>
typename Slice<T>::iterator::pointer
Slice<T>::iterator::operator->() const noexcept {
  return static_cast<T *>(this->pos);
}

template <typename T>
typename Slice<T>::iterator::reference Slice<T>::iterator::operator[](
    typename Slice<T>::iterator::difference_type n) const noexcept {
  auto ptr = static_cast<char *>(this->pos) + this->stride * n;
  return *reinterpret_cast<T *>(ptr);
}

template <typename T>
typename Slice<T>::iterator &Slice<T>::iterator::operator++() noexcept {
  this->pos = static_cast<char *>(this->pos) + this->stride;
  return *this;
}

template <typename T>
typename Slice<T>::iterator Slice<T>::iterator::operator++(int) noexcept {
  auto ret = iterator(*this);
  this->pos = static_cast<char *>(this->pos) + this->stride;
  return ret;
}

template <typename T>
typename Slice<T>::iterator &Slice<T>::iterator::operator--() noexcept {
  this->pos = static_cast<char *>(this->pos) - this->stride;
  return *this;
}

template <typename T>
typename Slice<T>::iterator Slice<T>::iterator::operator--(int) noexcept {
  auto ret = iterator(*this);
  this->pos = static_cast<char *>(this->pos) - this->stride;
  return ret;
}

template <typename T>
typename Slice<T>::iterator &Slice<T>::iterator::operator+=(
    typename Slice<T>::iterator::difference_type n) noexcept {
  this->pos = static_cast<char *>(this->pos) + this->stride * n;
  return *this;
}

template <typename T>
typename Slice<T>::iterator &Slice<T>::iterator::operator-=(
    typename Slice<T>::iterator::difference_type n) noexcept {
  this->pos = static_cast<char *>(this->pos) - this->stride * n;
  return *this;
}

template <typename T>
typename Slice<T>::iterator Slice<T>::iterator::operator+(
    typename Slice<T>::iterator::difference_type n) const noexcept {
  auto ret = iterator(*this);
  ret.pos = static_cast<char *>(this->pos) + this->stride * n;
  return ret;
}

template <typename T>
typename Slice<T>::iterator Slice<T>::iterator::operator-(
    typename Slice<T>::iterator::difference_type n) const noexcept {
  auto ret = iterator(*this);
  ret.pos = static_cast<char *>(this->pos) - this->stride * n;
  return ret;
}

template <typename T>
typename Slice<T>::iterator::difference_type
Slice<T>::iterator::operator-(const iterator &other) const noexcept {
  auto diff = std::distance(static_cast<char *>(other.pos),
                            static_cast<char *>(this->pos));
  return diff / static_cast<typename Slice<T>::iterator::difference_type>(
                    this->stride);
}

template <typename T>
bool Slice<T>::iterator::operator==(const iterator &other) const noexcept {
  return this->pos == other.pos;
}

template <typename T>
bool Slice<T>::iterator::operator!=(const iterator &other) const noexcept {
  return this->pos != other.pos;
}

template <typename T>
bool Slice<T>::iterator::operator<(const iterator &other) const noexcept {
  return this->pos < other.pos;
}

template <typename T>
bool Slice<T>::iterator::operator<=(const iterator &other) const noexcept {
  return this->pos <= other.pos;
}

template <typename T>
bool Slice<T>::iterator::operator>(const iterator &other) const noexcept {
  return this->pos > other.pos;
}

template <typename T>
bool Slice<T>::iterator::operator>=(const iterator &other) const noexcept {
  return this->pos >= other.pos;
}

template <typename T>
typename Slice<T>::iterator Slice<T>::begin() const noexcept {
  iterator it;
  it.pos = slicePtr(this);
  it.stride = size_of<T>();
  return it;
}

template <typename T>
typename Slice<T>::iterator Slice<T>::end() const noexcept {
  iterator it = this->begin();
  it.pos = static_cast<char *>(it.pos) + it.stride * this->size();
  return it;
}

template <typename T>
void Slice<T>::swap(Slice &rhs) noexcept {
  std::swap(*this, rhs);
}
#endif // CXXBRIDGE1_RUST_SLICE

#ifndef CXXBRIDGE1_RUST_BOX
#define CXXBRIDGE1_RUST_BOX
template <typename T>
class Box final {
public:
  using element_type = T;
  using const_pointer =
      typename std::add_pointer<typename std::add_const<T>::type>::type;
  using pointer = typename std::add_pointer<T>::type;

  Box() = delete;
  Box(Box &&) noexcept;
  ~Box() noexcept;

  explicit Box(const T &);
  explicit Box(T &&);

  Box &operator=(Box &&) & noexcept;

  const T *operator->() const noexcept;
  const T &operator*() const noexcept;
  T *operator->() noexcept;
  T &operator*() noexcept;

  template <typename... Fields>
  static Box in_place(Fields &&...);

  void swap(Box &) noexcept;

  static Box from_raw(T *) noexcept;

  T *into_raw() noexcept;

  /* Deprecated */ using value_type = element_type;

private:
  class uninit;
  class allocation;
  Box(uninit) noexcept;
  void drop() noexcept;

  friend void swap(Box &lhs, Box &rhs) noexcept { lhs.swap(rhs); }

  T *ptr;
};

template <typename T>
class Box<T>::uninit {};

template <typename T>
class Box<T>::allocation {
  static T *alloc() noexcept;
  static void dealloc(T *) noexcept;

public:
  allocation() noexcept : ptr(alloc()) {}
  ~allocation() noexcept {
    if (this->ptr) {
      dealloc(this->ptr);
    }
  }
  T *ptr;
};

template <typename T>
Box<T>::Box(Box &&other) noexcept : ptr(other.ptr) {
  other.ptr = nullptr;
}

template <typename T>
Box<T>::Box(const T &val) {
  allocation alloc;
  ::new (alloc.ptr) T(val);
  this->ptr = alloc.ptr;
  alloc.ptr = nullptr;
}

template <typename T>
Box<T>::Box(T &&val) {
  allocation alloc;
  ::new (alloc.ptr) T(std::move(val));
  this->ptr = alloc.ptr;
  alloc.ptr = nullptr;
}

template <typename T>
Box<T>::~Box() noexcept {
  if (this->ptr) {
    this->drop();
  }
}

template <typename T>
Box<T> &Box<T>::operator=(Box &&other) & noexcept {
  if (this->ptr) {
    this->drop();
  }
  this->ptr = other.ptr;
  other.ptr = nullptr;
  return *this;
}

template <typename T>
const T *Box<T>::operator->() const noexcept {
  return this->ptr;
}

template <typename T>
const T &Box<T>::operator*() const noexcept {
  return *this->ptr;
}

template <typename T>
T *Box<T>::operator->() noexcept {
  return this->ptr;
}

template <typename T>
T &Box<T>::operator*() noexcept {
  return *this->ptr;
}

template <typename T>
template <typename... Fields>
Box<T> Box<T>::in_place(Fields &&...fields) {
  allocation alloc;
  auto ptr = alloc.ptr;
  ::new (ptr) T{std::forward<Fields>(fields)...};
  alloc.ptr = nullptr;
  return from_raw(ptr);
}

template <typename T>
void Box<T>::swap(Box &rhs) noexcept {
  using std::swap;
  swap(this->ptr, rhs.ptr);
}

template <typename T>
Box<T> Box<T>::from_raw(T *raw) noexcept {
  Box box = uninit{};
  box.ptr = raw;
  return box;
}

template <typename T>
T *Box<T>::into_raw() noexcept {
  T *raw = this->ptr;
  this->ptr = nullptr;
  return raw;
}

template <typename T>
Box<T>::Box(uninit) noexcept {}
#endif // CXXBRIDGE1_RUST_BOX

#ifndef CXXBRIDGE1_RUST_BITCOPY_T
#define CXXBRIDGE1_RUST_BITCOPY_T
struct unsafe_bitcopy_t final {
  explicit unsafe_bitcopy_t() = default;
};
#endif // CXXBRIDGE1_RUST_BITCOPY_T

#ifndef CXXBRIDGE1_RUST_VEC
#define CXXBRIDGE1_RUST_VEC
template <typename T>
class Vec final {
public:
  using value_type = T;

  Vec() noexcept;
  Vec(std::initializer_list<T>);
  Vec(const Vec &);
  Vec(Vec &&) noexcept;
  ~Vec() noexcept;

  Vec &operator=(Vec &&) & noexcept;
  Vec &operator=(const Vec &) &;

  std::size_t size() const noexcept;
  bool empty() const noexcept;
  const T *data() const noexcept;
  T *data() noexcept;
  std::size_t capacity() const noexcept;

  const T &operator[](std::size_t n) const noexcept;
  const T &at(std::size_t n) const;
  const T &front() const noexcept;
  const T &back() const noexcept;

  T &operator[](std::size_t n) noexcept;
  T &at(std::size_t n);
  T &front() noexcept;
  T &back() noexcept;

  void reserve(std::size_t new_cap);
  void push_back(const T &value);
  void push_back(T &&value);
  template <typename... Args>
  void emplace_back(Args &&...args);
  void truncate(std::size_t len);
  void clear();

  using iterator = typename Slice<T>::iterator;
  iterator begin() noexcept;
  iterator end() noexcept;

  using const_iterator = typename Slice<const T>::iterator;
  const_iterator begin() const noexcept;
  const_iterator end() const noexcept;
  const_iterator cbegin() const noexcept;
  const_iterator cend() const noexcept;

  void swap(Vec &) noexcept;

  Vec(unsafe_bitcopy_t, const Vec &) noexcept;

private:
  void reserve_total(std::size_t new_cap) noexcept;
  void set_len(std::size_t len) noexcept;
  void drop() noexcept;

  friend void swap(Vec &lhs, Vec &rhs) noexcept { lhs.swap(rhs); }

  std::array<std::uintptr_t, 3> repr;
};

template <typename T>
Vec<T>::Vec(std::initializer_list<T> init) : Vec{} {
  this->reserve_total(init.size());
  std::move(init.begin(), init.end(), std::back_inserter(*this));
}

template <typename T>
Vec<T>::Vec(const Vec &other) : Vec() {
  this->reserve_total(other.size());
  std::copy(other.begin(), other.end(), std::back_inserter(*this));
}

template <typename T>
Vec<T>::Vec(Vec &&other) noexcept : repr(other.repr) {
  new (&other) Vec();
}

template <typename T>
Vec<T>::~Vec() noexcept {
  this->drop();
}

template <typename T>
Vec<T> &Vec<T>::operator=(Vec &&other) & noexcept {
  this->drop();
  this->repr = other.repr;
  new (&other) Vec();
  return *this;
}

template <typename T>
Vec<T> &Vec<T>::operator=(const Vec &other) & {
  if (this != &other) {
    this->drop();
    new (this) Vec(other);
  }
  return *this;
}

template <typename T>
bool Vec<T>::empty() const noexcept {
  return this->size() == 0;
}

template <typename T>
T *Vec<T>::data() noexcept {
  return const_cast<T *>(const_cast<const Vec<T> *>(this)->data());
}

template <typename T>
const T &Vec<T>::operator[](std::size_t n) const noexcept {
  assert(n < this->size());
  auto data = reinterpret_cast<const char *>(this->data());
  return *reinterpret_cast<const T *>(data + n * size_of<T>());
}

template <typename T>
const T &Vec<T>::at(std::size_t n) const {
  if (n >= this->size()) {
    panic<std::out_of_range>("rust::Vec index out of range");
  }
  return (*this)[n];
}

template <typename T>
const T &Vec<T>::front() const noexcept {
  assert(!this->empty());
  return (*this)[0];
}

template <typename T>
const T &Vec<T>::back() const noexcept {
  assert(!this->empty());
  return (*this)[this->size() - 1];
}

template <typename T>
T &Vec<T>::operator[](std::size_t n) noexcept {
  assert(n < this->size());
  auto data = reinterpret_cast<char *>(this->data());
  return *reinterpret_cast<T *>(data + n * size_of<T>());
}

template <typename T>
T &Vec<T>::at(std::size_t n) {
  if (n >= this->size()) {
    panic<std::out_of_range>("rust::Vec index out of range");
  }
  return (*this)[n];
}

template <typename T>
T &Vec<T>::front() noexcept {
  assert(!this->empty());
  return (*this)[0];
}

template <typename T>
T &Vec<T>::back() noexcept {
  assert(!this->empty());
  return (*this)[this->size() - 1];
}

template <typename T>
void Vec<T>::reserve(std::size_t new_cap) {
  this->reserve_total(new_cap);
}

template <typename T>
void Vec<T>::push_back(const T &value) {
  this->emplace_back(value);
}

template <typename T>
void Vec<T>::push_back(T &&value) {
  this->emplace_back(std::move(value));
}

template <typename T>
template <typename... Args>
void Vec<T>::emplace_back(Args &&...args) {
  auto size = this->size();
  this->reserve_total(size + 1);
  ::new (reinterpret_cast<T *>(reinterpret_cast<char *>(this->data()) +
                               size * size_of<T>()))
      T(std::forward<Args>(args)...);
  this->set_len(size + 1);
}

template <typename T>
void Vec<T>::clear() {
  this->truncate(0);
}

template <typename T>
typename Vec<T>::iterator Vec<T>::begin() noexcept {
  return Slice<T>(this->data(), this->size()).begin();
}

template <typename T>
typename Vec<T>::iterator Vec<T>::end() noexcept {
  return Slice<T>(this->data(), this->size()).end();
}

template <typename T>
typename Vec<T>::const_iterator Vec<T>::begin() const noexcept {
  return this->cbegin();
}

template <typename T>
typename Vec<T>::const_iterator Vec<T>::end() const noexcept {
  return this->cend();
}

template <typename T>
typename Vec<T>::const_iterator Vec<T>::cbegin() const noexcept {
  return Slice<const T>(this->data(), this->size()).begin();
}

template <typename T>
typename Vec<T>::const_iterator Vec<T>::cend() const noexcept {
  return Slice<const T>(this->data(), this->size()).end();
}

template <typename T>
void Vec<T>::swap(Vec &rhs) noexcept {
  using std::swap;
  swap(this->repr, rhs.repr);
}

template <typename T>
Vec<T>::Vec(unsafe_bitcopy_t, const Vec &bits) noexcept : repr(bits.repr) {}
#endif // CXXBRIDGE1_RUST_VEC

#ifndef CXXBRIDGE1_RUST_OPAQUE
#define CXXBRIDGE1_RUST_OPAQUE
class Opaque {
public:
  Opaque() = delete;
  Opaque(const Opaque &) = delete;
  ~Opaque() = delete;
};
#endif // CXXBRIDGE1_RUST_OPAQUE

#ifndef CXXBRIDGE1_IS_COMPLETE
#define CXXBRIDGE1_IS_COMPLETE
namespace detail {
namespace {
template <typename T, typename = std::size_t>
struct is_complete : std::false_type {};
template <typename T>
struct is_complete<T, decltype(sizeof(T))> : std::true_type {};
} // namespace
} // namespace detail
#endif // CXXBRIDGE1_IS_COMPLETE

#ifndef CXXBRIDGE1_LAYOUT
#define CXXBRIDGE1_LAYOUT
class layout {
  template <typename T>
  friend std::size_t size_of();
  template <typename T>
  friend std::size_t align_of();
  template <typename T>
  static typename std::enable_if<std::is_base_of<Opaque, T>::value,
                                 std::size_t>::type
  do_size_of() {
    return T::layout::size();
  }
  template <typename T>
  static typename std::enable_if<!std::is_base_of<Opaque, T>::value,
                                 std::size_t>::type
  do_size_of() {
    return sizeof(T);
  }
  template <typename T>
  static
      typename std::enable_if<detail::is_complete<T>::value, std::size_t>::type
      size_of() {
    return do_size_of<T>();
  }
  template <typename T>
  static typename std::enable_if<std::is_base_of<Opaque, T>::value,
                                 std::size_t>::type
  do_align_of() {
    return T::layout::align();
  }
  template <typename T>
  static typename std::enable_if<!std::is_base_of<Opaque, T>::value,
                                 std::size_t>::type
  do_align_of() {
    return alignof(T);
  }
  template <typename T>
  static
      typename std::enable_if<detail::is_complete<T>::value, std::size_t>::type
      align_of() {
    return do_align_of<T>();
  }
};

template <typename T>
std::size_t size_of() {
  return layout::size_of<T>();
}

template <typename T>
std::size_t align_of() {
  return layout::align_of<T>();
}
#endif // CXXBRIDGE1_LAYOUT

namespace repr {
struct PtrLen final {
  void *ptr;
  ::std::size_t len;
};
} // namespace repr

namespace detail {
template <typename T, typename = void *>
struct operator_new {
  void *operator()(::std::size_t sz) { return ::operator new(sz); }
};

template <typename T>
struct operator_new<T, decltype(T::operator new(sizeof(T)))> {
  void *operator()(::std::size_t sz) { return T::operator new(sz); }
};

class Fail final {
  ::rust::repr::PtrLen &throw$;
public:
  Fail(::rust::repr::PtrLen &throw$) noexcept : throw$(throw$) {}
  void operator()(char const *) noexcept;
  void operator()(std::string const &) noexcept;
};
} // namespace detail

template <typename T>
union MaybeUninit {
  T value;
  void *operator new(::std::size_t sz) { return detail::operator_new<T>{}(sz); }
  MaybeUninit() {}
  ~MaybeUninit() {}
};

namespace {
template <bool> struct deleter_if {
  template <typename T> void operator()(T *) {}
};
template <> struct deleter_if<true> {
  template <typename T> void operator()(T *ptr) { ptr->~T(); }
};
} // namespace
} // namespace cxxbridge1

namespace behavior {
class missing {};
missing trycatch(...);

template <typename Try, typename Fail>
static typename ::std::enable_if<::std::is_same<
    decltype(trycatch(::std::declval<Try>(), ::std::declval<Fail>())),
    missing>::value>::type
trycatch(Try &&func, Fail &&fail) noexcept try {
  func();
} catch (::std::exception const &e) {
  fail(e.what());
}
} // namespace behavior
} // namespace rust

namespace cxx_juce {
  struct BoxedAudioIODeviceCallback;
  struct BoxedAudioIODeviceType;
  struct BoxedAudioIODevice;
  using AudioDeviceSetup = ::cxx_juce::AudioDeviceSetup;
  using AudioDeviceManager = ::cxx_juce::AudioDeviceManager;
  using AudioCallbackWrapper = ::cxx_juce::AudioCallbackWrapper;
}
namespace juce {
  using AudioIODeviceTypeArray = ::juce::AudioIODeviceTypeArray;
  using AudioIODevice = ::juce::AudioIODevice;
  using AudioIODeviceType = ::juce::AudioIODeviceType;
  using AudioSampleBuffer = ::juce::AudioSampleBuffer;
  using SingleThreadedIIRFilter = ::juce::SingleThreadedIIRFilter;
}

namespace cxx_juce {
#ifndef CXXBRIDGE1_STRUCT_cxx_juce$BoxedAudioIODeviceCallback
#define CXXBRIDGE1_STRUCT_cxx_juce$BoxedAudioIODeviceCallback
struct BoxedAudioIODeviceCallback final : public ::rust::Opaque {
  ~BoxedAudioIODeviceCallback() = delete;

private:
  friend ::rust::layout;
  struct layout {
    static ::std::size_t size() noexcept;
    static ::std::size_t align() noexcept;
  };
};
#endif // CXXBRIDGE1_STRUCT_cxx_juce$BoxedAudioIODeviceCallback

#ifndef CXXBRIDGE1_STRUCT_cxx_juce$BoxedAudioIODeviceType
#define CXXBRIDGE1_STRUCT_cxx_juce$BoxedAudioIODeviceType
struct BoxedAudioIODeviceType final : public ::rust::Opaque {
  ~BoxedAudioIODeviceType() = delete;

private:
  friend ::rust::layout;
  struct layout {
    static ::std::size_t size() noexcept;
    static ::std::size_t align() noexcept;
  };
};
#endif // CXXBRIDGE1_STRUCT_cxx_juce$BoxedAudioIODeviceType

#ifndef CXXBRIDGE1_STRUCT_cxx_juce$BoxedAudioIODevice
#define CXXBRIDGE1_STRUCT_cxx_juce$BoxedAudioIODevice
struct BoxedAudioIODevice final : public ::rust::Opaque {
  ~BoxedAudioIODevice() = delete;

private:
  friend ::rust::layout;
  struct layout {
    static ::std::size_t size() noexcept;
    static ::std::size_t align() noexcept;
  };
};
#endif // CXXBRIDGE1_STRUCT_cxx_juce$BoxedAudioIODevice

extern "C" {
::std::size_t cxx_juce$cxxbridge1$194$BoxedAudioIODeviceCallback$operator$sizeof() noexcept;
::std::size_t cxx_juce$cxxbridge1$194$BoxedAudioIODeviceCallback$operator$alignof() noexcept;
} // extern "C"
} // namespace cxx_juce

namespace audio_io_device_callback {
extern "C" {
void audio_io_device_callback$cxxbridge1$194$about_to_start(::cxx_juce::BoxedAudioIODeviceCallback &callback, ::juce::AudioIODevice &device) noexcept;

void audio_io_device_callback$cxxbridge1$194$process_block(::cxx_juce::BoxedAudioIODeviceCallback &callback, ::juce::AudioSampleBuffer const &input, ::juce::AudioSampleBuffer &output) noexcept;

void audio_io_device_callback$cxxbridge1$194$stopped(::cxx_juce::BoxedAudioIODeviceCallback &callback) noexcept;
} // extern "C"
} // namespace audio_io_device_callback

namespace cxx_juce {
extern "C" {
::std::size_t cxx_juce$cxxbridge1$194$BoxedAudioIODeviceType$operator$sizeof() noexcept;
::std::size_t cxx_juce$cxxbridge1$194$BoxedAudioIODeviceType$operator$alignof() noexcept;
} // extern "C"
} // namespace cxx_juce

namespace audio_io_device_type {
extern "C" {
void audio_io_device_type$cxxbridge1$194$name(::cxx_juce::BoxedAudioIODeviceType const &self_, ::rust::String *return$) noexcept;

void audio_io_device_type$cxxbridge1$194$scan_for_devices(::cxx_juce::BoxedAudioIODeviceType &self_) noexcept;

void audio_io_device_type$cxxbridge1$194$get_device_names(::cxx_juce::BoxedAudioIODeviceType const &self_, bool input, ::rust::Vec<::rust::String> *return$) noexcept;

::cxx_juce::BoxedAudioIODevice *audio_io_device_type$cxxbridge1$194$create_device(::cxx_juce::BoxedAudioIODeviceType &self_, ::rust::Str input_device_name, ::rust::Str output_device_name) noexcept;

void audio_io_device_type$cxxbridge1$194$destroy_device(::cxx_juce::BoxedAudioIODevice *self_) noexcept;
} // extern "C"
} // namespace audio_io_device_type

namespace cxx_juce {
extern "C" {
::std::size_t cxx_juce$cxxbridge1$194$BoxedAudioIODevice$operator$sizeof() noexcept;
::std::size_t cxx_juce$cxxbridge1$194$BoxedAudioIODevice$operator$alignof() noexcept;
} // extern "C"
} // namespace cxx_juce

namespace audio_io_device {
extern "C" {
void audio_io_device$cxxbridge1$194$device_name(::cxx_juce::BoxedAudioIODevice const &self_, ::rust::String *return$) noexcept;

void audio_io_device$cxxbridge1$194$device_type_name(::cxx_juce::BoxedAudioIODevice const &self_, ::rust::String *return$) noexcept;

double audio_io_device$cxxbridge1$194$device_sample_rate(::cxx_juce::BoxedAudioIODevice &self_) noexcept;

::std::size_t audio_io_device$cxxbridge1$194$device_buffer_size(::cxx_juce::BoxedAudioIODevice &self_) noexcept;

void audio_io_device$cxxbridge1$194$device_available_sample_rates(::cxx_juce::BoxedAudioIODevice &self_, ::rust::Vec<double> *return$) noexcept;

void audio_io_device$cxxbridge1$194$device_available_buffer_sizes(::cxx_juce::BoxedAudioIODevice &self_, ::rust::Vec<::std::size_t> *return$) noexcept;

void audio_io_device$cxxbridge1$194$device_open(::cxx_juce::BoxedAudioIODevice &self_, double sample_rate, ::std::size_t buffer_size, ::rust::String *return$) noexcept;

void audio_io_device$cxxbridge1$194$device_close(::cxx_juce::BoxedAudioIODevice &self_) noexcept;
} // extern "C"
} // namespace audio_io_device

namespace cxx_juce {
extern "C" {
void cxx_juce$cxxbridge1$194$version(::rust::String *return$) noexcept {
  ::rust::String (*version$)() = ::cxx_juce::juceVersion;
  new (return$) ::rust::String(version$());
}

void cxx_juce$cxxbridge1$194$initialise_juce() noexcept {
  void (*initialise_juce$)() = ::cxx_juce::initialiseJuce;
  initialise_juce$();
}

void cxx_juce$cxxbridge1$194$shutdown_juce() noexcept {
  void (*shutdown_juce$)() = ::cxx_juce::shutdownJuce;
  shutdown_juce$();
}

::std::int32_t cxx_juce$cxxbridge1$194$AudioIODeviceTypeArray$size(::juce::AudioIODeviceTypeArray const &self) noexcept {
  ::std::int32_t (::juce::AudioIODeviceTypeArray::*size$)() const = &::juce::AudioIODeviceTypeArray::size;
  return (self.*size$)();
}

::juce::AudioIODeviceType *cxx_juce$cxxbridge1$194$AudioIODeviceTypeArray$get_unchecked(::juce::AudioIODeviceTypeArray const &self, ::std::int32_t index) noexcept {
  ::juce::AudioIODeviceType *(::juce::AudioIODeviceTypeArray::*get_unchecked$)(::std::int32_t) const = &::juce::AudioIODeviceTypeArray::getUnchecked;
  return (self.*get_unchecked$)(index);
}

::cxx_juce::AudioDeviceSetup *cxx_juce$cxxbridge1$194$create_audio_device_setup() noexcept {
  ::std::unique_ptr<::cxx_juce::AudioDeviceSetup> (*create_audio_device_setup$)() = ::cxx_juce::createAudioDeviceSetup;
  return create_audio_device_setup$().release();
}

void cxx_juce$cxxbridge1$194$AudioDeviceSetup$output_device_name(::cxx_juce::AudioDeviceSetup const &self, ::rust::Str *return$) noexcept {
  ::rust::Str (::cxx_juce::AudioDeviceSetup::*output_device_name$)() const = &::cxx_juce::AudioDeviceSetup::outputDeviceName;
  new (return$) ::rust::Str((self.*output_device_name$)());
}

void cxx_juce$cxxbridge1$194$AudioDeviceSetup$input_device_name(::cxx_juce::AudioDeviceSetup const &self, ::rust::Str *return$) noexcept {
  ::rust::Str (::cxx_juce::AudioDeviceSetup::*input_device_name$)() const = &::cxx_juce::AudioDeviceSetup::inputDeviceName;
  new (return$) ::rust::Str((self.*input_device_name$)());
}

double cxx_juce$cxxbridge1$194$AudioDeviceSetup$sample_rate(::cxx_juce::AudioDeviceSetup const &self) noexcept {
  double (::cxx_juce::AudioDeviceSetup::*sample_rate$)() const = &::cxx_juce::AudioDeviceSetup::sampleRate;
  return (self.*sample_rate$)();
}

::std::int32_t cxx_juce$cxxbridge1$194$AudioDeviceSetup$buffer_size(::cxx_juce::AudioDeviceSetup const &self) noexcept {
  ::std::int32_t (::cxx_juce::AudioDeviceSetup::*buffer_size$)() const = &::cxx_juce::AudioDeviceSetup::bufferSize;
  return (self.*buffer_size$)();
}

void cxx_juce$cxxbridge1$194$AudioDeviceSetup$set_output_device_name(::cxx_juce::AudioDeviceSetup &self, ::rust::Str name) noexcept {
  void (::cxx_juce::AudioDeviceSetup::*set_output_device_name$)(::rust::Str) = &::cxx_juce::AudioDeviceSetup::setOutputDeviceName;
  (self.*set_output_device_name$)(name);
}

void cxx_juce$cxxbridge1$194$AudioDeviceSetup$set_input_device_name(::cxx_juce::AudioDeviceSetup &self, ::rust::Str name) noexcept {
  void (::cxx_juce::AudioDeviceSetup::*set_input_device_name$)(::rust::Str) = &::cxx_juce::AudioDeviceSetup::setInputDeviceName;
  (self.*set_input_device_name$)(name);
}

void cxx_juce$cxxbridge1$194$AudioDeviceSetup$set_sample_rate(::cxx_juce::AudioDeviceSetup &self, double sample_rate) noexcept {
  void (::cxx_juce::AudioDeviceSetup::*set_sample_rate$)(double) = &::cxx_juce::AudioDeviceSetup::setSampleRate;
  (self.*set_sample_rate$)(sample_rate);
}

void cxx_juce$cxxbridge1$194$AudioDeviceSetup$set_buffer_size(::cxx_juce::AudioDeviceSetup &self, ::std::int32_t buffer_size) noexcept {
  void (::cxx_juce::AudioDeviceSetup::*set_buffer_size$)(::std::int32_t) = &::cxx_juce::AudioDeviceSetup::setBufferSize;
  (self.*set_buffer_size$)(buffer_size);
}

::std::int32_t cxx_juce$cxxbridge1$194$AudioDeviceSetup$number_of_input_channels(::cxx_juce::AudioDeviceSetup const &self) noexcept {
  ::std::int32_t (::cxx_juce::AudioDeviceSetup::*number_of_input_channels$)() const = &::cxx_juce::AudioDeviceSetup::numberOfInputChannels;
  return (self.*number_of_input_channels$)();
}

void cxx_juce$cxxbridge1$194$AudioDeviceSetup$set_number_of_input_channels(::cxx_juce::AudioDeviceSetup &self, ::std::int32_t number_of_input_channels) noexcept {
  void (::cxx_juce::AudioDeviceSetup::*set_number_of_input_channels$)(::std::int32_t) = &::cxx_juce::AudioDeviceSetup::setNumberOfInputChannels;
  (self.*set_number_of_input_channels$)(number_of_input_channels);
}

void cxx_juce$cxxbridge1$194$AudioDeviceSetup$use_default_input_channels(::cxx_juce::AudioDeviceSetup &self, bool use_default) noexcept {
  void (::cxx_juce::AudioDeviceSetup::*use_default_input_channels$)(bool) = &::cxx_juce::AudioDeviceSetup::useDefaultInputChannels;
  (self.*use_default_input_channels$)(use_default);
}

bool cxx_juce$cxxbridge1$194$AudioDeviceSetup$using_default_input_channels(::cxx_juce::AudioDeviceSetup const &self) noexcept {
  bool (::cxx_juce::AudioDeviceSetup::*using_default_input_channels$)() const = &::cxx_juce::AudioDeviceSetup::usingDefaultInputChannels;
  return (self.*using_default_input_channels$)();
}

::std::int32_t cxx_juce$cxxbridge1$194$AudioDeviceSetup$number_of_output_channels(::cxx_juce::AudioDeviceSetup const &self) noexcept {
  ::std::int32_t (::cxx_juce::AudioDeviceSetup::*number_of_output_channels$)() const = &::cxx_juce::AudioDeviceSetup::numberOfOutputChannels;
  return (self.*number_of_output_channels$)();
}

void cxx_juce$cxxbridge1$194$AudioDeviceSetup$set_number_of_output_channels(::cxx_juce::AudioDeviceSetup &self, ::std::int32_t number_of_output_channels) noexcept {
  void (::cxx_juce::AudioDeviceSetup::*set_number_of_output_channels$)(::std::int32_t) = &::cxx_juce::AudioDeviceSetup::setNumberOfOutputChannels;
  (self.*set_number_of_output_channels$)(number_of_output_channels);
}

void cxx_juce$cxxbridge1$194$AudioDeviceSetup$use_default_output_channels(::cxx_juce::AudioDeviceSetup &self, bool use_default) noexcept {
  void (::cxx_juce::AudioDeviceSetup::*use_default_output_channels$)(bool) = &::cxx_juce::AudioDeviceSetup::useDefaultOutputChannels;
  (self.*use_default_output_channels$)(use_default);
}

bool cxx_juce$cxxbridge1$194$AudioDeviceSetup$using_default_output_channels(::cxx_juce::AudioDeviceSetup const &self) noexcept {
  bool (::cxx_juce::AudioDeviceSetup::*using_default_output_channels$)() const = &::cxx_juce::AudioDeviceSetup::usingDefaultOutputChannels;
  return (self.*using_default_output_channels$)();
}

::cxx_juce::AudioDeviceManager *cxx_juce$cxxbridge1$194$create_audio_device_manager() noexcept {
  ::std::unique_ptr<::cxx_juce::AudioDeviceManager> (*create_audio_device_manager$)() = ::cxx_juce::createAudioDeviceManager;
  return create_audio_device_manager$().release();
}

::cxx_juce::AudioCallbackWrapper *cxx_juce$cxxbridge1$194$wrap_audio_callback(::cxx_juce::BoxedAudioIODeviceCallback *callback) noexcept {
  ::std::unique_ptr<::cxx_juce::AudioCallbackWrapper> (*wrap_audio_callback$)(::rust::Box<::cxx_juce::BoxedAudioIODeviceCallback>) = ::cxx_juce::wrapAudioCallback;
  return wrap_audio_callback$(::rust::Box<::cxx_juce::BoxedAudioIODeviceCallback>::from_raw(callback)).release();
}

::rust::repr::PtrLen cxx_juce$cxxbridge1$194$AudioDeviceManager$initialise_with_default_devices(::cxx_juce::AudioDeviceManager &self, ::std::int32_t num_input_channels, ::std::int32_t num_output_channels) noexcept {
  void (::cxx_juce::AudioDeviceManager::*initialise_with_default_devices$)(::std::int32_t, ::std::int32_t) = &::cxx_juce::AudioDeviceManager::initialiseWithDefaultDevices;
  ::rust::repr::PtrLen throw$;
  ::rust::behavior::trycatch(
      [&] {
        (self.*initialise_with_default_devices$)(num_input_channels, num_output_channels);
        throw$.ptr = nullptr;
      },
      ::rust::detail::Fail(throw$));
  return throw$;
}

::cxx_juce::AudioDeviceSetup *cxx_juce$cxxbridge1$194$AudioDeviceManager$get_audio_device_setup(::cxx_juce::AudioDeviceManager const &self) noexcept {
  ::std::unique_ptr<::cxx_juce::AudioDeviceSetup> (::cxx_juce::AudioDeviceManager::*get_audio_device_setup$)() const = &::cxx_juce::AudioDeviceManager::getAudioDeviceSetup;
  return (self.*get_audio_device_setup$)().release();
}

void cxx_juce$cxxbridge1$194$AudioDeviceManager$set_audio_device_setup(::cxx_juce::AudioDeviceManager &self, ::cxx_juce::AudioDeviceSetup const &setup) noexcept {
  void (::cxx_juce::AudioDeviceManager::*set_audio_device_setup$)(::cxx_juce::AudioDeviceSetup const &) = &::cxx_juce::AudioDeviceManager::setAudioDeviceSetup;
  (self.*set_audio_device_setup$)(setup);
}

::juce::AudioIODevice *cxx_juce$cxxbridge1$194$AudioDeviceManager$get_current_audio_device(::cxx_juce::AudioDeviceManager const &self) noexcept {
  ::juce::AudioIODevice *(::cxx_juce::AudioDeviceManager::*get_current_audio_device$)() const = &::cxx_juce::AudioDeviceManager::getCurrentAudioDevice;
  return (self.*get_current_audio_device$)();
}

void cxx_juce$cxxbridge1$194$AudioDeviceManager$get_available_device_types(::cxx_juce::AudioDeviceManager &self, ::juce::AudioIODeviceTypeArray const **return$) noexcept {
  ::juce::AudioIODeviceTypeArray const &(::cxx_juce::AudioDeviceManager::*get_available_device_types$)() = &::cxx_juce::AudioDeviceManager::getAvailableDeviceTypes;
  new (return$) ::juce::AudioIODeviceTypeArray const *(&(self.*get_available_device_types$)());
}

::juce::AudioIODeviceType *cxx_juce$cxxbridge1$194$AudioDeviceManager$get_current_device_type_object(::cxx_juce::AudioDeviceManager const &self) noexcept {
  ::juce::AudioIODeviceType *(::cxx_juce::AudioDeviceManager::*get_current_device_type_object$)() const = &::cxx_juce::AudioDeviceManager::getCurrentDeviceTypeObject;
  return (self.*get_current_device_type_object$)();
}

void cxx_juce$cxxbridge1$194$AudioDeviceManager$play_test_sound(::cxx_juce::AudioDeviceManager &self) noexcept {
  void (::cxx_juce::AudioDeviceManager::*play_test_sound$)() = &::cxx_juce::AudioDeviceManager::playTestSound;
  (self.*play_test_sound$)();
}

void cxx_juce$cxxbridge1$194$AudioDeviceManager$add_audio_callback(::cxx_juce::AudioDeviceManager &self, ::std::unique_ptr<::cxx_juce::AudioCallbackWrapper> const &callback) noexcept {
  void (::cxx_juce::AudioDeviceManager::*add_audio_callback$)(::std::unique_ptr<::cxx_juce::AudioCallbackWrapper> const &) = &::cxx_juce::AudioDeviceManager::addAudioCallback;
  (self.*add_audio_callback$)(callback);
}

void cxx_juce$cxxbridge1$194$AudioDeviceManager$remove_audio_callback(::cxx_juce::AudioDeviceManager &self, ::std::unique_ptr<::cxx_juce::AudioCallbackWrapper> const &callback) noexcept {
  void (::cxx_juce::AudioDeviceManager::*remove_audio_callback$)(::std::unique_ptr<::cxx_juce::AudioCallbackWrapper> const &) = &::cxx_juce::AudioDeviceManager::removeAudioCallback;
  (self.*remove_audio_callback$)(callback);
}

void cxx_juce$cxxbridge1$194$AudioDeviceManager$add_audio_device_type(::cxx_juce::AudioDeviceManager &self, ::cxx_juce::BoxedAudioIODeviceType *device_type) noexcept {
  void (::cxx_juce::AudioDeviceManager::*add_audio_device_type$)(::rust::Box<::cxx_juce::BoxedAudioIODeviceType>) = &::cxx_juce::AudioDeviceManager::addAudioDeviceType;
  (self.*add_audio_device_type$)(::rust::Box<::cxx_juce::BoxedAudioIODeviceType>::from_raw(device_type));
}

void cxx_juce$cxxbridge1$194$AudioDeviceManager$set_current_audio_device_type(::cxx_juce::AudioDeviceManager &self, ::rust::Str device_type) noexcept {
  void (::cxx_juce::AudioDeviceManager::*set_current_audio_device_type$)(::rust::Str) = &::cxx_juce::AudioDeviceManager::setCurrentAudioDeviceType;
  (self.*set_current_audio_device_type$)(device_type);
}
} // extern "C"

namespace audio_io_device {
extern "C" {
void cxx_juce$audio_io_device$cxxbridge1$194$get_device_name(::juce::AudioIODevice const &self_, ::rust::Str *return$) noexcept {
  ::rust::Str (*get_device_name$)(::juce::AudioIODevice const &) = ::cxx_juce::audio_io_device::getDeviceName;
  new (return$) ::rust::Str(get_device_name$(self_));
}

void cxx_juce$audio_io_device$cxxbridge1$194$get_device_type_name(::juce::AudioIODevice const &self_, ::rust::Str *return$) noexcept {
  ::rust::Str (*get_device_type_name$)(::juce::AudioIODevice const &) = ::cxx_juce::audio_io_device::getDeviceTypeName;
  new (return$) ::rust::Str(get_device_type_name$(self_));
}
} // extern "C"
} // namespace audio_io_device

extern "C" {
double cxx_juce$cxxbridge1$194$AudioIODevice$get_current_sample_rate(::juce::AudioIODevice &self) noexcept {
  double (::juce::AudioIODevice::*get_current_sample_rate$)() = &::juce::AudioIODevice::getCurrentSampleRate;
  return (self.*get_current_sample_rate$)();
}

::std::int32_t cxx_juce$cxxbridge1$194$AudioIODevice$get_current_buffer_size_samples(::juce::AudioIODevice &self) noexcept {
  ::std::int32_t (::juce::AudioIODevice::*get_current_buffer_size_samples$)() = &::juce::AudioIODevice::getCurrentBufferSizeSamples;
  return (self.*get_current_buffer_size_samples$)();
}
} // extern "C"

namespace audio_io_device {
extern "C" {
void cxx_juce$audio_io_device$cxxbridge1$194$get_available_sample_rates(::juce::AudioIODevice &self_, ::rust::Vec<double> *return$) noexcept {
  ::rust::Vec<double> (*get_available_sample_rates$)(::juce::AudioIODevice &) = ::cxx_juce::audio_io_device::getAvailableSampleRates;
  new (return$) ::rust::Vec<double>(get_available_sample_rates$(self_));
}

void cxx_juce$audio_io_device$cxxbridge1$194$get_available_buffer_sizes(::juce::AudioIODevice &self_, ::rust::Vec<::std::size_t> *return$) noexcept {
  ::rust::Vec<::std::size_t> (*get_available_buffer_sizes$)(::juce::AudioIODevice &) = ::cxx_juce::audio_io_device::getAvailableBufferSizes;
  new (return$) ::rust::Vec<::std::size_t>(get_available_buffer_sizes$(self_));
}

::rust::repr::PtrLen cxx_juce$audio_io_device$cxxbridge1$194$open(::juce::AudioIODevice &self_, double sample_rate, ::std::size_t buffer_size) noexcept {
  void (*open$)(::juce::AudioIODevice &, double, ::std::size_t) = ::cxx_juce::audio_io_device::open;
  ::rust::repr::PtrLen throw$;
  ::rust::behavior::trycatch(
      [&] {
        open$(self_, sample_rate, buffer_size);
        throw$.ptr = nullptr;
      },
      ::rust::detail::Fail(throw$));
  return throw$;
}
} // extern "C"
} // namespace audio_io_device

extern "C" {
void cxx_juce$cxxbridge1$194$AudioIODevice$close(::juce::AudioIODevice &self) noexcept {
  void (::juce::AudioIODevice::*close$)() = &::juce::AudioIODevice::close;
  (self.*close$)();
}
} // extern "C"

namespace audio_io_device {
extern "C" {
::std::int32_t cxx_juce$audio_io_device$cxxbridge1$194$count_active_input_channels(::juce::AudioIODevice const &self_) noexcept {
  ::std::int32_t (*count_active_input_channels$)(::juce::AudioIODevice const &) = ::cxx_juce::audio_io_device::countActiveInputChannels;
  return count_active_input_channels$(self_);
}

::std::int32_t cxx_juce$audio_io_device$cxxbridge1$194$count_active_output_channels(::juce::AudioIODevice const &self_) noexcept {
  ::std::int32_t (*count_active_output_channels$)(::juce::AudioIODevice const &) = ::cxx_juce::audio_io_device::countActiveOutputChannels;
  return count_active_output_channels$(self_);
}
} // extern "C"
} // namespace audio_io_device

namespace audio_io_device_type {
extern "C" {
void cxx_juce$audio_io_device_type$cxxbridge1$194$get_type_name(::juce::AudioIODeviceType const &self_, ::rust::String *return$) noexcept {
  ::rust::String (*get_type_name$)(::juce::AudioIODeviceType const &) = ::cxx_juce::audio_io_device_type::getTypeName;
  new (return$) ::rust::String(get_type_name$(self_));
}
} // extern "C"
} // namespace audio_io_device_type

extern "C" {
void cxx_juce$cxxbridge1$194$AudioIODeviceType$scan_for_devices(::juce::AudioIODeviceType &self) noexcept {
  void (::juce::AudioIODeviceType::*scan_for_devices$)() = &::juce::AudioIODeviceType::scanForDevices;
  (self.*scan_for_devices$)();
}
} // extern "C"

namespace audio_io_device_type {
extern "C" {
void cxx_juce$audio_io_device_type$cxxbridge1$194$get_input_device_names(::juce::AudioIODeviceType const &self_, ::rust::Vec<::rust::String> *return$) noexcept {
  ::rust::Vec<::rust::String> (*get_input_device_names$)(::juce::AudioIODeviceType const &) = ::cxx_juce::audio_io_device_type::getInputDeviceNames;
  new (return$) ::rust::Vec<::rust::String>(get_input_device_names$(self_));
}

void cxx_juce$audio_io_device_type$cxxbridge1$194$get_output_device_names(::juce::AudioIODeviceType const &self_, ::rust::Vec<::rust::String> *return$) noexcept {
  ::rust::Vec<::rust::String> (*get_output_device_names$)(::juce::AudioIODeviceType const &) = ::cxx_juce::audio_io_device_type::getOutputDeviceNames;
  new (return$) ::rust::Vec<::rust::String>(get_output_device_names$(self_));
}

::juce::AudioIODevice *cxx_juce$audio_io_device_type$cxxbridge1$194$new_device(::juce::AudioIODeviceType &self_, ::rust::Str input_device_name, ::rust::Str output_device_name) noexcept {
  ::std::unique_ptr<::juce::AudioIODevice> (*new_device$)(::juce::AudioIODeviceType &, ::rust::Str, ::rust::Str) = ::cxx_juce::audio_io_device_type::createDevice;
  return new_device$(self_, input_device_name, output_device_name).release();
}
} // extern "C"
} // namespace audio_io_device_type

extern "C" {
::std::int32_t cxx_juce$cxxbridge1$194$AudioSampleBuffer$get_num_channels(::juce::AudioSampleBuffer const &self) noexcept {
  ::std::int32_t (::juce::AudioSampleBuffer::*get_num_channels$)() const = &::juce::AudioSampleBuffer::getNumChannels;
  return (self.*get_num_channels$)();
}

::std::int32_t cxx_juce$cxxbridge1$194$AudioSampleBuffer$get_num_samples(::juce::AudioSampleBuffer const &self) noexcept {
  ::std::int32_t (::juce::AudioSampleBuffer::*get_num_samples$)() const = &::juce::AudioSampleBuffer::getNumSamples;
  return (self.*get_num_samples$)();
}

float const *cxx_juce$cxxbridge1$194$AudioSampleBuffer$get_read_pointer(::juce::AudioSampleBuffer const &self, ::std::int32_t channel) noexcept {
  float const *(::juce::AudioSampleBuffer::*get_read_pointer$)(::std::int32_t) const = &::juce::AudioSampleBuffer::getReadPointer;
  return (self.*get_read_pointer$)(channel);
}

float *cxx_juce$cxxbridge1$194$AudioSampleBuffer$get_write_pointer(::juce::AudioSampleBuffer &self, ::std::int32_t channel) noexcept {
  float *(::juce::AudioSampleBuffer::*get_write_pointer$)(::std::int32_t) = &::juce::AudioSampleBuffer::getWritePointer;
  return (self.*get_write_pointer$)(channel);
}

void cxx_juce$cxxbridge1$194$AudioSampleBuffer$clear(::juce::AudioSampleBuffer &self) noexcept {
  void (::juce::AudioSampleBuffer::*clear$)() = &::juce::AudioSampleBuffer::clear;
  (self.*clear$)();
}
} // extern "C"

namespace system_audio_volume {
extern "C" {
void cxx_juce$system_audio_volume$cxxbridge1$194$set_muted(bool muted) noexcept {
  void (*set_muted$)(bool) = ::cxx_juce::system_audio_volume::setMuted;
  set_muted$(muted);
}

bool cxx_juce$system_audio_volume$cxxbridge1$194$is_muted() noexcept {
  bool (*is_muted$)() = ::cxx_juce::system_audio_volume::isMuted;
  return is_muted$();
}

void cxx_juce$system_audio_volume$cxxbridge1$194$set_gain(float gain) noexcept {
  void (*set_gain$)(float) = ::cxx_juce::system_audio_volume::setGain;
  set_gain$(gain);
}

float cxx_juce$system_audio_volume$cxxbridge1$194$get_gain() noexcept {
  float (*get_gain$)() = ::cxx_juce::system_audio_volume::getGain;
  return get_gain$();
}
} // extern "C"
} // namespace system_audio_volume

namespace iir_filter {
extern "C" {
::juce::SingleThreadedIIRFilter *cxx_juce$iir_filter$cxxbridge1$194$create_iir_filter(::std::array<float, 5> *coefficients) noexcept {
  ::std::unique_ptr<::juce::SingleThreadedIIRFilter> (*create_iir_filter$)(::std::array<float, 5>) = ::cxx_juce::iir_filter::createIIRFilter;
  return create_iir_filter$(::std::move(*coefficients)).release();
}
} // extern "C"
} // namespace iir_filter
} // namespace cxx_juce

namespace juce {
extern "C" {
void juce$cxxbridge1$194$SingleThreadedIIRFilter$process_samples(::juce::SingleThreadedIIRFilter &self, float *samples, ::std::int32_t num_samples) noexcept {
  void (::juce::SingleThreadedIIRFilter::*process_samples$)(float *, ::std::int32_t) = &::juce::SingleThreadedIIRFilter::processSamples;
  (self.*process_samples$)(samples, num_samples);
}
} // extern "C"
} // namespace juce

namespace cxx_juce {
namespace iir_filter {
extern "C" {
void cxx_juce$iir_filter$cxxbridge1$194$make_low_pass(double sample_rate, double frequency, double q, ::std::array<float, 5> *return$) noexcept {
  ::std::array<float, 5> (*make_low_pass$)(double, double, double) = ::cxx_juce::iir_filter::makeLowPass;
  new (return$) ::std::array<float, 5>(make_low_pass$(sample_rate, frequency, q));
}

void cxx_juce$iir_filter$cxxbridge1$194$make_high_pass(double sample_rate, double frequency, double q, ::std::array<float, 5> *return$) noexcept {
  ::std::array<float, 5> (*make_high_pass$)(double, double, double) = ::cxx_juce::iir_filter::makeHighPass;
  new (return$) ::std::array<float, 5>(make_high_pass$(sample_rate, frequency, q));
}

void cxx_juce$iir_filter$cxxbridge1$194$make_notch_filter(double sample_rate, double frequency, double q, ::std::array<float, 5> *return$) noexcept {
  ::std::array<float, 5> (*make_notch_filter$)(double, double, double) = ::cxx_juce::iir_filter::makeNotchFilter;
  new (return$) ::std::array<float, 5>(make_notch_filter$(sample_rate, frequency, q));
}
} // extern "C"
} // namespace iir_filter

::std::size_t BoxedAudioIODeviceCallback::layout::size() noexcept {
  return cxx_juce$cxxbridge1$194$BoxedAudioIODeviceCallback$operator$sizeof();
}

::std::size_t BoxedAudioIODeviceCallback::layout::align() noexcept {
  return cxx_juce$cxxbridge1$194$BoxedAudioIODeviceCallback$operator$alignof();
}
} // namespace cxx_juce

namespace audio_io_device_callback {
void aboutToStart(::cxx_juce::BoxedAudioIODeviceCallback &callback, ::juce::AudioIODevice &device) noexcept {
  audio_io_device_callback$cxxbridge1$194$about_to_start(callback, device);
}

void processBlock(::cxx_juce::BoxedAudioIODeviceCallback &callback, ::juce::AudioSampleBuffer const &input, ::juce::AudioSampleBuffer &output) noexcept {
  audio_io_device_callback$cxxbridge1$194$process_block(callback, input, output);
}

void stopped(::cxx_juce::BoxedAudioIODeviceCallback &callback) noexcept {
  audio_io_device_callback$cxxbridge1$194$stopped(callback);
}
} // namespace audio_io_device_callback

namespace cxx_juce {
::std::size_t BoxedAudioIODeviceType::layout::size() noexcept {
  return cxx_juce$cxxbridge1$194$BoxedAudioIODeviceType$operator$sizeof();
}

::std::size_t BoxedAudioIODeviceType::layout::align() noexcept {
  return cxx_juce$cxxbridge1$194$BoxedAudioIODeviceType$operator$alignof();
}
} // namespace cxx_juce

namespace audio_io_device_type {
::rust::String name(::cxx_juce::BoxedAudioIODeviceType const &self_) noexcept {
  ::rust::MaybeUninit<::rust::String> return$;
  audio_io_device_type$cxxbridge1$194$name(self_, &return$.value);
  return ::std::move(return$.value);
}

void scanForDevices(::cxx_juce::BoxedAudioIODeviceType &self_) noexcept {
  audio_io_device_type$cxxbridge1$194$scan_for_devices(self_);
}

::rust::Vec<::rust::String> getDeviceNames(::cxx_juce::BoxedAudioIODeviceType const &self_, bool input) noexcept {
  ::rust::MaybeUninit<::rust::Vec<::rust::String>> return$;
  audio_io_device_type$cxxbridge1$194$get_device_names(self_, input, &return$.value);
  return ::std::move(return$.value);
}

::cxx_juce::BoxedAudioIODevice *createDevice(::cxx_juce::BoxedAudioIODeviceType &self_, ::rust::Str input_device_name, ::rust::Str output_device_name) noexcept {
  return audio_io_device_type$cxxbridge1$194$create_device(self_, input_device_name, output_device_name);
}

void destroyDevice(::cxx_juce::BoxedAudioIODevice *self_) noexcept {
  audio_io_device_type$cxxbridge1$194$destroy_device(self_);
}
} // namespace audio_io_device_type

namespace cxx_juce {
::std::size_t BoxedAudioIODevice::layout::size() noexcept {
  return cxx_juce$cxxbridge1$194$BoxedAudioIODevice$operator$sizeof();
}

::std::size_t BoxedAudioIODevice::layout::align() noexcept {
  return cxx_juce$cxxbridge1$194$BoxedAudioIODevice$operator$alignof();
}
} // namespace cxx_juce

namespace audio_io_device {
::rust::String deviceName(::cxx_juce::BoxedAudioIODevice const &self_) noexcept {
  ::rust::MaybeUninit<::rust::String> return$;
  audio_io_device$cxxbridge1$194$device_name(self_, &return$.value);
  return ::std::move(return$.value);
}

::rust::String typeName(::cxx_juce::BoxedAudioIODevice const &self_) noexcept {
  ::rust::MaybeUninit<::rust::String> return$;
  audio_io_device$cxxbridge1$194$device_type_name(self_, &return$.value);
  return ::std::move(return$.value);
}

double sampleRate(::cxx_juce::BoxedAudioIODevice &self_) noexcept {
  return audio_io_device$cxxbridge1$194$device_sample_rate(self_);
}

::std::size_t bufferSize(::cxx_juce::BoxedAudioIODevice &self_) noexcept {
  return audio_io_device$cxxbridge1$194$device_buffer_size(self_);
}

::rust::Vec<double> availableSampleRates(::cxx_juce::BoxedAudioIODevice &self_) noexcept {
  ::rust::MaybeUninit<::rust::Vec<double>> return$;
  audio_io_device$cxxbridge1$194$device_available_sample_rates(self_, &return$.value);
  return ::std::move(return$.value);
}

::rust::Vec<::std::size_t> availableBufferSizes(::cxx_juce::BoxedAudioIODevice &self_) noexcept {
  ::rust::MaybeUninit<::rust::Vec<::std::size_t>> return$;
  audio_io_device$cxxbridge1$194$device_available_buffer_sizes(self_, &return$.value);
  return ::std::move(return$.value);
}

::rust::String open(::cxx_juce::BoxedAudioIODevice &self_, double sample_rate, ::std::size_t buffer_size) noexcept {
  ::rust::MaybeUninit<::rust::String> return$;
  audio_io_device$cxxbridge1$194$device_open(self_, sample_rate, buffer_size, &return$.value);
  return ::std::move(return$.value);
}

void close(::cxx_juce::BoxedAudioIODevice &self_) noexcept {
  audio_io_device$cxxbridge1$194$device_close(self_);
}
} // namespace audio_io_device

extern "C" {
static_assert(::rust::detail::is_complete<::std::remove_extent<::cxx_juce::AudioDeviceSetup>::type>::value, "definition of `::cxx_juce::AudioDeviceSetup` is required");
static_assert(sizeof(::std::unique_ptr<::cxx_juce::AudioDeviceSetup>) == sizeof(void *), "");
static_assert(alignof(::std::unique_ptr<::cxx_juce::AudioDeviceSetup>) == alignof(void *), "");
void cxxbridge1$unique_ptr$cxx_juce$AudioDeviceSetup$null(::std::unique_ptr<::cxx_juce::AudioDeviceSetup> *ptr) noexcept {
  ::new (ptr) ::std::unique_ptr<::cxx_juce::AudioDeviceSetup>();
}
void cxxbridge1$unique_ptr$cxx_juce$AudioDeviceSetup$raw(::std::unique_ptr<::cxx_juce::AudioDeviceSetup> *ptr, ::std::unique_ptr<::cxx_juce::AudioDeviceSetup>::pointer raw) noexcept {
  ::new (ptr) ::std::unique_ptr<::cxx_juce::AudioDeviceSetup>(raw);
}
::std::unique_ptr<::cxx_juce::AudioDeviceSetup>::element_type const *cxxbridge1$unique_ptr$cxx_juce$AudioDeviceSetup$get(::std::unique_ptr<::cxx_juce::AudioDeviceSetup> const &ptr) noexcept {
  return ptr.get();
}
::std::unique_ptr<::cxx_juce::AudioDeviceSetup>::pointer cxxbridge1$unique_ptr$cxx_juce$AudioDeviceSetup$release(::std::unique_ptr<::cxx_juce::AudioDeviceSetup> &ptr) noexcept {
  return ptr.release();
}
void cxxbridge1$unique_ptr$cxx_juce$AudioDeviceSetup$drop(::std::unique_ptr<::cxx_juce::AudioDeviceSetup> *ptr) noexcept {
  ::rust::deleter_if<::rust::detail::is_complete<::cxx_juce::AudioDeviceSetup>::value>{}(ptr);
}

static_assert(::rust::detail::is_complete<::std::remove_extent<::cxx_juce::AudioDeviceManager>::type>::value, "definition of `::cxx_juce::AudioDeviceManager` is required");
static_assert(sizeof(::std::unique_ptr<::cxx_juce::AudioDeviceManager>) == sizeof(void *), "");
static_assert(alignof(::std::unique_ptr<::cxx_juce::AudioDeviceManager>) == alignof(void *), "");
void cxxbridge1$unique_ptr$cxx_juce$AudioDeviceManager$null(::std::unique_ptr<::cxx_juce::AudioDeviceManager> *ptr) noexcept {
  ::new (ptr) ::std::unique_ptr<::cxx_juce::AudioDeviceManager>();
}
void cxxbridge1$unique_ptr$cxx_juce$AudioDeviceManager$raw(::std::unique_ptr<::cxx_juce::AudioDeviceManager> *ptr, ::std::unique_ptr<::cxx_juce::AudioDeviceManager>::pointer raw) noexcept {
  ::new (ptr) ::std::unique_ptr<::cxx_juce::AudioDeviceManager>(raw);
}
::std::unique_ptr<::cxx_juce::AudioDeviceManager>::element_type const *cxxbridge1$unique_ptr$cxx_juce$AudioDeviceManager$get(::std::unique_ptr<::cxx_juce::AudioDeviceManager> const &ptr) noexcept {
  return ptr.get();
}
::std::unique_ptr<::cxx_juce::AudioDeviceManager>::pointer cxxbridge1$unique_ptr$cxx_juce$AudioDeviceManager$release(::std::unique_ptr<::cxx_juce::AudioDeviceManager> &ptr) noexcept {
  return ptr.release();
}
void cxxbridge1$unique_ptr$cxx_juce$AudioDeviceManager$drop(::std::unique_ptr<::cxx_juce::AudioDeviceManager> *ptr) noexcept {
  ::rust::deleter_if<::rust::detail::is_complete<::cxx_juce::AudioDeviceManager>::value>{}(ptr);
}

::cxx_juce::BoxedAudioIODeviceCallback *cxxbridge1$box$cxx_juce$BoxedAudioIODeviceCallback$alloc() noexcept;
void cxxbridge1$box$cxx_juce$BoxedAudioIODeviceCallback$dealloc(::cxx_juce::BoxedAudioIODeviceCallback *) noexcept;
void cxxbridge1$box$cxx_juce$BoxedAudioIODeviceCallback$drop(::rust::Box<::cxx_juce::BoxedAudioIODeviceCallback> *ptr) noexcept;

static_assert(::rust::detail::is_complete<::std::remove_extent<::cxx_juce::AudioCallbackWrapper>::type>::value, "definition of `::cxx_juce::AudioCallbackWrapper` is required");
static_assert(sizeof(::std::unique_ptr<::cxx_juce::AudioCallbackWrapper>) == sizeof(void *), "");
static_assert(alignof(::std::unique_ptr<::cxx_juce::AudioCallbackWrapper>) == alignof(void *), "");
void cxxbridge1$unique_ptr$cxx_juce$AudioCallbackWrapper$null(::std::unique_ptr<::cxx_juce::AudioCallbackWrapper> *ptr) noexcept {
  ::new (ptr) ::std::unique_ptr<::cxx_juce::AudioCallbackWrapper>();
}
void cxxbridge1$unique_ptr$cxx_juce$AudioCallbackWrapper$raw(::std::unique_ptr<::cxx_juce::AudioCallbackWrapper> *ptr, ::std::unique_ptr<::cxx_juce::AudioCallbackWrapper>::pointer raw) noexcept {
  ::new (ptr) ::std::unique_ptr<::cxx_juce::AudioCallbackWrapper>(raw);
}
::std::unique_ptr<::cxx_juce::AudioCallbackWrapper>::element_type const *cxxbridge1$unique_ptr$cxx_juce$AudioCallbackWrapper$get(::std::unique_ptr<::cxx_juce::AudioCallbackWrapper> const &ptr) noexcept {
  return ptr.get();
}
::std::unique_ptr<::cxx_juce::AudioCallbackWrapper>::pointer cxxbridge1$unique_ptr$cxx_juce$AudioCallbackWrapper$release(::std::unique_ptr<::cxx_juce::AudioCallbackWrapper> &ptr) noexcept {
  return ptr.release();
}
void cxxbridge1$unique_ptr$cxx_juce$AudioCallbackWrapper$drop(::std::unique_ptr<::cxx_juce::AudioCallbackWrapper> *ptr) noexcept {
  ::rust::deleter_if<::rust::detail::is_complete<::cxx_juce::AudioCallbackWrapper>::value>{}(ptr);
}

::cxx_juce::BoxedAudioIODeviceType *cxxbridge1$box$cxx_juce$BoxedAudioIODeviceType$alloc() noexcept;
void cxxbridge1$box$cxx_juce$BoxedAudioIODeviceType$dealloc(::cxx_juce::BoxedAudioIODeviceType *) noexcept;
void cxxbridge1$box$cxx_juce$BoxedAudioIODeviceType$drop(::rust::Box<::cxx_juce::BoxedAudioIODeviceType> *ptr) noexcept;

static_assert(::rust::detail::is_complete<::std::remove_extent<::juce::AudioIODevice>::type>::value, "definition of `::juce::AudioIODevice` is required");
static_assert(sizeof(::std::unique_ptr<::juce::AudioIODevice>) == sizeof(void *), "");
static_assert(alignof(::std::unique_ptr<::juce::AudioIODevice>) == alignof(void *), "");
void cxxbridge1$unique_ptr$juce$AudioIODevice$null(::std::unique_ptr<::juce::AudioIODevice> *ptr) noexcept {
  ::new (ptr) ::std::unique_ptr<::juce::AudioIODevice>();
}
void cxxbridge1$unique_ptr$juce$AudioIODevice$raw(::std::unique_ptr<::juce::AudioIODevice> *ptr, ::std::unique_ptr<::juce::AudioIODevice>::pointer raw) noexcept {
  ::new (ptr) ::std::unique_ptr<::juce::AudioIODevice>(raw);
}
::std::unique_ptr<::juce::AudioIODevice>::element_type const *cxxbridge1$unique_ptr$juce$AudioIODevice$get(::std::unique_ptr<::juce::AudioIODevice> const &ptr) noexcept {
  return ptr.get();
}
::std::unique_ptr<::juce::AudioIODevice>::pointer cxxbridge1$unique_ptr$juce$AudioIODevice$release(::std::unique_ptr<::juce::AudioIODevice> &ptr) noexcept {
  return ptr.release();
}
void cxxbridge1$unique_ptr$juce$AudioIODevice$drop(::std::unique_ptr<::juce::AudioIODevice> *ptr) noexcept {
  ::rust::deleter_if<::rust::detail::is_complete<::juce::AudioIODevice>::value>{}(ptr);
}

static_assert(::rust::detail::is_complete<::std::remove_extent<::juce::SingleThreadedIIRFilter>::type>::value, "definition of `::juce::SingleThreadedIIRFilter` is required");
static_assert(sizeof(::std::unique_ptr<::juce::SingleThreadedIIRFilter>) == sizeof(void *), "");
static_assert(alignof(::std::unique_ptr<::juce::SingleThreadedIIRFilter>) == alignof(void *), "");
void cxxbridge1$unique_ptr$juce$SingleThreadedIIRFilter$null(::std::unique_ptr<::juce::SingleThreadedIIRFilter> *ptr) noexcept {
  ::new (ptr) ::std::unique_ptr<::juce::SingleThreadedIIRFilter>();
}
void cxxbridge1$unique_ptr$juce$SingleThreadedIIRFilter$raw(::std::unique_ptr<::juce::SingleThreadedIIRFilter> *ptr, ::std::unique_ptr<::juce::SingleThreadedIIRFilter>::pointer raw) noexcept {
  ::new (ptr) ::std::unique_ptr<::juce::SingleThreadedIIRFilter>(raw);
}
::std::unique_ptr<::juce::SingleThreadedIIRFilter>::element_type const *cxxbridge1$unique_ptr$juce$SingleThreadedIIRFilter$get(::std::unique_ptr<::juce::SingleThreadedIIRFilter> const &ptr) noexcept {
  return ptr.get();
}
::std::unique_ptr<::juce::SingleThreadedIIRFilter>::pointer cxxbridge1$unique_ptr$juce$SingleThreadedIIRFilter$release(::std::unique_ptr<::juce::SingleThreadedIIRFilter> &ptr) noexcept {
  return ptr.release();
}
void cxxbridge1$unique_ptr$juce$SingleThreadedIIRFilter$drop(::std::unique_ptr<::juce::SingleThreadedIIRFilter> *ptr) noexcept {
  ::rust::deleter_if<::rust::detail::is_complete<::juce::SingleThreadedIIRFilter>::value>{}(ptr);
}
} // extern "C"

namespace rust {
inline namespace cxxbridge1 {
template <>
::cxx_juce::BoxedAudioIODeviceCallback *Box<::cxx_juce::BoxedAudioIODeviceCallback>::allocation::alloc() noexcept {
  return cxxbridge1$box$cxx_juce$BoxedAudioIODeviceCallback$alloc();
}
template <>
void Box<::cxx_juce::BoxedAudioIODeviceCallback>::allocation::dealloc(::cxx_juce::BoxedAudioIODeviceCallback *ptr) noexcept {
  cxxbridge1$box$cxx_juce$BoxedAudioIODeviceCallback$dealloc(ptr);
}
template <>
void Box<::cxx_juce::BoxedAudioIODeviceCallback>::drop() noexcept {
  cxxbridge1$box$cxx_juce$BoxedAudioIODeviceCallback$drop(this);
}
template <>
::cxx_juce::BoxedAudioIODeviceType *Box<::cxx_juce::BoxedAudioIODeviceType>::allocation::alloc() noexcept {
  return cxxbridge1$box$cxx_juce$BoxedAudioIODeviceType$alloc();
}
template <>
void Box<::cxx_juce::BoxedAudioIODeviceType>::allocation::dealloc(::cxx_juce::BoxedAudioIODeviceType *ptr) noexcept {
  cxxbridge1$box$cxx_juce$BoxedAudioIODeviceType$dealloc(ptr);
}
template <>
void Box<::cxx_juce::BoxedAudioIODeviceType>::drop() noexcept {
  cxxbridge1$box$cxx_juce$BoxedAudioIODeviceType$drop(this);
}
} // namespace cxxbridge1
} // namespace rust

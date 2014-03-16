#ifndef Q_OPERATORS_HPP
#define Q_OPERATORS_HPP

#include <cstdlib>
#include <cstring>
#include <vector>
#include <set>
#include "q_types.hpp"

#ifdef __CUDACC__
#define CUDA_PREFIX_HOST_DEVICE __host__ __device__
#else
#define CUDA_PREFIX_HOST_DEVICE
#endif

namespace QDB {

  template <typename T_IN,
            typename T_OUT>
  struct CONV {
    inline
    CUDA_PREFIX_HOST_DEVICE
    T_OUT operator()(T_IN a) const {
      return static_cast<T_OUT>(a);
    }
  };

  template <FLD_TYPE FT_IN, FLD_TYPE FT_OUT,
            typename T_IN = typename QDB::fldtype_traits<FT_IN>::q_type, typename T_OUT = typename QDB::fldtype_traits<FT_OUT>::q_type >
  struct CONV_FT : public CONV<T_IN, T_OUT> {
  };

  // forward declare popcount
  template <typename T> struct popcount;

  template <typename T>
  struct BITCOUNT {
    inline
    CUDA_PREFIX_HOST_DEVICE
    Q_I1_t operator()(T a) const {
      return popcount<T>(a);
    }
  };

  template <FLD_TYPE FT,
            typename T = typename QDB::fldtype_traits<FT>::q_type >
  struct BITCOUNT_FT : public BITCOUNT<T> {
  };

  template <typename T>
  struct SQRT {
    inline
    CUDA_PREFIX_HOST_DEVICE
    T operator()(T a) const {
      return sqrt(a);
    }
  };

  template <FLD_TYPE FT,
            typename T = typename QDB::fldtype_traits<FT>::q_type >
  struct SQRT_FT : public SQRT<T> {
  };

  template <typename T>
  struct NEGATION {
    inline
    CUDA_PREFIX_HOST_DEVICE
    T operator()(T a) const {
      return -a;
    }
  };

  template <FLD_TYPE FT,
            typename T = typename QDB::fldtype_traits<FT>::q_type >
  struct NEGATION_FT : public NEGATION<T> {
  };

  template <typename T>
  struct COMPLEMENT { // ONES-COMPLEMENT
    inline
    CUDA_PREFIX_HOST_DEVICE
    T operator()(T a) const {
      return ~a;
    }
  };

  template <FLD_TYPE FT,
            typename T = typename QDB::fldtype_traits<FT>::q_type >
  struct COMPLEMENT_FT : public COMPLEMENT<T> {
  };

  template <typename T_IN, typename T_OUT = T_IN>
  struct INCREMENT {
    inline
    CUDA_PREFIX_HOST_DEVICE
    T_OUT operator()(T_IN a) const {
      return T_OUT(a + 1);
    }
  };

  template <FLD_TYPE FT_IN, FLD_TYPE FT_OUT,
            typename T_IN = typename QDB::fldtype_traits<FT_IN>::q_type, typename T_OUT = typename QDB::fldtype_traits<FT_OUT>::q_type >
  struct INCREMENT_FT : public INCREMENT<T_IN, T_OUT> {
  };

  template <typename T_IN, typename T_OUT = T_IN>
  struct DECREMENT {
    inline
    CUDA_PREFIX_HOST_DEVICE
    T_OUT operator()(T_IN a) const {
      return T_OUT(a - 1);
    }
  };

  template <FLD_TYPE FT_IN, FLD_TYPE FT_OUT,
            typename T_IN = typename QDB::fldtype_traits<FT_IN>::q_type, typename T_OUT = typename QDB::fldtype_traits<FT_OUT>::q_type >
  struct DECREMENT_FT : public DECREMENT<T_IN, T_OUT> {
  };

  template <typename T_IN,
            typename T_OUT>
  struct CUM {
    inline
    CUDA_PREFIX_HOST_DEVICE
    CUM()
      : m_sum(0) {
    }
    inline
    CUDA_PREFIX_HOST_DEVICE
    T_OUT operator()(T_IN a) const {
      m_sum += a;
      return m_sum;
    }
  private:
    T_OUT m_sum;
  };

  template <FLD_TYPE FT_IN, FLD_TYPE FT_OUT,
            typename T_IN = typename QDB::fldtype_traits<FT_IN>::q_type, typename T_OUT = typename QDB::fldtype_traits<FT_OUT>::q_type >
  struct CUM_FT : public CUM<T_IN, T_OUT> {
  };

  template <typename T>
  struct SHIFT {
    SHIFT(Q_I8_t n, T newVal)
      : m_n(n),
        m_newVal(newVal) {
    }
  private:
    Q_I8_t m_n;
    T m_newVal;
  };

  template <FLD_TYPE FT,
            typename T = typename QDB::fldtype_traits<FT>::q_type >
  struct SHIFT_FT : public SHIFT<T> {
    SHIFT_FT(Q_I8_t n, T newVal)
      : SHIFT<T>(n, newVal) {
    }
  };

  template <typename T>
  struct INVALID_INDEX {
    inline
    CUDA_PREFIX_HOST_DEVICE
    INVALID_INDEX(T ub)
      : m_ub(ub) {
    }
    inline
    CUDA_PREFIX_HOST_DEVICE
    Q_I1_t operator()(T a) const {
      return ((a < 0) || (a >= m_ub));
    }
    T m_ub;
  };

  template <FLD_TYPE FT,
            typename T = typename QDB::fldtype_traits<FT>::q_type >
  struct INVALID_INDEX_FT : public INVALID_INDEX<T> {
    INVALID_INDEX_FT(T ub)
      : INVALID_INDEX<T>(ub) {
    }
  };

  template <typename T>
  struct IS_ZERO {
    inline
    CUDA_PREFIX_HOST_DEVICE
    bool operator()(T a) const {
      return (a == 0);
    }
  };

  template <FLD_TYPE FT,
            typename T = typename QDB::fldtype_traits<FT>::q_type >
  struct IS_ZERO_FT : public IS_ZERO<T> {
  };

  template <typename T_IN, typename T_OUT>
  struct CONCAT {
    inline
    CUDA_PREFIX_HOST_DEVICE
    T_OUT operator()(T_IN a, T_IN b) const {
      return T_OUT(T_OUT(a) << sizeof(T_IN) | T_OUT(b));
    }
  };

  template <FLD_TYPE FT_IN, FLD_TYPE FT_OUT,
            typename T_IN = typename QDB::fldtype_traits<FT_IN>::q_type, typename T_OUT = typename QDB::fldtype_traits<FT_OUT>::q_type >
  struct CONCAT_FT : public CONCAT<T_IN, T_OUT> {
  };

  template <typename T>
  struct CMP_EQ {
    inline
    CUDA_PREFIX_HOST_DEVICE
    Q_I1_t operator()(T a, T b) const {
      return a == b;
    }
  };

  template <FLD_TYPE FT,
            typename T = typename QDB::fldtype_traits<FT>::q_type >
  struct CMP_EQ_FT : public CMP_EQ<T> {
  };

  template <typename T>
  struct CMP_NE {
    inline
    CUDA_PREFIX_HOST_DEVICE
    Q_I1_t operator()(T a, T b) const {
      return a != b;
    }
  };

  template <FLD_TYPE FT,
            typename T = typename QDB::fldtype_traits<FT>::q_type >
  struct CMP_NE_FT : public CMP_NE<T> {
  };

  template <typename T>
  struct CMP_LT {
    inline
    CUDA_PREFIX_HOST_DEVICE
    Q_I1_t operator()(T a, T b) const {
      return a < b;
    }
  };

  template <FLD_TYPE FT,
            typename T = typename QDB::fldtype_traits<FT>::q_type >
  struct CMP_LT_FT : public CMP_LT<T> {
  };

  template <typename T>
  struct CMP_LE {
    inline
    CUDA_PREFIX_HOST_DEVICE
    Q_I1_t operator()(T a, T b) const {
      return a <= b;
    }
  };

  template <FLD_TYPE FT,
            typename T = typename QDB::fldtype_traits<FT>::q_type >
  struct CMP_LE_FT : public CMP_LE<T> {
  };

  template <typename T>
  struct CMP_GT {
    inline
    CUDA_PREFIX_HOST_DEVICE
    Q_I1_t operator()(T a, T b) const {
      return a > b;
    }
  };

  template <FLD_TYPE FT,
            typename T = typename QDB::fldtype_traits<FT>::q_type >
  struct CMP_GT_FT : public CMP_GT<T> {
  };

  template <typename T>
  struct CMP_GE {
    inline
    CUDA_PREFIX_HOST_DEVICE
    Q_I1_t operator()(T a, T b) const {
      return a >= b;
    }
  };

  template <FLD_TYPE FT,
            typename T = typename QDB::fldtype_traits<FT>::q_type >
  struct CMP_GE_FT : public CMP_GE<T> {
  };

  template <typename T>
  struct AND {
    inline
    CUDA_PREFIX_HOST_DEVICE
    Q_I1_t operator()(T a, T b) const {
      return a && b;
    }
  };

  template <FLD_TYPE FT,
            typename T = typename QDB::fldtype_traits<FT>::q_type >
  struct AND_FT : public AND<T> {
  };

  template <typename T>
  struct OR {
    inline
    CUDA_PREFIX_HOST_DEVICE
    Q_I1_t operator()(T a, T b) const {
      return a || b;
    }
  };

  template <FLD_TYPE FT,
            typename T = typename QDB::fldtype_traits<FT>::q_type >
  struct OR_FT : public OR<T> {
  };

  template <typename T>
  struct ANDNOT {
    inline
    CUDA_PREFIX_HOST_DEVICE
    Q_I1_t operator()(T a, T b) const {
      return a && !b;
    }
  };

  template <FLD_TYPE FT,
            typename T = typename QDB::fldtype_traits<FT>::q_type >
  struct ANDNOT_FT : public ANDNOT<T> {
  };

  template <typename T>
  struct BWISE_AND {
    inline
    CUDA_PREFIX_HOST_DEVICE
    T operator()(T a, T b) const {
      return a & b;
    }
  };

  template <FLD_TYPE FT,
            typename T = typename QDB::fldtype_traits<FT>::q_type >
  struct BWISE_AND_FT : public BWISE_AND<T> {
  };

  template <typename T>
  struct BWISE_OR {
    inline
    CUDA_PREFIX_HOST_DEVICE
    T operator()(T a, T b) const {
      return a | b;
    }
  };

  template <FLD_TYPE FT,
            typename T = typename QDB::fldtype_traits<FT>::q_type >
  struct BWISE_OR_FT : public BWISE_OR<T> {
  };

  template <typename T>
  struct BWISE_XOR {
    inline
    CUDA_PREFIX_HOST_DEVICE
    T operator()(T a, T b) const {
      return a ^ b;
    }
  };

  template <FLD_TYPE FT,
            typename T = typename QDB::fldtype_traits<FT>::q_type >
  struct BWISE_XOR_FT : public BWISE_XOR<T> {
  };

  template <typename T, typename T_OUT = T>
  struct ADD {
    inline
    CUDA_PREFIX_HOST_DEVICE
    T_OUT operator()(T a, T b) const {
      return T_OUT(a + b);
    }
  };

  template <FLD_TYPE FT, FLD_TYPE FT_OUT = FT,
            typename T = typename QDB::fldtype_traits<FT>::q_type, typename T_OUT = typename QDB::fldtype_traits<FT_OUT>::q_type >
  struct ADD_FT : public ADD<T, T_OUT> {
  };

  template <typename T, typename T_OUT = T>
  struct SUB {
    inline
    CUDA_PREFIX_HOST_DEVICE
    T_OUT operator()(T a, T b) const {
      return T_OUT(a - b);
    }
  };

  template <FLD_TYPE FT, FLD_TYPE FT_OUT = FT,
            typename T = typename QDB::fldtype_traits<FT>::q_type, typename T_OUT = typename QDB::fldtype_traits<FT_OUT>::q_type >
  struct SUB_FT : public SUB<T, T_OUT> {
  };

  template <typename T, typename T_OUT = T>
  struct MUL {
    inline
    CUDA_PREFIX_HOST_DEVICE
    T_OUT operator()(T a, T b) const {
      return T_OUT(a * b);
    }
  };

  template <FLD_TYPE FT,
            typename T = typename QDB::fldtype_traits<FT>::q_type>
  struct MUL_FT : public MUL<T> {
  };

  template <typename T, typename T_OUT = T>
  struct DIV {
    inline
    CUDA_PREFIX_HOST_DEVICE
    T_OUT operator()(T a, T b) const {
      return T_OUT(a / b);
    }
  };

  template <FLD_TYPE FT,
            typename T = typename QDB::fldtype_traits<FT>::q_type>
  struct DIV_FT : public DIV<T> {
  };

  template <typename T>
  struct REM {
    inline
    CUDA_PREFIX_HOST_DEVICE
    T operator()(T a, T b) const {
      return a % b;
    }
  };

  template <FLD_TYPE FT,
            typename T = typename QDB::fldtype_traits<FT>::q_type>
  struct REM_FT : public REM<T> {
  };

  template <typename T>
  struct SHIFT_LEFT {
    inline
    CUDA_PREFIX_HOST_DEVICE
    T operator()(T a, Q_I1_t b) const {
      return a << b;
    }
  };

  template <FLD_TYPE FT,
            typename T = typename QDB::fldtype_traits<FT>::q_type>
  struct SHIFT_LEFT_FT : public SHIFT_LEFT<T> {
  };

  template <typename T>
  struct SHIFT_RIGHT {
    inline
    CUDA_PREFIX_HOST_DEVICE
    T operator()(T a, Q_I1_t b) const {
      return a >> b;
    }
  };

  template <FLD_TYPE FT,
            typename T = typename QDB::fldtype_traits<FT>::q_type>
  struct SHIFT_RIGHT_FT : public SHIFT_RIGHT<T> {
  };

  template <typename T, typename T_OUT = T>
  struct FILTER {
    inline
    CUDA_PREFIX_HOST_DEVICE
    T_OUT operator()(T a, T b) const {
      return (a ? b : 0);
    }
  };

  template <typename T, typename T_OUT = T>
  struct MAX {
    inline
    CUDA_PREFIX_HOST_DEVICE
    T_OUT operator()(T a, T b) const {
      return (a >= b ? a : b);
    }
  };

  template <FLD_TYPE FT, FLD_TYPE FT_OUT = FT,
            typename T = typename QDB::fldtype_traits<FT>::q_type, typename T_OUT = typename QDB::fldtype_traits<FT_OUT>::q_type >
  struct MAX_FT : public MAX<T, T_OUT> {
  };

  template <typename T, typename T_OUT = T>
  struct MIN {
    inline
    CUDA_PREFIX_HOST_DEVICE
    T_OUT operator()(T a, T b) const {
      return (a < b ? a : b);
    }
  };

  template <FLD_TYPE FT, FLD_TYPE FT_OUT = FT,
            typename T = typename QDB::fldtype_traits<FT>::q_type, typename T_OUT = typename QDB::fldtype_traits<FT_OUT>::q_type >
  struct MIN_FT : public MIN<T, T_OUT> {
  };

  template <typename T>
  struct SECOND {
    inline
    CUDA_PREFIX_HOST_DEVICE
    T operator()(T a, T b) const {
      return b;
    }
  };

  template <FLD_TYPE FT,
            typename T = typename QDB::fldtype_traits<FT>::q_type>
  struct SECOND_FT : public SECOND<T> {
  };

  template <typename T>
  struct FIRST_PLUS_ONE {
    inline
    CUDA_PREFIX_HOST_DEVICE
    T operator()(T a, T b) const {
      return a + 1;
    }
  };

  template <FLD_TYPE FT,
            typename T = typename QDB::fldtype_traits<FT>::q_type>
  struct FIRST_PLUS_ONE_FT : public FIRST_PLUS_ONE<T> {
  };


  template <typename T>
  struct IN_RANGE {
    IN_RANGE(T lb, T ub)
      : bounds(lb, ub) {
    }
    inline
    Q_I1_t operator()(T val) const {
      return (val >= bounds.first) && (val <= bounds.second);
    }
  private:
    std::pair<T, T> bounds;
  };

  template <typename T>
  struct IN_STRICT_RANGE {
    IN_STRICT_RANGE(T lb, T ub)
      : bounds(lb, ub) {
    }
    inline
    Q_I1_t operator()(T val) const {
      return (val > bounds.first) && (val < bounds.second);
    }
  private:
    std::pair<T, T> bounds;
  };

  template <typename T>
  struct NOT_IN_RANGE : public IN_RANGE<T> {
    NOT_IN_RANGE(T lb, T ub)
      : IN_RANGE<T>(lb, ub) {
    }
    inline
    Q_I1_t operator()(T val) const {
      return !IN_RANGE<T>::operator()(val);
    }
  };

  template <typename T>
  struct NOT_IN_STRICT_RANGE : public IN_STRICT_RANGE<T> {
    NOT_IN_STRICT_RANGE(T lb, T ub)
      : IN_STRICT_RANGE<T>(lb, ub) {
    }
    inline
    Q_I1_t operator()(T val) const {
      return !IN_STRICT_RANGE<T>::operator()(val);
    }
  };

  template <typename T>
  struct IN_SET {
    IN_SET(char const * values)
      : vSet(),
        usSet() {
      size_t sz = std::strlen(values) + 1;
      char * tokens = new char[sz + 1];
      std::strncpy(tokens, values, sz + 1);
      char * pSavePtr;
      char * pVal = strtok_r(tokens, ":", &pSavePtr);
      while (pVal) {
        Q_I8_t val = std::strtoll(pVal, NULL, 10);
        vSet.push_back(static_cast<T>(val));
        pVal = strtok_r(NULL, ":", &pSavePtr);
      }
      delete [] tokens;
      init();
    }
    IN_SET(T const * values, size_t sz)
      : vSet(values, values + sz),
        usSet() {
      init();
    }
    IN_SET(std::vector<T> const & values)
      : vSet(values),
        usSet() {
      init();
    }
    inline
    Q_I1_t operator()(T val) const {
      return ((bUseVector && !vSet.empty() && std::find(vSet.begin(), vSet.end(), val) != vSet.end()) ||
              !bUseVector && (usSet.find(val) != usSet.end()));
    }
  private:
    bool bUseVector;
    std::vector<T> vSet;
    std::set<T> usSet;
    inline
    void init() {
      std::swap(usSet, std::set<T>(vSet.begin(), vSet.end()));
      size_t iSize = usSet.size();
      if (iSize <= 32) {
        bUseVector = true;
        std::swap(vSet, std::vector<T>(usSet.begin(), usSet.end()));
      }
    }
  };

  template <typename T>
  struct NOT_IN_SET : public IN_SET<T> {
    NOT_IN_SET(char const * values)
      : IN_SET<T>(values) {
    }
    NOT_IN_SET(T const * values, size_t sz)
      : IN_SET<T>(values, sz) {
    }
    NOT_IN_SET(std::vector<T> const & values)
      : IN_SET<T>(values) {
    }
    inline
    Q_I1_t operator()(T val) const {
      return !IN_SET<T>::operator()(val);
    }
  };

  static Q_I1_t const s_iPopcountBitCount[256] = {
    0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4,
    1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
    1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
    1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
    3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
    4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8
  };

  template <typename T>
  struct popcount {
    inline
    Q_I1_t operator()(T val) {
      bool negative = (val < 0);
      val = std::abs(val);
      Q_I1_t iCount = 0;
      while (val) {
        iCount += s_iPopcountBitCount[val & 0xFF];
        val >>= 8;
      }
      if (negative) {
        ++iCount;
      }
      return iCount;
    }
  };

  template <>
  inline
  Q_I1_t
  popcount<Q_I4_t>::operator()(Q_I4_t val) {
    return __builtin_popcount(val);
  }

  template <>
  inline
  Q_I1_t
  popcount<Q_I8_t>::operator()(Q_I8_t val) {
    return __builtin_popcountll(val);
  }

}; // namespace QDB

#endif // Q_OPERATORS_HPP

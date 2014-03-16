#ifndef Q_UTILITIES_HPP
#define Q_UTILITIES_HPP

#include <string>
#include "q_types.hpp"

namespace QDB {

  size_t get_sizeof(FLD_TYPE fldtype);
  std::string get_fldtype_string(FLD_TYPE fldtype);
  FLD_TYPE get_fldtype_enum(std::string const & fldtype);

  template <typename qtype>
  struct qtype_traits {
    typedef qtype print_type;
  };

  template<>
  struct qtype_traits<Q_I1_t> {
    typedef Q_I4_t print_type;
  };

  template<>
  struct qtype_traits<Q_U1_t> {
    typedef Q_U4_t print_type;
  };

  template<>
  struct qtype_traits<Q_I2_t> {
    typedef Q_I2_t print_type;
  };

  template<>
  struct qtype_traits<Q_U2_t> {
    typedef Q_U2_t print_type;
  };

  template <FLD_TYPE>
  struct fldtype_traits {
    typedef void q_type;
  };

  template <>
  struct fldtype_traits<I1> {
    typedef Q_I1_t q_type;
    typedef Q_U1_t unsigned_type;
    typedef Q_I4_t print_type;
  };

  template <>
  struct fldtype_traits<I2> {
    typedef Q_I2_t q_type;
    typedef Q_U2_t unsigned_type;
    typedef Q_I4_t print_type;
  };

  template <>
  struct fldtype_traits<I4> {
    typedef Q_I4_t q_type;
    typedef Q_U4_t unsigned_type;
    typedef Q_I4_t print_type;
  };

  template <>
  struct fldtype_traits<I8> {
    typedef Q_I8_t q_type;
    typedef Q_U8_t unsigned_type;
    typedef Q_I8_t print_type;
  };

  template <>
  struct fldtype_traits<F4> {
    typedef Q_F4_t q_type;
    typedef Q_F4_t print_type;
  };

  template <>
  struct fldtype_traits<F8> {
    typedef Q_F8_t q_type;
    typedef Q_F8_t print_type;
  };


  class MmappedData {
  public:
    // size < 0 for reading (exact value determined by actual filesize)
    // size >=0 for writing (to allow for creating 0 length files)
    MmappedData(std::string const & filename, off_t size = -1);
    ~MmappedData();

    inline char * X() {
      return m_X;
    }

    inline char const * X() const {
      return m_X;
    }

    inline size_t nX() const {
      return m_nX;
    }

    inline bool is_ok() const {
      return m_is_ok;
    }

  private:
    MmappedData();
    MmappedData(MmappedData const &);
    MmappedData & operator=(MmappedData const &);

    char * m_X;
    size_t m_nX;
    bool m_is_ok;
  };


}

#endif

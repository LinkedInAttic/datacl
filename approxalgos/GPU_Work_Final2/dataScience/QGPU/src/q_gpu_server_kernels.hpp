#ifndef QGPU_SERVER_KERNELS_H
#define QGPU_SERVER_KERNELS_H

#include <string>
#include <iostream>
#include <iomanip>
#include <iterator>

#include "q_types.hpp"
#include "q_utilities.hpp"
#include "q_operators.hpp"

static size_t const NTHREADS = 512;
static size_t const NBLOCKS = 512;
static size_t const MAX_SHARED_MEM = 48 * 1024;

// source for atomicAdd Q_I2_t
// https://devtalk.nvidia.com/default/topic/495219/how-to-use-atomiccas-to-implement-atomicadd-short-trouble-adapting-programming-guide-example/

template <typename T>
__device__
Q_I2_t
atomicAdd(Q_I2_t * address, T val) {
  unsigned int *base_address = (unsigned int *) ((char *)address - ((size_t)address & 2));
  unsigned int long_val = ((size_t)address & 2) ? ((unsigned int)val << 16) : (unsigned short)val;
  unsigned int long_old = atomicAdd(base_address, long_val);

  if((size_t)address & 2) {
    return (short)(long_old >> 16);
  } else {
    unsigned int overflow = ((long_old & 0xffff) + long_val) & 0xffff0000;
    if (overflow) {
      atomicSub(base_address, overflow);
    }
    return (short)(long_old & 0xffff);
  }
}

template <typename T>
__device__
Q_I8_t
atomicAdd(Q_I8_t * dst, T val) {
  return (Q_I8_t)(atomicAdd((unsigned long long int *)(dst), (unsigned long long int)(Q_I8_t(val))));
}

template <typename T>
__device__
Q_I8_t
atomicInc(Q_I8_t * dst) {
  return (Q_I8_t)(atomicAdd((unsigned long long int *)(dst), 1));
}

__device__
inline double
atomicAdd(double * address, double val) {
  unsigned long long int* address_as_ull = (unsigned long long int*)address;
  unsigned long long int old = *address_as_ull, assumed;
  do {
    assumed = old;
    old = atomicCAS(address_as_ull, assumed,
                    __double_as_longlong(val +
                                         __longlong_as_double(assumed)));
  } while (assumed != old);
  return __longlong_as_double(old);
}


namespace QDB {

  template <typename T>
  int make_d_copy(T ** d_val, T const & h_val) {
    if (cudaMalloc((void **)(d_val), sizeof(T)) != cudaSuccess) {
      whereami(__FILE__, __LINE__);
      return Q_FAIL;
    }
    if (cudaMemcpy((void *)(*d_val), (void const *)(&h_val), sizeof(T), cudaMemcpyHostToDevice) != cudaSuccess) {
      whereami(__FILE__, __LINE__);
      return Q_FAIL;
    }
    return Q_PASS;
  }

  template <typename T>
  int restore_h_copy(T & h_val, T ** d_val) {
    if (cudaMemcpy((void *)(&h_val), (void const *)(*d_val), sizeof(T), cudaMemcpyDeviceToHost) != cudaSuccess) {
      whereami(__FILE__, __LINE__);
      return Q_FAIL;
    }
    if (cudaFree(*d_val) != cudaSuccess) {
      whereami(__FILE__, __LINE__);
      return Q_FAIL;
    }
    *d_val = NULL;
    return Q_PASS;
  }

  template <typename T1, typename T2, typename BinaryFunction>
  __global__
  void
  f1s1opf2_kernel(
                  T1 const * d_f1,
                  T1 s1,
                  T2 * d_f2,
                  size_t numElements,
                  BinaryFunction bin_op
                  ) {
    for (size_t tid = (blockDim.x * blockIdx.x) + threadIdx.x, tid_step = blockDim.x * gridDim.x;
         tid < numElements; tid += tid_step) {
      d_f2[tid] = bin_op(d_f1[tid], s1);
    }
  }

  template <typename T_IN, typename T_OUT, typename BinaryFunction>
  __global__
  void
  f1f2opf3_kernel(
                  T_IN const * d_f1,
                  T_IN const * d_f2,
                  T_OUT * d_f3,
                  size_t numElements,
                  BinaryFunction bin_op
                  ) {
    for (size_t tid = (blockDim.x * blockIdx.x) + threadIdx.x, tid_step = blockDim.x * gridDim.x;
         tid < numElements; tid += tid_step) {
      d_f3[tid] = bin_op(d_f1[tid], d_f2[tid]);
    }
  }

  template<typename T, typename OP>
  __device__
  void
  reduce_data(T * d_data, size_t sz, OP bin_op) {
    // for the reduction to work, the range to be reduced must have size that is a power of 2
    // and the starting split point must be half of the range
    //  sz/2 <= i < sz
    size_t i = 1;
    while ((i << 1) < sz) {
      i = (i << 1);
    }
    if ((i << 1) > sz) {
      // blockDim.x is not a power of 2
      // need special handling for elements between i and blockDim.x to make sure they are valid
      if ((threadIdx.x < i) && ((threadIdx.x + i) < sz)) {
        d_data[threadIdx.x] = bin_op(d_data[threadIdx.x], d_data[threadIdx.x + i]);
      }
      i = (i >> 1);
      __syncthreads();
    }
    // i is a power of two. run the reduction algoirthm without the extra special handling
    while (i) {
      if (threadIdx.x < i) {
        d_data[threadIdx.x] = bin_op(d_data[threadIdx.x], d_data[threadIdx.x + i]);
      }
      i = (i >> 1);
      __syncthreads();
    }
  }

  template<typename T, typename T_RESULT, typename BinaryFunction>
  __global__
  void
  filter_reduce_to_partial_kernel(
                                  T const * d_data, //input data to be reduced
                                  size_t numElements, //number of elements
                                  T_RESULT * d_partial, //array for storing the partial sum for each block (must have size gridDim.x)
                                  Q_I1_t const * d_filter1, // optional filter: only consider i s.t. d_filter1[i] != 0
                                  Q_I1_t const * d_filter2, // optional filter: only consider i s.t. d_filter2[i] != 0
                                  BinaryFunction bin_op, //binary operation to do to pairwise elements for reduce
                                  T_RESULT init_val //value to be used to initialize any buffers
                                  ) {
    T_RESULT temp = init_val;
    if (!d_filter1 && !d_filter2) {
      for (size_t tid = (blockDim.x * blockIdx.x) + threadIdx.x, tid_step = blockDim.x * gridDim.x;
           tid < numElements; tid += tid_step) {
        temp = bin_op( temp, (T_RESULT)(d_data[tid]) );
      }
    } else if (!d_filter1 || !d_filter2) {
      Q_I1_t const * filter = d_filter1 ? d_filter1 : d_filter2;
      for (size_t tid = (blockDim.x * blockIdx.x) + threadIdx.x, tid_step = blockDim.x * gridDim.x;
           tid < numElements; tid += tid_step) {
        if (filter[tid]) {
          temp = bin_op( temp, (T_RESULT)(d_data[tid]) );
        }
      }
    } else {
      for (size_t tid = (blockDim.x * blockIdx.x) + threadIdx.x, tid_step = blockDim.x * gridDim.x;
           tid < numElements; tid += tid_step) {
        if (d_filter1[tid] && d_filter2[tid]) {
          temp = bin_op( temp, (T_RESULT)(d_data[tid]) );
        }
      }
    }

    extern __shared__ char d_shared[]; // must be allocated to blockDim.x * sizeof(T_RESULT) bytes
    T_RESULT * d_threadPartial = (T_RESULT *)(d_shared); // must be allocated to blockDim.x * sizeof(T_RESULT) bytes
    d_threadPartial[threadIdx.x] = temp;
    __syncthreads();

    reduce_data<T_RESULT, BinaryFunction>(d_threadPartial, blockDim.x, bin_op);

    // store result in global memory
    if (threadIdx.x == 0) {
      d_partial[blockIdx.x] = d_threadPartial[0];
    }
  }

  //kernel to aggregate partial results produced per block after reduce
  template <typename T, typename BinaryFunction>
  __global__
  void
  reduce_partial_results_kernel(
                                T * d_data,
                                size_t numElements,
                                BinaryFunction bin_op,
                                T init_val
                                ) {
    // only one block will do all the works since we need to be able to sync all threads used for the reduction
    // remember to set dimGrid = 1x1x1
    if (blockIdx.x != 0) {
      return;
    }
    // collapse data down to num-threads (== blockDim.x)
    if (numElements > blockDim.x) {
      for (size_t tid = threadIdx.x + blockDim.x; tid < numElements; tid += blockDim.x) {
        d_data[threadIdx.x] = bin_op(d_data[threadIdx.x], d_data[tid]);
      }
      __syncthreads();
    }

    reduce_data<T, BinaryFunction>(d_data, blockDim.x, bin_op);
    // final result is now in d_data[0]
  }

  template<typename T>
  __global__
  void
  filter_sum_kernel(
                    T const * d_data, //input data to be reduced
                    size_t numElements, //number of elements
                    Q_I1_t const * d_filter1, // optional filter: only consider i s.t. d_filter1[i] != 0
                    Q_I1_t const * d_filter2, // optional filter: only consider i s.t. d_filter2[i] != 0
                    unsigned long long int * d_sum, // global variable
                    Q_I8_t init_val = 0 //value to be used to initialize any buffers
                    ) {
    Q_I8_t temp = init_val;
    if (!d_filter1 && !d_filter2) {
      for (size_t tid = (blockDim.x * blockIdx.x) + threadIdx.x, tid_step = blockDim.x * gridDim.x;
           tid < numElements; tid += tid_step) {
        temp += (Q_I8_t)(d_data[tid]);
      }
    } else if (!d_filter1 || !d_filter2) {
      Q_I1_t const * filter = d_filter1 ? d_filter1 : d_filter2;
      for (size_t tid = (blockDim.x * blockIdx.x) + threadIdx.x, tid_step = blockDim.x * gridDim.x;
           tid < numElements; tid += tid_step) {
        if (filter[tid]) {
          temp += (Q_I8_t)(d_data[tid]);
        }
      }
    } else {
      for (size_t tid = (blockDim.x * blockIdx.x) + threadIdx.x, tid_step = blockDim.x * gridDim.x;
           tid < numElements; tid += tid_step) {
        if (d_filter1[tid] && d_filter2[tid]) {
          temp += (Q_I8_t)(d_data[tid]);
        }
      }
    }

    extern __shared__ char d_shared[]; // must be allocated to blockDim.x * sizeof(Q_I8_t) bytes
    Q_I8_t * d_threadPartial = (Q_I8_t *)(d_shared);
    d_threadPartial[threadIdx.x] = temp;
    __syncthreads();

    reduce_data<Q_I8_t, ADD<Q_I8_t> >(d_threadPartial, blockDim.x, ADD<Q_I8_t>());

    // combine result in global memory
    if (threadIdx.x == 0) {
      atomicAdd(d_sum, (unsigned long long int)(d_threadPartial[0]));
    }
  }

  template <typename T_INDEX, typename T_VALUE, typename T_RESULT>
  __global__
  void
  count_kernel(
               T_INDEX const * d_idx,
               Q_I1_t const * d_cond,
               T_VALUE const * d_value, // for countf
               T_RESULT * d_dst,
               size_t nSrc,
               size_t nDst,
               size_t nShared
               ) {
    extern __shared__ char d_shared[]; // must be allocated to nDst * sizeof(T_RESULT) bytes

    T_RESULT * d_tgt = nShared ? (T_RESULT *)(d_shared) : d_dst;

    if (nShared) {
      for (size_t tid = threadIdx.x, tid_step = blockDim.x; tid < nDst; tid += tid_step) {
        d_tgt[tid] = 0;
      }
      __syncthreads();
    }

    if (d_value && d_cond) {
      for (size_t tid = (blockDim.x * blockIdx.x) + threadIdx.x, tid_step = blockDim.x * gridDim.x;
           tid < nSrc; tid += tid_step) {
        if (d_cond[tid]) {
          atomicAdd(&d_tgt[d_idx[tid]], d_value[tid]);
        }
      }
    } else if (d_value) {
      for (size_t tid = (blockDim.x * blockIdx.x) + threadIdx.x, tid_step = blockDim.x * gridDim.x;
           tid < nSrc; tid += tid_step) {
        if (d_cond[tid]) {
          atomicAdd(&d_tgt[d_idx[tid]], d_value[tid]);
        }
      }
    } else if (d_cond) {
      for (size_t tid = (blockDim.x * blockIdx.x) + threadIdx.x, tid_step = blockDim.x * gridDim.x;
           tid < nSrc; tid += tid_step) {
        if (d_cond[tid]) {
          atomicAdd(&d_tgt[d_idx[tid]], 1);
        }
      }
    } else {
      for (size_t tid = (blockDim.x * blockIdx.x) + threadIdx.x, tid_step = blockDim.x * gridDim.x;
           tid < nSrc; tid += tid_step) {
        atomicAdd(&d_tgt[d_idx[tid]], 1);
      }
    }

    if (nShared) {
      __syncthreads();
      for (size_t tid = threadIdx.x, tid_step = blockDim.x; tid < nDst; tid += tid_step) {
        atomicAdd(&d_dst[tid], d_tgt[tid]);
      }
    }
  }

  // TODO: try replacing this with thrust::lower_bound
  template <typename T>
  __device__
  T const *
  lower_bound(
              T const * first,
              T const * last,
              T val
              ) {
    T const * it;
    size_t count, step;
    count = last - first;
    while (count) {
      it = first;
      step = count/2;
      it += step;
      if (*it < val) {
        first = ++it;
        count -= step+1;
      } else {
        count = step;
      }
    }
    return first;
  }

  template <typename T_INDEX, typename T_VALUE, typename MERGE_OP>
  __device__
  T_VALUE
  reduce_join_range(
                    T_INDEX const ** range_begin,
                    T_INDEX const * range_limit,
                    T_VALUE const * val,
                    MERGE_OP op,
                    T_VALUE init_val
                    ) {
    T_VALUE link_val = **range_begin;
    T_VALUE result = init_val;
    for ( ;(*range_begin != range_limit) && (**range_begin == link_val); ++(*range_begin), ++val) {
      result = op(result, *val);
    }
    return result;
  }

  template <typename T_LINK, typename T_VALUE, typename MERGE_OP>
  __global__
  void
  srt_join_kernel(
                  T_LINK const * d_src_link,
                  T_VALUE const * d_src_val,
                  size_t nSrc,
                  T_LINK const * d_dst_link,
                  T_VALUE * d_dst_val,
                  Q_I1_t * d_dst_nn,
                  size_t nDst,
                  MERGE_OP op,
                  T_VALUE init_val,
                  T_VALUE notfound_val, // if no match is found in src
                  Q_I1_t notfound_nn_val
                  ) {
    T_LINK const * src_link_begin = d_src_link;
    T_LINK const * src_link_limit = d_src_link + nSrc;
    for (size_t tid = (blockDim.x * blockIdx.x) + threadIdx.x, tid_step = blockDim.x * gridDim.x;
         (tid < nDst) && (src_link_begin != src_link_limit); tid += tid_step) {
      if ((tid == 0) || (d_dst_link[tid] != d_dst_link[tid-1])) {
        // this is the first of a range of elements in dst_link. find the corresponding range in src_link
        src_link_begin = lower_bound(src_link_begin, src_link_limit, d_dst_link[tid]);
        Q_I1_t dst_nn = notfound_val;
        T_VALUE dst_value = notfound_nn_val;
        if ((src_link_begin != src_link_limit) && (*src_link_begin == d_dst_link[tid])) {
          dst_nn = 1;
          size_t nSrcRange = src_link_begin - d_src_link;
          dst_value = reduce_join_range(&src_link_begin, src_link_limit, d_src_val + nSrcRange, op, init_val);
        }
        for (size_t i = tid; (i < nDst) && (d_dst_link[i] == d_dst_link[tid]); ++i) {
          d_dst_val[i] = dst_value;
          d_dst_nn[i] = dst_nn;
        }
      }
    }
  }

  template<typename T, typename UnaryFunction>
  __global__
  void
  is_any_op_kernel(
                   T * d_data,
                   size_t nelem,
                   UnaryFunction op,
                   int * d_result
                   ) {
    if (*d_result)
      return;
    for (size_t tid = blockIdx.x * blockDim.x + threadIdx.x, tid_step = blockDim.x * gridDim.x;
         tid < nelem; tid += tid_step) {
      if (op(d_data[tid])) {
        atomicCAS(d_result, 0, 1);
        break;
      }
    }
  }

  template<typename T_DATA, typename T_IDX>
  __global__
  void
  permute_by_idx_kernel(
                        T_DATA const * d_data,
                        T_IDX const * d_idx,
                        size_t nelem,
                        bool bForward,
                        T_DATA * d_result
                        ) {
    if (bForward) {
      for (size_t tid = blockIdx.x * blockDim.x + threadIdx.x, tid_step = blockDim.x * gridDim.x;
           tid < nelem; tid += tid_step) {
        d_result[tid] = d_data[d_idx[tid]];
      }
    } else {
      for (size_t tid = blockIdx.x * blockDim.x + threadIdx.x, tid_step = blockDim.x * gridDim.x;
           tid < nelem; tid += tid_step) {
        d_result[d_idx[tid]] = d_data[tid];
      }
    }
  }

  template<typename T>
  __global__
  void
  funnel_kernel(
                T const * d_data,
                size_t nelem,
                T value,
                Q_I1_t const * d_same_session,
                Q_I1_t const * d_prev_funnel,
                Q_I1_t * d_result
                ) {
    for (size_t tid = blockIdx.x * blockDim.x + threadIdx.x, tid_step = blockDim.x * gridDim.x;
         tid < nelem; tid += tid_step) {
      //   d_result[tid] = d_same_session[tid] && (tid && d_prev_funnel[tid - 1]) && (d_data[tid] == value);
      // note:  if d_same_session[tid] != 0 then tid > 0 so no need to check tid as well
      d_result[tid] = d_same_session[tid] && d_prev_funnel[tid - 1] && (d_data[tid] == value);
    }
  }

  template <typename T_INDEX, typename T_RESULT>
  __global__
  void
  funnel_count_kernel(
                      T_INDEX const * d_idx,
                      Q_I1_t const * d_same_session,
                      Q_I1_t const * d_current,
                      size_t distance,
                      T_RESULT * d_dst,
                      size_t nSrc,
                      size_t nDst,
                      size_t nShared
                      ) {
    extern __shared__ char d_shared[]; // must be allocated to nDst * sizeof(T_RESULT) bytes

    T_RESULT * d_tgt = nShared ? (T_RESULT *)(d_shared) : d_dst;

    if (nShared) {
      for (size_t tid = threadIdx.x, tid_step = blockDim.x; tid < nDst; tid += tid_step) {
        d_tgt[tid] = 0;
      }
      __syncthreads();
    }

    for (size_t tid = (blockDim.x * blockIdx.x) + threadIdx.x, tid_step = blockDim.x * gridDim.x;
         tid < nSrc; tid += tid_step) {
      typedef unsigned long long ull_t;
      if (d_current[tid]) {
        for (size_t i = tid + 1, iEnd = min(ull_t(i + distance), ull_t(nSrc));
             (i < iEnd) && d_same_session[i]; ++i) {
          if (d_idx[i] < nDst) {
            atomicAdd(&d_tgt[d_idx[i]], 1);
          } else {
            // TODO: Deal with this
          }
        }
      }
    }

    if (nShared) {
      __syncthreads();
      for (size_t tid = threadIdx.x, tid_step = blockDim.x; tid < nDst; tid += tid_step) {
        atomicAdd(&d_dst[tid], d_tgt[tid]);
      }
    }
  }

}
#endif

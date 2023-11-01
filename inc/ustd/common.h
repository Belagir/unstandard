
#ifndef __COMMON_H__
#define __COMMON_H__

#include <stddef.h>
#include <stdint.h>

#ifdef DEBUG
#define UNITTESTING
#endif

// temp
#define UNITTESTING

typedef unsigned char byte;

typedef int8_t  i8;   ///< convenience type redefinition
typedef int16_t i16;  ///< convenience type redefinition
typedef int32_t i32;  ///< convenience type redefinition
typedef int64_t i64;  ///< convenience type redefinition

typedef uint8_t  u8;  ///< convenience type redefinition
typedef uint16_t u16; ///< convenience type redefinition
typedef uint32_t u32; ///< convenience type redefinition
typedef uint64_t u64; ///< convenience type redefinition

typedef float  f32; ///< convenience type redefinition
typedef double f64; ///< convenience type redefinition

#define force_cast(__TYPE, __variable) (*((__TYPE*) &(__variable)))
/// returns the sign bitfield of a 32-bit signed integer
#define sgn_i32(_v) ((_v) >> 31)

#define ceil_div(_a, _b) (((_a) + (_b) - 1) / _b)

/// maximum between two values
#define max(_a, _b) (((_a) > (_b)) ? (_a) : (_b))
/// minimum between two values
#define min(_a, _b) (((_a) < (_b)) ? (_a) : (_b))

#define sizeof_element(__array) (sizeof(*(__array)))
#define count_of(__array) (sizeof(__array) / sizeof_element(__array))

/**
 * @brief Copies byte to byte from `source` to `dest`.
 * The size of `source` and `dest` must both be of at least `nb_bytes`, and `source` must be after `dest` if the two regions overlap.
 *
 * @param[out] dest pointer to the start of the copied-on region
 * @param[in] source pointer to the start of the copied-from region
 * @param[in] nb_bytes number of bytes copied.
 */
void bytewise_copy(void *dest, const void *source, size_t nb_bytes);

/**
 * @brief Counts the number of set bits in a byte.
 *
 * @param[in] value a byte
 * @return u8 total number of set bits
 */
u8 count_set_bits(u8 value);

/**
 * @brief Reverse the bit representation of a 32 bit integer.
 *
 * @param[in] x
 * @return u32
 */
u32 reverse_bit_representation(u32 x);

#endif

#ifndef __BIT_TYPES_H__
#define __BIT_TYPES_H__

#include <limits.h>

namespace __bit_type_h_deprecated {
    unsigned int constexpr char_bit = CHAR_BIT;

    template <int t> struct __select_signed_type;
    template <> struct __select_signed_type<1> {typedef signed char      type;};
    template <> struct __select_signed_type<2> {typedef signed short     type;};
    template <> struct __select_signed_type<3> {typedef signed int       type;};
    template <> struct __select_signed_type<4> {typedef signed long      type;};
    template <> struct __select_signed_type<5> {typedef signed long long type;};

    template <int t> struct __select_unsigned_type;
    template <> struct __select_unsigned_type<1> {typedef unsigned char      type;};
    template <> struct __select_unsigned_type<2> {typedef unsigned short     type;};
    template <> struct __select_unsigned_type<3> {typedef unsigned int       type;};
    template <> struct __select_unsigned_type<4> {typedef unsigned long      type;};
    template <> struct __select_unsigned_type<5> {typedef unsigned long long type;};

    template <int t> struct __select_float_type;
    template <> struct __select_float_type<1> {typedef float  type;};
    template <> struct __select_float_type<2> {typedef double type;};

    template <int size>
    struct __signed_type_of_bit_width {
    private:
        static int constexpr index =
                sizeof(signed char     ) * char_bit == size ? 1 :
                sizeof(signed short    ) * char_bit == size ? 2 :
                sizeof(signed int      ) * char_bit == size ? 3 :
                sizeof(signed long     ) * char_bit == size ? 4 :
                sizeof(signed long long) * char_bit == size ? 5 : 0;
        static_assert(index, "The compiler doesn't support signed integer of the given bit width");
    public:
        typedef typename __select_signed_type<index>::type type;
    };

    template <int size>
    struct __unsigned_type_of_bit_width {
    private:
        static int constexpr index =
            sizeof(unsigned char     ) * char_bit == size ? 1 :
            sizeof(unsigned short    ) * char_bit == size ? 2 :
            sizeof(unsigned int      ) * char_bit == size ? 3 :
            sizeof(unsigned long     ) * char_bit == size ? 4 :
            sizeof(unsigned long long) * char_bit == size ? 5 : 0;
        static_assert(index, "The compiler doesn't support unsigned integer of the given bit width");
    public:
        typedef typename __select_unsigned_type<index>::type type;
    };

    template <int size>
    struct __float_type_of_bit_width {
    private:
        static int constexpr index =
            sizeof(float ) * char_bit == size ? 1 :
            sizeof(double) * char_bit == size ? 2 : 0;
        static_assert(index, "The compiler doesn't support float point type of the given bit width");
    public:
        typedef typename __select_float_type<index>::type type;
    };
}

#define __using_signed(b, t) typedef __bit_type_h_deprecated::__signed_type_of_bit_width<b>::type t;
#define __using_unsigned(b, t) typedef __bit_type_h_deprecated::__unsigned_type_of_bit_width<b>::type t;
#define __using_float(b, t) typedef __bit_type_h_deprecated::__float_type_of_bit_width<b>::type t;

#endif

#ifndef _CGLIB_HASH_H
#define _CGLIB_HASH_H

#include "base.h"
#include "vec.h"
#include "mat.h"
#include "bbox.h"
#include "ray.h"
#include "frustum3.h"

#include <functional>

namespace cglib
{

    template <class T> CGLIB_FORCEINLINE void
        custom_hash_combine(std::size_t & seed, const T & val)
    {
        std::hash<T> hasher;
        seed ^= hasher(val) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }

}

namespace std
{

    template <typename T, size_t N, typename Traits>
        struct hash<cglib::vec<T, N, Traits> >
    {
        CGLIB_FORCEINLINE size_t operator() (const cglib::vec<T, N, Traits> & v) const
        {
            size_t seed = 0;
            cglib::for_each_unrolled<N>([&](size_t i)
            {
                cglib::custom_hash_combine(seed, v(i));
            });
            return seed;
        }

    };

    template <typename T, size_t N, typename Traits>
        struct hash<cglib::mat<T, N, Traits> >
    {
        size_t operator() (const cglib::mat<T, N, Traits> & m) const
        {
            size_t seed = 0;
            for (size_t i = 0; i < N; i++)
            {
                cglib::for_each_unrolled<N>([&](size_t j)
                {
                    cglib::custom_hash_combine(seed, m(i, j));
                });
            }
            return seed;
        }

    };

    template <typename T, size_t N, typename Traits>
        struct hash<cglib::bbox<T, N, Traits> >
    {
        size_t operator() (const cglib::bbox<T, N, Traits> & b) const
        {
            size_t seed = 0;
            cglib::custom_hash_combine(seed, b.min);
            cglib::custom_hash_combine(seed, b.max);
            return seed;
        }

    };

    template <typename T, size_t N, typename Traits>
        struct hash<cglib::ray<T, N, Traits> >
    {
        size_t operator() (const cglib::ray<T, N, Traits> & r) const
        {
            size_t seed = 0;
            cglib::custom_hash_combine(seed, r.origin);
            cglib::custom_hash_combine(seed, r.direction);
            return seed;
        }

    };

    template <typename T, typename Traits>
        struct hash<cglib::frustum3<T, Traits> >
    {
        size_t operator() (const cglib::frustum3<T, Traits> & fru) const
        {
            size_t seed = 0;
            for (std::size_t i = 0; i < fru.planes.size(); i++)
            {
                cglib::custom_hash_combine(seed, fru.planes[i]);
            }
            return seed;
        }

    };

}    

#endif

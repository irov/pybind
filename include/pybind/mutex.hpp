#pragma once

namespace pybind
{
    typedef struct {
        void *ctx;

        bool( *try_lock )(void *ctx);
        void( *lock )(void *ctx);
        void( *unlock )(void *ctx);
    } kernel_mutex_t;
}
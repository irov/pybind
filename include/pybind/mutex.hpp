#pragma once

namespace pybind
{
    typedef struct {
        void *ctx;

        void( *lock )(void *ctx);
        void( *unlock )(void *ctx);
    } kernel_mutex_t;
}
#include <teks/buffer/Buffer.hpp>

#if TEKS_BUFFER_IMPL_STRING
#include "../../internal/src/buffer/StringBuffer.cpp"
#else
#error "Unknown buffer implementation selected"
#endif

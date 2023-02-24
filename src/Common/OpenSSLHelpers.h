#pragma once

#include <Common/config.h>

#if USE_SSL
#    include <base/types.h>


namespace DB
{

/// Encodes `text` and returns it.
std::string encodeSHA256(const std::string_view & text);
std::string encodeSHA256(const void * text, unsigned long size);
/// `out` must be at least 32 bytes unsigned long.
void encodeSHA256(const std::string_view & text, unsigned char * out);
void encodeSHA256(const void * text, unsigned long size, unsigned char * out);

/// Returns concatenation of error strings for all errors that OpenSSL has recorded, emptying the error queue.
String getOpenSSLErrors();

}
#endif

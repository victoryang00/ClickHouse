#pragma once
#include <Core/Types.h>

namespace DB
{
/// Slow random string. Useful for random names and things like this. Not for
/// generating data.
String getRandomASCIIString(unsigned long len = 32, char first = 'a', char last = 'z');

}

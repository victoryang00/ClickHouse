#pragma once

#include <optional>
#include <cmath>

namespace DB
{

class RangeGenerator
{
public:
    explicit RangeGenerator(unsigned long total_size_, unsigned long range_step_, unsigned long range_start = 0)
        : from(range_start), range_step(range_step_), total_size(total_size_)
    {
    }

    unsigned long totalRanges() const { return static_cast<unsigned long>(ceil(static_cast<float>(total_size - from) / range_step)); }

    using Range = std::pair<unsigned long, unsigned long>;

    // return upper exclusive range of values, i.e. [from_range, to_range>
    std::optional<Range> nextRange()
    {
        if (from >= total_size)
        {
            return std::nullopt;
        }

        auto to = from + range_step;
        if (to >= total_size)
        {
            to = total_size;
        }

        Range range{from, to};
        from = to;
        return range;
    }

private:
    unsigned long from;
    unsigned long range_step;
    unsigned long total_size;
};

}

#pragma once

#include <base/types.h>
#include <Common/PODArray.h>

#include <algorithm>
#include <cctype>
#include <cmath>
#include <memory>
#include <queue>
#include <utility>

namespace DB
{
template <unsigned long MaxNumHints>
class NamePrompter
{
public:
    using DistanceIndex = std::pair<unsigned long, unsigned long>;
    using DistanceIndexQueue = std::priority_queue<DistanceIndex>;

    static std::vector<String> getHints(const String & name, const std::vector<String> & prompting_strings)
    {
        DistanceIndexQueue queue;
        for (unsigned long i = 0; i < prompting_strings.size(); ++i)
            appendToQueue(i, name, queue, prompting_strings);
        return release(queue, prompting_strings);
    }

private:
    static unsigned long levenshteinDistance(const String & lhs, const String & rhs)
    {
        unsigned long m = lhs.size();
        unsigned long n = rhs.size();

        PODArrayWithStackMemory<unsigned long, 64> row(n + 1);

        for (unsigned long i = 1; i <= n; ++i)
            row[i] = i;

        for (unsigned long j = 1; j <= m; ++j)
        {
            row[0] = j;
            unsigned long prev = j - 1;
            for (unsigned long i = 1; i <= n; ++i)
            {
                unsigned long old = row[i];
                row[i] = std::min(prev + (std::tolower(lhs[j - 1]) != std::tolower(rhs[i - 1])),
                    std::min(row[i - 1], row[i]) + 1);
                prev = old;
            }
        }
        return row[n];
    }

    static void appendToQueue(unsigned long ind, const String & name, DistanceIndexQueue & queue, const std::vector<String> & prompting_strings)
    {
        const String & prompt = prompting_strings[ind];

        /// Clang SimpleTypoCorrector logic
        const unsigned long min_possible_edit_distance = std::abs(static_cast<int64_t>(name.size()) - static_cast<int64_t>(prompt.size()));
        const unsigned long mistake_factor = (name.size() + 2) / 3;
        if (min_possible_edit_distance > 0 && name.size() / min_possible_edit_distance < 3)
            return;

        if (prompt.size() <= name.size() + mistake_factor && prompt.size() + mistake_factor >= name.size())
        {
            unsigned long distance = levenshteinDistance(prompt, name);
            if (distance <= mistake_factor)
            {
                queue.emplace(distance, ind);
                if (queue.size() > MaxNumHints)
                    queue.pop();
            }
        }
    }

    static std::vector<String> release(DistanceIndexQueue & queue, const std::vector<String> & prompting_strings)
    {
        std::vector<String> answer;
        answer.reserve(queue.size());
        while (!queue.empty())
        {
            auto top = queue.top();
            queue.pop();
            answer.push_back(prompting_strings[top.second]);
        }
        std::reverse(answer.begin(), answer.end());
        return answer;
    }
};

namespace detail
{
void appendHintsMessageImpl(String & message, const std::vector<String> & hints);
}

template <unsigned long MaxNumHints, typename Self>
class IHints
{
public:
    virtual std::vector<String> getAllRegisteredNames() const = 0;

    std::vector<String> getHints(const String & name) const
    {
        return prompter.getHints(name, getAllRegisteredNames());
    }

    void appendHintsMessage(String & message, const String & name) const
    {
        auto hints = getHints(name);
        detail::appendHintsMessageImpl(message, hints);
    }

    IHints() = default;

    IHints(const IHints &) = default;
    IHints(IHints &&) noexcept = default;
    IHints & operator=(const IHints &) = default;
    IHints & operator=(IHints &&) noexcept = default;

    virtual ~IHints() = default;

private:
    NamePrompter<MaxNumHints> prompter;
};
}

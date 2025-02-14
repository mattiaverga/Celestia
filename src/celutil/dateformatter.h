// dateformatter.h
//
// Copyright (C) 2023-present, Celestia Development Team.
//
// ICU date formatter
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

#pragma once

#ifdef USE_ICU
#include <celutil/includeicu.h>
#include <array>
#include "uniquedel.h"
#endif

#include <celastro/date.h>

namespace celestia::engine
{

class DateFormatter
{
public:
    DateFormatter() = default;
    ~DateFormatter() = default;
    DateFormatter(const DateFormatter &) = delete;
    DateFormatter(DateFormatter &&) noexcept = default;
    DateFormatter &operator=(const DateFormatter &) = delete;
    DateFormatter &operator=(DateFormatter &&) noexcept = default;

    std::string formatDate(double tdb, bool local, astro::Date::Format format);

#ifdef USE_ICU
private:
    using UniqueDateFormat = util::UniquePtrDel<UDateFormat, udat_close>;

    static constexpr auto FormatCount = static_cast<std::size_t>(astro::Date::FormatCount);

    std::array<UniqueDateFormat, FormatCount> localFormatters;
    std::array<UniqueDateFormat, FormatCount> utcFormatters;

    UDateFormat *getFormatter(bool local, astro::Date::Format format);
#endif
};

}

/* Smack C++ @ https://github.com/smacklib/dev_smack_cpp
 *
 * Localisation support.
 *
 * Copyright © 2025-2026 Michael Binz
 */

#include "smack_locale.h"
#include "smack_system.h"

namespace smack {

std::optional<Locale> Locale::current_;

auto Locale::makeLocaleFromName(const std::string& name) -> Locale
{
    if (name.empty()) {
        return Locale{};
    }

    auto parts = smack::split(name, "_");

    if (parts.size() == 1) {
        return Locale{parts[0]};
    }
    if (parts.size() == 2) {
        return Locale{parts[0], parts[1]};
    }

    throw std::invalid_argument("Invalid locale name: " + name);
}

Locale::Locale()
{
}

Locale::Locale(std::string_view isoLanguage, std::string_view isoCountry)
    : language_{isoLanguage}
    , country_{isoCountry}
{
    if (country_.find_first_of("_") != std::string::npos) {
        throw std::invalid_argument("isoCountry contains '_'.");
    }
    if (language_.find_first_of("_") != std::string::npos) {
        throw std::invalid_argument("isoLanguage contains '_'.");
    }
}

auto Locale::isEmpty() const -> bool
{
    return language_.empty() && country_.empty();
}

auto Locale::toString() const -> std::string
{
    if (country_.empty()) {
        return language_;
    }

    return language_ + "_" + country_;
}

auto Locale::getCountry() const -> std::string
{
    return country_;
}

auto Locale::getLanguage() const -> std::string
{
    return language_;
}

auto Locale::getParent() const -> Locale
{
    if (country_.empty()) {
        return Locale{};
    }

    return Locale{language_};
}

auto Locale::getCurrent() -> Locale
{
    std::lock_guard<std::mutex> _(current_mutex_);

    if (!current_.has_value())
        current_ = smack::system::getLocale();

    return current_.value();
}

auto Locale::setCurrent(const Locale& locale) -> Locale
{
    auto result = getCurrent();

    std::lock_guard<std::mutex> _(current_mutex_);

    current_ = locale;

    return result;
}

bool Locale::operator<(const Locale& r) const
{
    return toString() < r.toString();
}

bool Locale::operator==(const Locale& r) const
{
    return toString() == r.toString();
}

} // namespace smack

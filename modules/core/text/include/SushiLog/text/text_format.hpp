/**************************************************************************/
/* text_format.hpp                                                        */
/**************************************************************************/
/*                          This file is part of:                         */
/*                                SushiLog                                */
/*                https://github.com/SushiSystems/SushiLog                */
/*                         https://sushisystems.io                        */
/**************************************************************************/
/* Copyright (c) 2026-present Mustafa Garip & Sushi Systems               */
/*                                                                        */
/* Licensed under the PolyForm Noncommercial License 1.0.0 (the           */
/* "License"); you may not use this file except in compliance with the    */
/* License. You may obtain a copy of the License at                       */
/*                                                                        */
/*     https://polyformproject.org/licenses/noncommercial/1.0.0           */
/*                                                                        */
/* Noncommercial use is free. Commercial use requires a separate licence  */
/* from Sushi Systems; see COMMERCIAL.md. The software is provided        */
/* "as is", without warranty of any kind.                                 */
/**************************************************************************/

/**
 * @file text_format.hpp
 * @brief Declares format_text, which writes each {} of a format with the next argument.
 * @author Mustafa Garip
 * @see modules/core/text/README.md
 */

#pragma once

#include <cstddef>
#include <ostream>
#include <sstream>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>

namespace SushiLog
{
    /** @brief Holds what format_text builds on; nothing outside the text module calls into it. */
    namespace Detail
    {
        /** @brief Tells whether an output stream can write a const T&. */
        template <typename T, typename = void>
        struct IsStreamable : std::false_type
        {
        };

        /** @brief Tells whether an output stream can write a const T&. */
        template <typename T>
        struct IsStreamable<T, std::void_t<decltype(std::declval<std::ostream&>() << std::declval<const T&>())>>
            : std::true_type
        {
        };

        /**
         * @brief Writes @p value: an 8-bit integer as a number, a scoped enum with no operator<< as
         *        its underlying value, a null C string as (null), anything else through operator<<.
         */
        template <typename T>
        void write_value(std::ostream& out, const T& value)
        {
            if constexpr (std::is_same_v<T, signed char> || std::is_same_v<T, unsigned char>)
                out << static_cast<int>(value);
            else if constexpr (std::is_enum_v<T> && !IsStreamable<T>::value)
                out << static_cast<long long>(static_cast<std::underlying_type_t<T>>(value));
            else if constexpr (std::is_pointer_v<T> &&
                               std::is_same_v<std::remove_cv_t<std::remove_pointer_t<T>>, char>)
                out << (value != nullptr ? value : "(null)");
            else
                out << value;
        }

        /** @brief Returns @p value as text, as write_value() writes it. */
        template <typename T>
        std::string value_text(const T& value)
        {
            std::ostringstream stream;
            write_value(stream, value);
            return stream.str();
        }

        /**
         * @brief Returns @p format with each {} replaced by the next of @p count @p values.
         *
         * A {} past the last value becomes {?}, and values past the last {} are counted at the
         * end as {+N}, so a message with a missing or extra argument shows it.
         */
        inline std::string substitute(std::string_view format, const std::string* values,
                                      std::size_t count)
        {
            std::string out;
            out.reserve(format.size());
            std::size_t used = 0;
            for (std::size_t i = 0; i < format.size(); ++i)
            {
                const char c = format[i];
                const bool has_next = i + 1 < format.size();
                if ((c == '{' || c == '}') && has_next && format[i + 1] == c)
                {
                    out += c;
                    ++i;
                    continue;
                }
                if (c == '{' && has_next && format[i + 1] == '}')
                {
                    out += used < count ? values[used] : std::string("{?}");
                    ++used;
                    ++i;
                    continue;
                }
                out += c;
            }
            if (used < count)
                out += " {+" + std::to_string(count - used) + "}";
            return out;
        }
    } // namespace Detail

    /**
     * @brief Returns @p format with each {} replaced by the next argument, in order.
     *
     * {{ and }} are literal braces; there are no format specs. Values are written as
     * Detail::write_value() does, and missing or extra arguments as Detail::substitute() does.
     * @tparam Args Types an output stream can write, or scoped enums.
     */
    template <typename... Args>
    std::string format_text(std::string_view format, const Args&... args)
    {
        const std::string values[] = {Detail::value_text(args)..., std::string()};
        return Detail::substitute(format, values, sizeof...(Args));
    }
} // namespace SushiLog

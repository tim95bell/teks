#pragma once

#include <compare>
#include <limits>
#include <optional>
#include <teks/assert.hpp>
#include <teks/types.hpp>

namespace teks::buffer {
    struct Offset;
    struct Bytes;

    struct Bytes {
        using ValueType = u64;

        static constexpr ValueType maxValue() { return std::numeric_limits<ValueType>::max(); }

        constexpr Bytes() = default;
        explicit constexpr Bytes(ValueType value);
        explicit constexpr Bytes(Offset offset);
        constexpr Bytes(const Bytes&) = default;
        constexpr Bytes(Bytes&&) noexcept = default;

        Bytes& operator=(const Bytes&) = default;
        Bytes& operator=(Bytes&&) noexcept = default;
        [[nodiscard]] friend constexpr bool operator==(const Bytes&, const Bytes&) = default;
        [[nodiscard]] friend constexpr
        std::strong_ordering operator<=>(const Bytes&, const Bytes&) = default;
        friend constexpr Bytes operator+(Bytes, Bytes);
        friend constexpr Bytes operator-(Bytes, Bytes);

        [[nodiscard]] inline constexpr ValueType raw() const;

    private:
        ValueType value_{0};
    };

    struct Offset {
        using ValueType = Bytes::ValueType;

        constexpr Offset() = default;
        explicit constexpr Offset(ValueType value);
        explicit constexpr Offset(Bytes bytes);
        constexpr Offset(const Offset&) = default;
        constexpr Offset(Offset&&) noexcept = default;

        Offset& operator=(const Offset&) = default;
        Offset& operator=(Offset&&) noexcept = default;
        [[nodiscard]] friend constexpr
        bool operator==(const Offset&, const Offset&) = default;
        [[nodiscard]] friend constexpr
        std::strong_ordering operator<=>(const Offset&, const Offset&) = default;
        friend constexpr Offset operator+(Offset, Bytes);
        friend constexpr Offset operator-(Offset, Bytes);
        friend constexpr Bytes operator-(Offset, Offset);

        [[nodiscard]] inline constexpr ValueType raw() const;

    private:
        Bytes value_{0};
    };

    // [start, end)
    struct Range {
        using ValueType = Offset::ValueType;

        [[nodiscard]] static std::optional<Range> tryMake(Offset start, Offset end);
        [[nodiscard]] static std::optional<Range> tryMake(Offset start, Bytes size);
        [[nodiscard]] static Range makeUnchecked(Offset start, Offset end);
        [[nodiscard]] static Range makeUnchecked(Offset start, Bytes size);

        Range() = default;
        explicit Range(Offset end);
        explicit Range(Bytes size);
        constexpr Range(const Range&) = default;
        constexpr Range(Range&&) noexcept = default;

        ~Range() = default;

        Range& operator=(const Range&) = default;
        Range& operator=(Range&&) noexcept = default;

        [[nodiscard]] inline Offset start() const;
        [[nodiscard]] inline Offset end() const;
        [[nodiscard]] inline Bytes size() const;

        [[nodiscard]] friend constexpr bool operator==(const Range&, const Range&) = default;

    private:
        Offset start_;
        Offset end_;

        explicit Range(ValueType size);
        Range(Offset start, Offset end);
        Range(Offset start, Bytes size);
    };

    [[nodiscard]] inline constexpr bool addWillOverflow(Bytes lhs, Bytes rhs) {
        return lhs > Bytes(Bytes::maxValue()) - rhs;
    }

    [[nodiscard]] inline constexpr bool addWillOverflow(Offset lhs, Bytes rhs) {
        return addWillOverflow(Bytes(lhs), rhs);
    }

    // #region Bytes
    constexpr Bytes::Bytes(ValueType value) : value_(value) {}
    constexpr Bytes::Bytes(Offset offset) : value_(offset.raw()) {}

    [[nodiscard]] inline constexpr Bytes operator+(Bytes lhs, Bytes rhs) {
        TEKS_ASSERT_MSG(!addWillOverflow(lhs, rhs), "Bytes overflow");
        return Bytes{lhs.raw() + rhs.raw()};
    }

    [[nodiscard]] inline constexpr Bytes operator-(Bytes lhs, Bytes rhs) {
        TEKS_ASSERT_MSG(lhs >= rhs, "Bytes underflow");
        return Bytes{lhs.raw() - rhs.raw()};
    }

    [[nodiscard]] inline constexpr Bytes::ValueType Bytes::raw() const { return value_; }
    // #endregion Bytes

    // #region Offset
    constexpr Offset::Offset(ValueType value) : value_(Bytes(value)) {}
    constexpr Offset::Offset(Bytes bytes) : value_(bytes) {}

    [[nodiscard]] inline constexpr bool operator<(Offset lhs, Bytes rhs) {
        return lhs.raw() < rhs.raw();
    }

    [[nodiscard]] inline constexpr bool operator<=(Offset lhs, Bytes rhs) {
        return lhs.raw() <= rhs.raw();
    }

    [[nodiscard]] inline constexpr bool operator>(Offset lhs, Bytes rhs) {
        return lhs.raw() > rhs.raw();
    }

    [[nodiscard]] inline constexpr bool operator>=(Offset lhs, Bytes rhs) {
        return lhs.raw() >= rhs.raw();
    }

    [[nodiscard]] inline constexpr Offset operator+(Offset lhs, Bytes rhs) {
        TEKS_ASSERT_MSG(!addWillOverflow(lhs, rhs), "Offset overflow");
        return Offset{lhs.raw() + rhs.raw()};
    }

    [[nodiscard]] inline constexpr Offset operator-(Offset lhs, Bytes rhs) {
        TEKS_ASSERT_MSG(lhs >= rhs, "Offset underflow");
        return Offset{lhs.raw() - rhs.raw()};
    }

    [[nodiscard]] inline constexpr Bytes operator-(Offset lhs, Offset rhs) {
        TEKS_ASSERT_MSG(lhs >= rhs, "Negative offset distance");
        return Bytes{lhs.raw() - rhs.raw()};
    }

    [[nodiscard]] inline constexpr Offset::ValueType Offset::raw() const { return value_.raw(); }
    // #endregion Offset

    // #region Range
    [[nodiscard]] inline std::optional<Range> Range::tryMake(Offset start, Offset end) {
        if (start <= end) {
            return std::optional<Range>(Range(start, end));
        }
        return std::nullopt;
    }

    [[nodiscard]] inline std::optional<Range> Range::tryMake(Offset start, Bytes size) {
        if (addWillOverflow(start, size)) {
            return std::nullopt;
        }
        return std::optional<Range>(Range(start, size));
    }

    [[nodiscard]] inline Range Range::makeUnchecked(Offset start, Offset end) {
        return Range(start, end);
    }

    [[nodiscard]] inline Range Range::makeUnchecked(Offset start, Bytes size) {
        return Range(start, size);
    }

    inline Range::Range(Offset start, Offset end) : start_(start), end_(end) {
        TEKS_ASSERT_MSG(start <= end, "Range start must be <= end");
    }

    inline Range::Range(Offset start, Bytes size) : start_(start), end_(start + size) {}
    inline Range::Range(ValueType size) : start_(0), end_(size) {}
    inline Range::Range(Offset end) : Range(end.raw()) {}
    inline Range::Range(Bytes size) : Range(size.raw()) {}

    [[nodiscard]] inline Offset Range::start() const { return start_; }
    [[nodiscard]] inline Offset Range::end() const { return end_; }
    [[nodiscard]] inline Bytes Range::size() const { return Bytes(end_.raw() - start_.raw()); }
    // #endregion Range
} // namespace teks::buffer

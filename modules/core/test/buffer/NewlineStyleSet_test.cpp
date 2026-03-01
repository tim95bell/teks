#include <teks/buffer/NewlineStyleSet.hpp>
#include <gtest/gtest.h>
#include <set>
#include <array>

using namespace teks::buffer;

namespace {
    void assertHas(const NewlineStyleSet& set, std::set<NewlineStyleSet::Style> styles) {
        using Style = NewlineStyleSet::Style;

        const std::array allStyles{
            Style::Cr,
            Style::Lf,
            Style::Crlf,
        };

        for (Style style : allStyles) {
            ASSERT_EQ(set.has(style), styles.contains(style));
        }
    }
}

TEST(teksBufferNewlineStyleSet, defaultConstructorResultsInEmptySet) {
    using Style = NewlineStyleSet::Style;
    NewlineStyleSet set;
    ASSERT_TRUE(set.hasExactly({}));
    assertHas(set, std::set<Style>{});
}

TEST(teksBufferNewlineStyleSet, ofCr) {
    using Style = NewlineStyleSet::Style;
    NewlineStyleSet set = NewlineStyleSet::of({Style::Cr});
    ASSERT_TRUE(set.hasExactly({Style::Cr}));
    assertHas(set, std::set{Style::Cr});
}

TEST(teksBufferNewlineStyleSet, ofLf) {
    using Style = NewlineStyleSet::Style;
    NewlineStyleSet set = NewlineStyleSet::of({Style::Lf});
    ASSERT_TRUE(set.hasExactly({Style::Lf}));
    assertHas(set, std::set{Style::Lf});
}

TEST(teksBufferNewlineStyleSet, ofCrlf) {
    using Style = NewlineStyleSet::Style;
    NewlineStyleSet set = NewlineStyleSet::of({Style::Crlf});
    ASSERT_TRUE(set.hasExactly({Style::Crlf}));
    assertHas(set, std::set{Style::Crlf});
}

TEST(teksBufferNewlineStyleSet, ofAll) {
    using Style = NewlineStyleSet::Style;
    NewlineStyleSet set = NewlineStyleSet::of({Style::Cr, Style::Lf, Style::Crlf});
    ASSERT_TRUE(set.hasExactly({Style::Cr, Style::Lf, Style::Crlf}));
    assertHas(set, std::set{Style::Cr, Style::Lf, Style::Crlf});
}

TEST(teksBufferNewlineStyleSet, add) {
    using Style = NewlineStyleSet::Style;
    NewlineStyleSet set;
    ASSERT_TRUE(set.hasExactly({}));
    assertHas(set, std::set<Style>{});

    set.add(Style::Cr);
    ASSERT_TRUE(set.hasExactly({Style::Cr}));
    assertHas(set, std::set{Style::Cr});

    set.add(Style::Cr);
    ASSERT_TRUE(set.hasExactly({Style::Cr}));
    assertHas(set, std::set{Style::Cr});

    set.add(Style::Lf);
    ASSERT_TRUE(set.hasExactly({Style::Cr, Style::Lf}));
    assertHas(set, std::set{Style::Cr, Style::Lf});

    set.add(Style::Lf);
    ASSERT_TRUE(set.hasExactly({Style::Cr, Style::Lf}));
    assertHas(set, std::set{Style::Cr, Style::Lf});

    set.add(Style::Crlf);
    ASSERT_TRUE(set.hasExactly({Style::Cr, Style::Lf, Style::Crlf}));
    assertHas(set, std::set{Style::Cr, Style::Lf, Style::Crlf});

    set.add(Style::Crlf);
    ASSERT_TRUE(set.hasExactly({Style::Cr, Style::Lf, Style::Crlf}));
    assertHas(set, std::set{Style::Cr, Style::Lf, Style::Crlf});
}

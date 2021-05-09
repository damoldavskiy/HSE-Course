#include <cstdint>
#include <string>

#include "gtest/gtest.h"
#include "optional.h"

task::Optional<std::string> Create(bool b) {
    if (b) {
        return "Godzilla";
    }
    return {};
}

auto Create2(bool b) {
    return (b) ? task::Optional<std::string>{"Godzilla"} : task::kNullOpt;
}

auto CreateRef(bool b) {
    static std::string value = "Godzilla";
    return b ? task::Optional<std::reference_wrapper<std::string>>{value} : task::kNullOpt;
}

TEST(Custom, Test1) {
    ASSERT_EQ(*Create(true), "Godzilla");
    ASSERT_EQ(Create(false).ValueOr("empty"), "empty");
    ASSERT_EQ(*Create2(true), "Godzilla");
    ASSERT_EQ(Create2(false).ValueOr("empty"), "empty");

    auto str = CreateRef(true);
    ASSERT_EQ(str->get(), "Godzilla");

    str->get() = "Mothra";

    ASSERT_EQ(str->get(), "Mothra");
}

TEST(ValueOR, Test1) {
    task::Optional<std::string> opt("Hello world");
    ASSERT_EQ(opt.ValueOr("empty"), "Hello world");
}

TEST(ValueOR, Test2) {
    task::Optional<std::string> opt;
    ASSERT_EQ(opt.ValueOr("empty"), "empty");
}

TEST(HasValue, Test1) {
    task::Optional<std::string> opt("Hello world");
    ASSERT_TRUE(opt.HasValue());
}

TEST(Reset, Test1) {
    task::Optional<std::string> opt("Hello world");
    opt.Reset();
    ASSERT_FALSE(opt.HasValue());
}

TEST(ConversionToBool, Test1) {
    task::Optional<std::string> opt("Hello world");
    ASSERT_TRUE(opt);
}

TEST(ArrowOperator, Test1) {
    task::Optional<std::string> opt("Hello world");
    ASSERT_EQ(std::string(opt->c_str()), "Hello world");
}

TEST(IndirectionOperator, Test1) {
    task::Optional<int32_t> opt(1);
    ASSERT_EQ(*opt, 1);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

#include <array>

namespace utilities::metadata::names
{

namespace _detail
{
consteval auto length(const char * s)
{
    size_t ret = 0;
    while (s[ret] != 0) ret++;
    return ret;
}
typedef char (*char_mapping)(char);
template<char_mapping...mappings> struct compose;

template<char_mapping mapping> struct compose<mapping>
{
    constexpr char operator()(char c) { return mapping(c); }
};

template<char_mapping mapping, char_mapping... mappings> struct compose<mapping, mappings...>
{
    constexpr char operator()(char c) { return compose<mappings...>(mapping(c)); }
};

template<typename NamedType, char_mapping... Mappings>
struct respeller
{
    static constexpr size_t N = length(NamedType::name());
    static constexpr std::array<char, N> value = [](const char * s)
    {
        auto mapping = compose<Mappings...>{}; // construct the composition of mappings
        std::array<char, N> ret{};
        for (int i = 0; i < N; ++i)
        {
            char c = s[i];
            ret[i] = mapping(c); // apply the mapping
        }
        return ret;
    }(NamedType::name());
};

constexpr char snake(char c) {return c == ' ' ? '_' : c;}
constexpr char kebab(char c) {return c == ' ' ? '-' : c;}
}
template<typename NamedType>
constexpr const char * snake_case = _detail::respeller<NamedType, _detail::snake>::value.data();
template<typename NamedType>
constexpr const char * kebab_case = _detail::respeller<NamedType, _detail::kebab>::value.data();

}

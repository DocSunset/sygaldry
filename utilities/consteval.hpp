#ifdef __clang__
#define _consteval constexpr
#else
#define _consteval consteval
#endif

./sh/litlit.sh
[ "$#" -gt 0 ] && dir="$1" || dir='_build_debug'
[ -d "$dir" ] || CMAKE_BUILD_TYPE=Debug cmake -B "$dir" -S .
cmake --build "$dir" &&
{
    ctest --test-dir "$dir" ||
        ctest --test-dir "$dir" --rerun-failed --output-on-failure
}

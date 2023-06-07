./sh/litlit.sh
[ -d "$1" ] && dir="$1" || dir='_build_debug'

cmake --build "$dir" &&
{
    ctest --test-dir "$dir" ||
        ctest --test-dir "$dir" --rerun-failed --output-on-failure
}

[ -d "$1" ] ||
{
    echo "No build directory called $1.
Run e.g. `cmake -B $1 -S .` from the root of the repository to generate one."
    exit 2
}
./sh/litlit.sh
cmake --build "$1" &&
{
    ctest --test-dir "$1" ||
        ctest --test-dir "$1" --rerun-failed --output-on-failure
}

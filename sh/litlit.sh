# generate machine sources from literate source code
find -iname '*.litlit' | parallel '
    cd {//}
    generated="$(grep -m 1 -h -r -I "@#" {/} | head -n 1 | sed 's/^.*@#.//' | sed 's/.$//')"
    [ {/} -nt "$generated" ] &&
    {
        echo {} "newer than $generated, running litlit..."
        litlit {/}
    }
'
find -iname '*.litlit' | parallel '
    cd {//}
    [ -L "README.md" ] &&
    {
        [ "$(readlink README.md)" != {/} ] && rm "README.md" && ln -s -T {/} README.md
    } || [ ! -e "README.md" ] &&
        ln -s -T {/} README.md
'

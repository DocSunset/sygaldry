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

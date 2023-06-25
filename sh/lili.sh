# generate machine sources from literate source code
find -iname '*.lili.md' | parallel '
    cd {//}
    generated="$(grep -m 1 -h -r -I "@#" {/} | head -n 1 | sed 's/^.*@#.//' | sed 's/.$//')"
    [ "$generated" ] && [ {/} -nt "$generated" ] &&
    {
        echo {} "newer than $generated, running lili..."
        lili {/} || exit 1 # short circuit on any lili error with failure return code
    } || exit 0
'

# generate machine sources from literate source code
find -iname '*.lili' | parallel '
    cd {//}
    [ -L "README.md" ] &&
    {
        [ "$(readlink README.md)" != {/} ] && ln -sfv -T {/} README.md
    } || {
        [ ! -e "README.md" ] && ln -s -T {/} README.md
    }
    [ -L "{/}.md" ] && [ "$(readlink {/}.md)" == {/} ] || ln -sfv -T {/} {/}.md
'
find -iname '*.lili' | parallel '
    cd {//}
    generated="$(grep -m 1 -h -r -I "@#" {/} | head -n 1 | sed 's/^.*@#.//' | sed 's/.$//')"
    [ "$generated" ] && [ {/} -nt "$generated" ] &&
    {
        echo {} "newer than $generated, running lili..."
        lili {/}
    } || exit 0
'

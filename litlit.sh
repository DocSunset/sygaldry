# generate machine sources from literate source code
find -iname '*.litlit' | parallel 'echo {}; cd {//}; litlit {/}'

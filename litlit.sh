# generate machine sources from literate source code
find -iname '*.litlit' | tee | parallel 'echo {}; cd {//}; litlit {/}'

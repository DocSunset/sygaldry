#pragma once

#include <stdio.h>
#include <stdlib.h>

namespace sygaldry { namespace bindings { namespace basic_reader {

struct CstdioReader
{
    int last_read;
    bool ready()
    {
        last_read = getc(stdin);
        return last_read != EOF;
    }

    char getchar()
    {
        return last_read;
    }
};

} } }

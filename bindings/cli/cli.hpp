#pragma once
#include <memory>
#include <tuple>
#include <string_view>
#include "utilities/consteval/consteval.hpp"
#include "matcher.hpp"
#include "commands/list.hpp"
#include "commands/help.hpp"
//#include "commands/describe.hpp"

namespace sygaldry { namespace bindings::cli
{

template<typename Config, typename cpts_t, template<typename> typename ... Commands>
struct Cli
{
    [[no_unique_address]] typename Config::basic_logger_type log{};
    std::tuple<Commands<Config>...> commands{};
    cpts_t components;

    Cli(cpts_t cpts, const char * extra_boot_message)
    : components{cpts}
    {
        log.println("CLI enabled. Write `help` for a list of available commands.");
        if (extra_boot_message[0] != '\0') log.println(extra_boot_message);
        _prompt();
    }

    static constexpr size_t MAX_ARGS = 5;
    static constexpr size_t BUFFER_SIZE = 128;
    int argc = 0;
    char * argv[MAX_ARGS];
    unsigned char write_pos = 0;
    char buffer[BUFFER_SIZE];

    int _try_to_match_and_execute()
    {
        return try_to_match_and_execute(argv[0], commands, 127, [this](auto& command)
            {
                if constexpr (std::is_same_v<decltype(command), commands::Help<Config>&>)
                {
                    return std::apply([&]<typename ... Cmds>(Cmds&& ... cmds)
                    {
                        return command.main(cmds...);
                    }, commands);
                }
                else return command.main(argc, argv, *components);
            });
    }
    bool _is_whitespace(char c)
    {
        if (c == ' ' || c == '\t' || c == '\n') return true;
        else return false;
    }

    bool _new_arg() const
    {
        return write_pos == 0 || buffer[write_pos-1] == 0;
    }

    bool _overflow() const
    {
        return argc == MAX_ARGS || write_pos == BUFFER_SIZE;
    }

    void _prompt()
    {
        log.print("> ");
    }

    void _reset()
    {
        argc = 0;
        write_pos = 0;
        _prompt();
    }

    void _complain_about_command_failure(int retcode)
    {} // TODO

    void process(char c)
    {
        if (_is_whitespace(c))
            buffer[write_pos++] = 0;
        else
        {
            buffer[write_pos] = c;
            if (_new_arg())
                argv[argc++] = &buffer[write_pos];
            write_pos++;
        }

        if (c == '\n')
        {
            auto retcode = _try_to_match_and_execute();
            if (retcode != 0) _complain_about_command_failure(retcode);
            _reset();
        }

        if (_overflow())
        {
            log.println("CLI line buffer overflow!");
            _reset();
        }
    }
};

template<typename Config, template<typename>typename ... Commands, typename ... Cpts>
auto make_cli(std::shared_ptr<std::tuple<Cpts...>> cpts, const char * boot_message = "")
{
    return Cli<Config, std::shared_ptr<std::tuple<Cpts...>>, commands::Help, Commands...>(cpts, boot_message);
}

template<typename Config, typename ... Cpts>
auto make_default_cli(std::shared_ptr<std::tuple<Cpts...>> cpts, const char * boot_message = "")
{
    return Cli<Config, std::shared_ptr<std::tuple<Cpts...>>, commands::Help
            , commands::List
            >(cpts, boot_message);
}

} }

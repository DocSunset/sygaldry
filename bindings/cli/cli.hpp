#pragma once
#include <memory>
#include <tuple>
#include <string_view>

#include "commands/list.hpp"
#include "commands/help.hpp"

namespace sygaldry { namespace bindings::cli
{

template<typename Config, typename devs_t, template<typename> typename ... Commands>
struct Cli
{
    [[no_unique_address]] typename Config::basic_logger_type log{};
    std::tuple<Commands<Config>...> commands{};
    devs_t devices;

    Cli(devs_t devs, const char * extra_boot_message)
    : devices{devs}
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

    template<typename Cmd, typename ... Cmds>
    int _try_to_match_and_execute_impl(Cmd&& command, Cmds&&... cmds)
    {
        if (std::string_view(argv[0]) == std::string_view(command.name()))
        {
            return command.main(argc, argv, *devices);
        }
        else if constexpr (sizeof...(Cmds) == 0)
            return 127;
        else
            return _try_to_match_and_execute_impl(cmds...);
    }

    int _try_to_match_and_execute()
    {
        constexpr auto N_cmds = std::tuple_size_v<decltype(commands)>;

        if (std::string_view(argv[0]) == std::string_view("help"))
        {
            auto help = commands::Help<Config>{};
            return std::apply([&]<typename ... Cmds>(Cmds&& ... cmds)
            {
                return help.main(cmds...);
            }, commands);
        }
        else if constexpr (N_cmds == 0) return 127; // avoid impl if no args
        else return std::apply([this]<typename ... Cmds>(Cmds&& ... cmds)
        {
            return _try_to_match_and_execute_impl(cmds...);
        }, commands);
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

template<typename Config, template<typename>typename ... Commands, typename ... Devs>
auto make_cli(std::shared_ptr<std::tuple<Devs...>> devs, const char * boot_message = "")
{
    return Cli<Config, std::shared_ptr<std::tuple<Devs...>>, Commands...>(devs, boot_message);
}

template<typename Config, typename ... Devs>
auto make_default_cli(std::shared_ptr<std::tuple<Devs...>> devs, const char * boot_message = "")
{
    return Cli<Config, std::shared_ptr<std::tuple<Devs...>>, commands::List>(devs, boot_message);
}

} }

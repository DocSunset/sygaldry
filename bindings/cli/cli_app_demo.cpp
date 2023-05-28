#include "cli_app.hpp"

struct Device1 {
    static consteval auto name() { return "Test Device 1"; }
};

struct Device2 {
    static consteval auto name() { return "Test Device 2"; }
};

int main()
{
    CliApp<Device1, Device2> app{};
    app.main();
}

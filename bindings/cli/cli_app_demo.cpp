#include "cli_app.hpp"
#include "utilities/consteval/consteval.hpp"
#include "components/sensors/button/gesture_model.hpp"

struct Component1 {
    static _consteval auto name() { return "Test Component 1"; }
};

struct Component2 {
    static _consteval auto name() { return "Test Component 2"; }
};

int main()
{
    using namespace sygaldry::components;
    CliApp<Component1, Component2, ButtonGestureModel> app{};
    app.main();
}

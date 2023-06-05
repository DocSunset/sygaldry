#include "cli_app.hpp"
#include "utilities/consteval.hpp"
#include "components/sensors/button/gesture_model.hpp"
#include "components/tests/testcomponent.hpp"

struct Component1 {
    static _consteval auto name() { return "Test Component 1"; }
};

struct Component2 {
    static _consteval auto name() { return "Test Component 2"; }
};

using namespace sygaldry::components;

struct AppComponents {
    Component1 cpt1;
    Component2 cpt2;
    TestComponent tc;
    ButtonGestureModel bgm;
};

int main()
{
    CliApp<AppComponents> app{};
    app.main();
}

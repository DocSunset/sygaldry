#include <boost/pfr.hpp>
#include "../first_button_gesture_model/button_gesture_model.hpp"
template<typename... Processors> void print_everything(Processors... ps);
template<> void print_everything() {};
template<typename Processor, typename... Processors>
void print_everything(Processor processor, Processors... ps)
{
    auto print_one_thing = [](auto endpoint) {
        if constexpr (requires {endpoint.name();} )
        {
            if constexpr (requires {endpoint.value;})
                printf("    %s: %d\n", endpoint.name(), endpoint.value);
            else
            {
                printf("%s\n", endpoint.name());
            }

        }
    };
    boost::pfr::for_each_field(processor.inputs, print_one_thing);
    boost::pfr::for_each_field(processor.outputs, print_one_thing);
}

int main()
{
    ButtonGestureModel bgm{};
    print_everything(bgm);
    bgm.inputs.button_state.value = true;
    bgm();
    print_everything(bgm);
    bgm();
    print_everything(bgm);
    bgm.inputs.button_state.value = false;
    bgm();
    print_everything(bgm);
}

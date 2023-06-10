#include <components/esp32/button.hpp>

void button()
{
    sygaldry::components::esp32::Button<GPIO_NUM_23> button; 
    button.init();
    button();
}

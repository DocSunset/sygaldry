#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

extern "C" void app_main(void)
{
    for (;;)
    {
        printf("Hello world.\n");
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

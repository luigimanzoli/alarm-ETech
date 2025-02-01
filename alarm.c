#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "hardware/adc.h"
#include "pico/bootrom.h"
#include "hardware/pwm.h"

// Definição dos LEDs RGB
#define RLED_PIN 13
#define GLED_PIN 11
#define BLED_PIN 12

// Definição dos botões
#define BTNA_PIN 5
#define BTNB_PIN 6

bool all_leds = 0;

int contador = 0;
static volatile uint32_t last_time = 0; 

void init_all() {
    gpio_init(RLED_PIN);
    gpio_set_dir(RLED_PIN, GPIO_OUT);
    gpio_put(RLED_PIN, 0);

    gpio_init(GLED_PIN);
    gpio_set_dir(GLED_PIN, GPIO_OUT);
    gpio_put(GLED_PIN, 0);

    gpio_init(BLED_PIN);
    gpio_set_dir(BLED_PIN, GPIO_OUT);
    gpio_put(BLED_PIN, 0);

    gpio_init(BTNA_PIN);
    gpio_set_dir(BTNA_PIN, GPIO_IN);
    gpio_pull_up(BTNA_PIN);

}

void get_led(bool R, bool G, bool B) {
    gpio_put(RLED_PIN, R);
    gpio_put(GLED_PIN, G);
    gpio_put(BLED_PIN, B);
}

// Inicializa o sistema de clock
void inicializar_clock() {
    bool ok = set_sys_clock_khz(100000, false);
    if (ok) {
        printf("Clock set to %ld\n", clock_get_hz(clk_sys));
    } else {
        printf("Falha ao configurar o clock\n");
    }
}

void gpio_irq_handler(uint gpio, uint32_t events){

    uint32_t current_time = to_us_since_boot(get_absolute_time());
        if (current_time - last_time > 200000){
            last_time = current_time;

            }
}

// Função de callback para desligar o LED após o tempo programado.
int64_t turn_off_callback(alarm_id_t id, void *user_data) {

    printf("Alarme ativado. \n");

    if (gpio_get(RLED_PIN) && gpio_get(GLED_PIN) && gpio_get(BLED_PIN)){

        get_led(0,1,1);
        add_alarm_in_ms(3000, turn_off_callback, NULL, true);

    } else if (gpio_get(GLED_PIN) && gpio_get(BLED_PIN)){

        get_led(0,0,1);
        add_alarm_in_ms(3000, turn_off_callback, NULL, true);

    } else if (gpio_get(BLED_PIN)){

        get_led(0,0,0);

    }
}

// Função principal
int main() {
    // Inicializa clock, stdio e configurações
    stdio_init_all();
    init_all();
    inicializar_clock();

    printf("Sistema inicializado. Aguardando entrada...\n");

    // Configuração dos botões como interrupções
    gpio_set_irq_enabled_with_callback(BTNA_PIN, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);

    while (true) {
        if (!gpio_get(BTNA_PIN) && gpio_get(RLED_PIN) == 0 && gpio_get(GLED_PIN) == 0 && gpio_get(BLED_PIN) == 0){

            sleep_ms(50);

            get_led(1,1,1);

            add_alarm_in_ms(3000, turn_off_callback, NULL, true);
        }
        sleep_ms(50);
    }

    return 0;
}
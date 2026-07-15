#include <Arduino.h>
#include <lvgl.h>

#include "driver/i2c_master.h"
#include "face_detection/face_app.h"
#include "lcd/st7701_lcd.h"
#include "lvgl_port.h"
#include "touch/gt911_touch.h"

// Touch controller pins. This board wires no reset/backlight GPIO for the
// display panel itself (handled internally by the ST7701 driver), so only the
// touch controller pins are configured here.
#define TP_I2C_SDA 7
#define TP_I2C_SCL 8
#define TP_RST 3
#define TP_INT -1

// Display panel has no external reset line on this board.
static st7701_lcd lcd(-1);
static gt911_touch touch(TP_I2C_SDA, TP_I2C_SCL, TP_RST, TP_INT);

// Brings up the shared I2C master bus (port I2C_NUM_1) used by the touch
// controller and, later, the camera sensor.
static void i2c_bus_init() {
    i2c_master_bus_config_t cfg = {};
    cfg.i2c_port = I2C_NUM_1;
    cfg.sda_io_num = (gpio_num_t)TP_I2C_SDA;
    cfg.scl_io_num = (gpio_num_t)TP_I2C_SCL;
    cfg.clk_source = I2C_CLK_SRC_DEFAULT;
    cfg.glitch_ignore_cnt = 7;
    cfg.flags.enable_internal_pullup = 1;

    i2c_master_bus_handle_t bus = NULL;
    i2c_new_master_bus(&cfg, &bus);
}

void setup() {
    Serial.begin(115200);
    Serial.println("ESP32-P4 face detection starting");

    i2c_bus_init();
    lcd.begin();
    touch.begin();

    lvgl_port_init(lcd, touch);

    // Builds the UI and starts the camera + face-detection task. Camera
    // hardware is optional: without it the face tab shows an error state.
    face_app_start();
}

void loop() {
    lv_timer_handler();
    delay(5);
}

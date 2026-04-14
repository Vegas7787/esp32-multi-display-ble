#include <iostream>
#include <SPI.h>
#include <BleKeyboard.h>
#include <Preferences.h>
#include "Display_EPD_W21_spi.h"
#include "Display_EPD_W21.h"
#include "Ap_29demo.h"

using namespace std;

// Экран 1
#define EPD_BUSY_1 34
#define EPD_RST_1 16
#define EPD_DC_1 17
#define EPD_CS_1 5
#define BUTTON_PIN_1 4

// Экран 2
#define EPD_BUSY_2 35
#define EPD_RST_2 13
#define EPD_DC_2 12
#define EPD_CS_2 27
#define BUTTON_PIN_2 14

// Экран 3
#define EPD_BUSY_3 32
#define EPD_RST_3 33
#define EPD_DC_3 25
#define EPD_CS_3 26
#define BUTTON_PIN_3 21

enum State
{
    WAIT_HEADER_1,
    WAIT_HEADER_2,
    READ_SIZE,
    TYPE,
    READ_DATA,
    READ_SUM
};

State state = WAIT_HEADER_1;
BleKeyboard bleKeyboard;
Preferences prefs;
bool justConnected = false;
int key_id;
int k = 0;
int size_index = 0;
uint8_t type_byte;
uint8_t sum_calc = 0;
uint8_t sum_rec = 0;
uint8_t size_bytes[4];
uint8_t buffer_img[2024];
uint32_t size = 0;
uint32_t received = 0;
vector<uint8_t> action;
vector<uint8_t> new_action;
vector<display> displays{display(1, EPD_BUSY_1, EPD_RST_1, EPD_DC_1, EPD_CS_1, BUTTON_PIN_1),
                         display(2, EPD_BUSY_2, EPD_RST_2, EPD_DC_2, EPD_CS_2, BUTTON_PIN_2),
                         display(3, EPD_BUSY_3, EPD_RST_3, EPD_DC_3, EPD_CS_3, BUTTON_PIN_3)};

void get_img(uint8_t *buffer);
void show_img(int id);
void serialTask(void *param);
void bleTask(void *param);
void save_action(int id, vector<uint8_t> &action);
void load_action(int id, vector<uint8_t> &action);

void setup()
{

    // pinMode(SWITCH_PIN, INPUT_PULLUP); // активируем внутренний pull-up резистор

    pinMode(EPD_BUSY_1, INPUT);
    pinMode(EPD_RST_1, OUTPUT);
    pinMode(EPD_DC_1, OUTPUT);
    pinMode(EPD_CS_1, OUTPUT);
    pinMode(EPD_BUSY_2, INPUT);
    pinMode(EPD_RST_2, OUTPUT);
    pinMode(EPD_DC_2, OUTPUT);
    pinMode(EPD_CS_2, OUTPUT);
    pinMode(EPD_BUSY_3, INPUT);
    pinMode(EPD_RST_3, OUTPUT);
    pinMode(EPD_DC_3, OUTPUT);
    pinMode(EPD_CS_3, OUTPUT);
    pinMode(BUTTON_PIN_1, INPUT_PULLUP);
    pinMode(BUTTON_PIN_2, INPUT_PULLUP);
    pinMode(BUTTON_PIN_3, INPUT_PULLUP);

    Serial.begin(115200);
    Serial.setRxBufferSize(3000);

    // SPI
    SPI.begin(18, -1, 23, -1); // SCK, MISO (-1 если не нужен), MOSI, CS
    SPI.beginTransaction(SPISettings(10000000, MSBFIRST, SPI_MODE0));

    bleKeyboard.begin();

    for (display &d : displays)
    {
        load_action(d.id, d.action);
    }
    

    xTaskCreatePinnedToCore(serialTask, "serialTask", 4096, NULL, 1, NULL, 0);
    xTaskCreatePinnedToCore(bleTask, "bleTask", 4096, NULL, 1, NULL, 1);
}

void loop()
{

    if (k >= 3)
    {
        Serial.println(key_id);
        show_img(key_id);
        k = 0;
        key_id = 0;
    }
}

void serialTask(void *param)
{
    while (true)
    {
        if (Serial.available())
        {
            get_img(buffer_img);
        }
        vTaskDelay(1);
    }
}

void bleTask(void *param)
{
    while (true)
    {
        if (bleKeyboard.isConnected())
        {
            for (display &d : displays)
            {
                if (digitalRead(d.btn) == LOW)
                {

                    uint32_t now = millis();

                    if (now - d.lastPressTime < 200)
                        continue;

                    d.lastPressTime = now;

                    vector<uint8_t> &action = d.action;

                    if (action.size() == 1)
                    {
                        bleKeyboard.write(action[0]);
                    }
                    else if (action.size() > 1)
                    {
                        bleKeyboard.releaseAll();

                        for (uint8_t key : action)
                        {
                            if (key >= 0x80)
                            {
                                bleKeyboard.press(key);
                            }
                        }

                        for (uint8_t key : action)
                        {
                            if (key < 0x80)
                            {
                                bleKeyboard.press(key);
                            }
                        }

                        vTaskDelay(80);
                        bleKeyboard.releaseAll();
                    }
                }
            }
        }
        vTaskDelay(1);
    }
}

void show_img(int id)
{
    for (display &d : displays)
    {
        if (d.id == id)
        {
            d.select();
            EPD_HW_Init(d);
            EPD_Load_Data(d.image, d);
            d.deselect();

            d.select();
            EPD_Update_NoWait(d); // запускает обновление
            d.deselect();

            Epaper_READBUSY(d);

            d.select();
            EPD_DeepSleep(d);
            d.deselect();
        }
    }
}

void get_img(uint8_t *buffer)
{

    while (Serial.available())
    {

        uint8_t b = Serial.read();

        switch (state)
        {
        case WAIT_HEADER_1:
            if (b == 0xAA)
                state = WAIT_HEADER_2;
            break;

        case WAIT_HEADER_2:
            if (b == 0x55)
                state = TYPE;
            else
                state = WAIT_HEADER_1;
            break;

        case TYPE:
            type_byte = b;
            if ((b & 0xF0) == 0x00)
            {
                received -= size;
                k--;
            }
            state = READ_SIZE;
            break;

        case READ_SIZE:
            size_bytes[size_index++] = b;

            if (size_index == 4)
            {
                memcpy(&size, size_bytes, 4);
                size_index = 0;
                state = READ_DATA;
            }
            break;

        case READ_DATA:
            switch (type_byte & 0x0F)
            {
            case 0x00:
                key_id = (int)b;
                sum_calc += b;
                state = READ_SUM;
                break;
            case 0x01:
                displays[key_id - 1].image[received++] = b;
                sum_calc += b;

                if (received >= size)
                {
                    state = READ_SUM;
                }
                break;
            case 0x02:
                new_action.push_back(b);
                received++;
                sum_calc += b;

                if (received >= size)
                {
                    state = READ_SUM;
                }
                break;
            }
            break;

        case READ_SUM:
            sum_rec = b;

            if (sum_calc == sum_rec)
                Serial.write(0x06);
            else
                Serial.write(0x15);

            state = WAIT_HEADER_1;
            displays[key_id - 1].action.clear();
            displays[key_id - 1].action = new_action;
            save_action(key_id, new_action);
            new_action.clear();
            k++;
            received = 0;
            sum_calc = 0;
            size = 0;
            size_index = 0;
            break;
        }
    }
}

void save_action(int id, vector<uint8_t> &action)
{
    prefs.begin("keys", false);
    String key = "act" + String(id);

    prefs.putBytes(key.c_str(), action.data(), action.size());

    prefs.putUChar((key + "_size").c_str(), action.size());

    prefs.end();
}

void load_action(int id, vector<uint8_t> &action)
{
    prefs.begin("keys", true);

    String key = "act" + String(id);
    String sizeKey = key + "_size";

    uint8_t size = prefs.getUChar(sizeKey.c_str(), 0);

    if (size > 0)
    {
        action.resize(size);
        prefs.getBytes(key.c_str(), action.data(), size);
    }

    prefs.end();
}
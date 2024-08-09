#include <DFPlayer_Mini_Mp3.h>
#include <string.h>
#include <SoftwareSerial.h>

#define BUTTON_1_PIN 6
#define BUTTON_2_PIN 8

// Default Sound
#define SPARK_OFF  0
#define GUN_MODE   1
#define GUN_SHOT   2
#define SPARK_ON   3
#define ERROR      4
#define KEY_REMOVE 5
#define STARTUP    6

// TRIGGER MODE(EVERY TYPE UNIQUE)
#define TRIGGER_TYPE     1
#define BOOT_UP          2
#define BOOTUP_SHOT      3
#define TRIGGER_SPARK_ON 4
#define HENSHIN_TRIGGER  5

// TRIGGER/TIGA TYPE LIST
#define MULTI_TYPE      1
#define POWER_TYPE      2
#define SKY_TYPE        3
#define TRIGGER_TRUTH   4
#define GLITTER_TRIGGER 5
#define HYPER_GUN_MODE  6

// Z ULTRA FUSION LIST
#define ORIGINAL       7
#define ALPHA_EDGE     8
#define BETA_SMASH     9
#define GAMMA_FUTURE   10
#define DELTARISE_CLAW 11

// TIGA TYPE LIST
#define TIGA_MULTI    12
#define TIGA_POWER    13
#define TIGA_SKY      14
#define TIGA_GLITTER  15 

#define N_BUTTON 2
#define ON LOW
#define OFF HIGH

SoftwareSerial mySerial(5, 3); // RX, TX

const uint8_t OFF_OFF[] = {OFF, OFF};
const uint8_t ON_OFF[] = {ON, OFF};
const uint8_t OFF_ON[] = {OFF, ON};
const uint8_t ON_ON[] = {ON, ON};

const uint8_t trigger_type[] = {MULTI_TYPE, POWER_TYPE, SKY_TYPE, TRIGGER_TRUTH, GLITTER_TRIGGER};
const uint8_t tiga_type[] = {TIGA_MULTI, TIGA_POWER, TIGA_SKY, TIGA_GLITTER};
const uint8_t z_fusion[] = {ORIGINAL, ALPHA_EDGE, BETA_SMASH, GAMMA_FUTURE, DELTARISE_CLAW};
uint8_t type = HYPER_GUN_MODE;
uint8_t currentIndex;

void setup()
{
    mySerial.begin(9600); // set SoftwareSerial for DFPlayerMP3
    mp3_set_serial(mySerial);
    mp3_set_volume(28);
    mp3_play_file_in_folder(HYPER_GUN_MODE, STARTUP);
    Serial.begin(9600);
    Serial.println("START");

    pinMode(BUTTON_1_PIN, INPUT_PULLUP);
    pinMode(BUTTON_2_PIN, INPUT_PULLUP);
}

const unsigned long longPressDuration = 1000; // Threshold duration for long press (1000 milliseconds = 1 second)
const unsigned long cycleInterval = 1000;     // Interval to cycle through array elements during a long press
unsigned long buttonPressTime = 0;            // Variable to store the time when the button is pressed
unsigned long lastCycleTime = 0;              // Variable to store the last time we cycled the array

uint8_t before_state[2] = {};
uint8_t current_state[2] = {};
uint8_t hyperkey_mode_flag = OFF;
uint8_t hyperkey_bootup_flag = OFF;
uint8_t action = SPARK_OFF;

void loop()
{
    current_state[0] = digitalRead(BUTTON_1_PIN);
    current_state[1] = digitalRead(BUTTON_2_PIN);

    // SPARKLENCE MODE
    if (memcmp(before_state, OFF_OFF, N_BUTTON) == 0)
    {
        if (memcmp(current_state, OFF_OFF, N_BUTTON) == 0)
        {
            ;
        }
        else if (memcmp(current_state, ON_OFF, N_BUTTON) == 0)
        {
            // BERUBAH KE TRIGGER
            if (hyperkey_mode_flag == OFF)
            {
                Serial.println("ERROR!");
                action = set_action(HYPER_GUN_MODE, ERROR);
            }
            else
            {
                action = set_action(type, HENSHIN_TRIGGER);
                Serial.println("TRIGGER HENSHIN!");
                delay(15000);
                // Tambah Flag?
            }
        }
        else if (memcmp(current_state, OFF_ON, N_BUTTON) == 0)
        {
            // KEMBALI KE HYPERGUN MODE
            action = set_action(HYPER_GUN_MODE, GUN_MODE);
            Serial.println("HYPERGUN MODE!");
        }
        else if (memcmp(current_state, ON_ON, N_BUTTON) == 0)
        {
            ;
        }
    }
    else if (memcmp(before_state, ON_OFF, N_BUTTON) == 0)
    {
        if (memcmp(current_state, OFF_OFF, N_BUTTON) == 0)
        {
            ;
        }
        else if (memcmp(current_state, ON_OFF, N_BUTTON) == 0)
        {
            ;
        }
        else if (memcmp(current_state, OFF_ON, N_BUTTON) == 0)
        {
            ;
        }
        else if (memcmp(current_state, ON_ON, N_BUTTON) == 0)
        {
            ;
        }
    }

    // HYPERGUN MODE
    else if (memcmp(before_state, OFF_ON, N_BUTTON) == 0)
    {
        if (memcmp(current_state, OFF_OFF, N_BUTTON) == 0)
        {
            // KE SPARKLENCE MODE
            if (hyperkey_mode_flag == OFF)
            {
                Serial.println("SPARKLENCE MODE ON!");
                action = set_action(HYPER_GUN_MODE, SPARK_ON);
            }
            else
            {
                Serial.println("HYPERKEY DETECTED! SPARKLENCE MODE ON!");
                action = set_action(type, TRIGGER_SPARK_ON);
            }
        }
        else if (memcmp(current_state, ON_OFF, N_BUTTON) == 0)
        {
            if (hyperkey_mode_flag == OFF)
            {
                action = set_action(HYPER_GUN_MODE, SPARK_ON);
            }
            else
            {
                action = set_action(type, TRIGGER_SPARK_ON);
            }
        }
        else if (memcmp(current_state, OFF_ON, N_BUTTON) == 0)
        {
            ;
        }
        else if (memcmp(current_state, ON_ON, N_BUTTON) == 0)
        {
            buttonPressTime = millis(); // Record the time when the button is pressed
        }
    }
    else if (memcmp(before_state, ON_ON, N_BUTTON) == 0)
    {
        if (memcmp(current_state, OFF_OFF, N_BUTTON) == 0)
        {
        }
        else if (memcmp(current_state, ON_OFF, N_BUTTON) == 0)
        {
        }
        else if (memcmp(current_state, OFF_ON, N_BUTTON) == 0)
        {
            if (hyperkey_bootup_flag == OFF)
            {
                if (hyperkey_mode_flag == OFF)
                {
                    action = set_action(HYPER_GUN_MODE, GUN_SHOT);
                    Serial.println("SHOT!");
                }
                else
                {
                    action = set_action(HYPER_GUN_MODE, KEY_REMOVE);
                    Serial.println("KEY REMOVED!");
                    hyperkey_mode_flag = OFF;
                    currentIndex = 0;
                }
            }
            else
            {
                if (hyperkey_mode_flag == OFF)
                {
                    if (type == MULTI_TYPE)
                    {
                        action = set_action(type, BOOT_UP);
                        Serial.println("BOOT UP! ZEPERION");
                    }
                    else if (type == POWER_TYPE)
                    {
                        action = set_action(type, BOOT_UP);
                        Serial.println("BOOT UP! DERACIUM");
                    }
                    else if (type == SKY_TYPE)
                    {
                        action = set_action(type, BOOT_UP);
                        Serial.println("BOOT UP! RUNBOLT");
                    }
                    else if (type == TRIGGER_TRUTH)
                    {
                        action = set_action(type, BOOT_UP);
                        Serial.println("BOOT UP! TRUE ZEPERION");
                    }
                    else if (type == GLITTER_TRIGGER)
                    {
                        action = set_action(type, BOOT_UP);
                        Serial.println("BOOT UP! GLITTER ZEPERION");
                    }

                    hyperkey_mode_flag = ON;
                }
                else
                {
                    action = set_action(type, BOOTUP_SHOT);
                    Serial.println("ZEPERION SHOOT!");
                }
            }
        }
        else if (memcmp(current_state, ON_ON, N_BUTTON) == 0)
        {
            if (hyperkey_mode_flag == OFF)
            {
                unsigned long pressDuration = millis() - buttonPressTime;
                // GANTI TYPE/MODE HYPER KEY
                if (pressDuration >= longPressDuration)
                { // Check if it is a long press
                    if (millis() - lastCycleTime >= cycleInterval)
                    {                             // Check if it is time to cycle the array
                        lastCycleTime = millis(); // Update the last cycle time
                        type = trigger_type[currentIndex];
                        action = set_action(type, TRIGGER_TYPE);
                        Serial.print("Cycled mode: ");
                        Serial.println(trigger_type[currentIndex]);
                        currentIndex++;
                        if (currentIndex >= sizeof(trigger_type) / sizeof(trigger_type[0]))
                        {
                            currentIndex = 0;
                        }
                    }
                    hyperkey_bootup_flag = ON;
                }
            }
            else
            {
                unsigned long pressDuration = millis() - buttonPressTime;
                // GANTI TYPE/MODE HYPER KEY
                if (pressDuration >= longPressDuration)
                { // Check if it is a long press
                    hyperkey_bootup_flag = OFF;
                }
            }
        }
    }

    before_state[0] = current_state[0];
    before_state[1] = current_state[1];
    delay(100);
}

uint8_t set_action(uint8_t folder, uint8_t action)
{
    mp3_play_file_in_folder(folder, action);
    return action;
}

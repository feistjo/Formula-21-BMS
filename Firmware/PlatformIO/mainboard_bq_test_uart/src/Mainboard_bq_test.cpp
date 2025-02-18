#include <Arduino.h>

#define BQTEST
// #define serialdebug 1

#include "bq_comm.h"
#include "teensy_pin_defs.h"
#include "thermistor.h"

NXFT15XH103FA2B050 bq_thermistor{};
BQ79656 bq(Serial8, 35, bq_thermistor);
std::vector<float> voltages(20, 0);
std::vector<float> temperatures(16, 0);
std::vector<float> current(1, 0);

void setup()
{
    pinMode(contactorprecharge_ctrl, OUTPUT);
    pinMode(contactorp_ctrl, OUTPUT);
    pinMode(contactorn_ctrl, OUTPUT);
    pinMode(coolant_ctrl, OUTPUT);
    digitalWrite(contactorprecharge_ctrl, LOW);
    digitalWrite(contactorp_ctrl, LOW);
    digitalWrite(contactorn_ctrl, LOW);
    digitalWrite(coolant_ctrl, LOW);
    Serial.begin(9600);
    delay(5000);
    Serial.println("Starting");
    digitalWrite(contactorprecharge_ctrl, HIGH);
    digitalWrite(contactorp_ctrl, HIGH);
    digitalWrite(contactorn_ctrl, HIGH);
    digitalWrite(coolant_ctrl, HIGH);

    /* Serial2.begin(1000000, SERIAL_8N1_HALF_DUPLEX);
    while (1)
    {
        Serial2.println("Test");
        delay(100);
    } */
    // delay(5);
    bq.SetStackSize(2);
    bq.Initialize();
    bq.SetProtectors(4.20f, 2.50f, 60.0f, -40.0f);
    delay(1000);
}

void loop()
{
    // bqGetReg(byte req_type, byte data_size, byte dev_addr, uint16_t reg_addr, byte resp_size)
    /* uint8_t* bqResponse = bqReadReg(BQ_SINGLE_READ, 0, BRIDGE_COMM_TIMEOUT, 1);
     Serial.println("Response:");
     for (int i = 0; i < 7; i++) {
       Serial.print(((uint8_t[][176])bqResponse)[0][i], HEX);
       Serial.print(" ");
     }
     Serial.println();
     delay(1000);*/
    std::vector<byte> byteArr{0b0010010};
    // bq.Comm(BQ79656::request_type::SINGLE_WRITE, 1, 0, BRIDGE_COMM_TIMEOUT, byteArr);

    // Stack test
    Serial.println("Stack test");
    // Serial.println("Test auto-addressing");
    // bq.AutoAddressing(1);
    while (1)
    {
        Serial.println("Test read voltages");
        bq.GetVoltages(voltages);
        Serial.print(voltages[0]);
        for (size_t i = 1; i < voltages.size(); i++)
        {
            Serial.print(", ");
            Serial.print(voltages[i]);
        }
        Serial.println("\nDone");
        delay(1000);
        Serial.println("Test read current" /*and temperatures*/);

        Serial.print("Current: ");
        bq.GetCurrent(current);
        Serial.print(current[0]);
        Serial.println("A");
        delay(500);
        // bq.ProcessBalancing(voltages);
        bq.ProcessBalancingSimple(millis());
        delay(10);
        /* bq.GetTemps(temperatures);
        Serial.println("Temperatures: ");
        Serial.print(temperatures[0]);
        Serial.print("C");
        for (size_t i = 1; i < temperatures.size(); i++)
        {
            Serial.print(", ");
            Serial.print(temperatures[i]);
            Serial.print("C");
        } */
    }
    /* byteArr[0] = 0x00;
    bq.Comm(BQ79656::RequestType::BROAD_WRITE, 1, 0, BQ79656::RegisterAddress::OTP_ECC_TEST, byteArr);
    byteArr[0] = 0x01;
    bq.Comm(BQ79656::RequestType::BROAD_WRITE, 1, 0, BQ79656::RegisterAddress::CONTROL1, byteArr);
    byteArr[0] = 0x00;
    bq.Comm(BQ79656::RequestType::BROAD_WRITE, 1, 0, BQ79656::RegisterAddress::DIR0_ADDR, byteArr);
    byteArr[0] = 0x01;
    bq.Comm(BQ79656::RequestType::BROAD_WRITE, 1, 0, BQ79656::RegisterAddress::DIR0_ADDR, byteArr);
    byteArr[0] = 0x00;
    // bqComm(BQ_SINGLE_WRITE, 1, 0, BRIDGE_COMM_CTRL, byteArr);
    byteArr[0] = 0x02;
    bq.Comm(BQ79656::RequestType::BROAD_WRITE, 1, 0, BQ79656::RegisterAddress::COMM_CTRL, byteArr);
    byteArr[0] = 0x03;
    bq.Comm(BQ79656::RequestType::SINGLE_WRITE, 1, 1, BQ79656::RegisterAddress::COMM_CTRL, byteArr);
    bq.DummyReadReg(BQ79656::RequestType::BROAD_READ, 0, BQ79656::RegisterAddress::OTP_ECC_TEST, 1);
    Serial.println("Auto-addressing finished. Testing single read:");
    // bqReadReg(BQ_SINGLE_READ, 0, BRIDGE_COMM_TIMEOUT, 1);
    bq.ReadReg(BQ79656::RequestType::SINGLE_READ, 1, BQ79656::RegisterAddress::COMM_CTRL, 1); */

    // Serial.println("Enabling balance with 300s timer");

    // bqWakeChip();
    // send wake

    // byteArr[0] = 0b00100000;
    // bqComm(BQ_SINGLE_WRITE, 1, 0, BRIDGE_CONTROL1, byteArr);
    delay(50000000);

    // sync DLL
    for (byte i = 0; i < 8; i++)
    {
        byteArr[0] = 0x00;
        bq.Comm(BQ79656::RequestType::STACK_WRITE, 1, 0, static_cast<BQ79656::RegisterAddress>(0x343 + i), byteArr);
    }

    // int seriesPerSegment = num_series / num_segments;
    //  set up balancing time control registers to 300s (0x04)
    /* std::vector<byte> balTimes(seriesPerSegment, 0x04);
    bq.Comm(BQ79656::RequestType::STACK_WRITE,
            seriesPerSegment,
            0,
            static_cast<BQ79656::RegisterAddress>(static_cast<uint16_t>(BQ79656::RegisterAddress::CB_CELL1_CTRL) + 1
                                                  - seriesPerSegment),
            balTimes); */

    // start balancing with AUTO_BAL to automatically cycle between even/odd
    std::vector<byte> startBal{0b00000011};
    bq.Comm(BQ79656::RequestType::STACK_WRITE, 1, 0, BQ79656::RegisterAddress::BAL_CTRL2, startBal);
    delay(50000);

    // bqAutoAddressing(1);
    // byte byteArr[] = {0b00100000};
    // bqComm(BQ_SINGLE_WRITE, 1, 0, BRIDGE_CONTROL1, byteArr);
    // bqSetStackSize(1);

    /*while (1) {
      byte byteArr[] = {0};
      bqComm(BQ_BROAD_READ, 0, 0, 0x032F, byteArr);
      bqCommClear();
      delay(1);
    }*/

    /*bqReadReg(BQ_BROAD_READ, 0, 0x032F, 1);
    for (int i = 0; i < 7; i++) {
      Serial.print(((uint8_t[][176])bqResponse)[0][i], HEX);
      Serial.print(" ");
    }
    Serial.println();
    for (int i = 0; i < 7; i++) {
      Serial.print(((uint8_t[][176])bqResponse)[1][i], HEX);
      Serial.print(" ");
    }
    Serial.println();*/
    delay(10000);
}

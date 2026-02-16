# ESP32 LoRa Environmental & Health Monitoring System

Long-range multi-sensor monitoring system using ESP32 LoRa nodes for environmental and physiological data transmission.

The system consists of:

- **Transmitter node** → collects sensor data  
- **Receiver node** → receives LoRa packets and outputs data  

## Features

- LoRa wireless telemetry (433 MHz)
- Air quality sensing (MQ135)
- Temperature & humidity monitoring (DHT11)
- SpO₂ monitoring (MAX30105)
- Multi-sensor data fusion
- Long-range wireless transmission
- Real-time remote monitoring

## System Architecture

Sensors → ESP32 LoRa Transmitter → LoRa RF → ESP32 LoRa Receiver → Serial Monitor / Gateway

## Hardware Components

### Transmitter Node

- ESP32 LoRa board (SX1276/78)
- MQ135 air quality sensor
- DHT11 temperature/humidity sensor
- MAX30105 pulse oximeter
- 433 MHz antenna

### Receiver Node

- ESP32 LoRa board (SX1276/78)
- 433 MHz antenna

## Pin Connections (Transmitter ESP32)

| Device | ESP32 Pin |
|--------|----------|
MQ135 Analog | GPIO34 |
DHT11 Data | GPIO4 |
LoRa SS | GPIO5 |
LoRa RST | GPIO14 |
LoRa DIO0 | GPIO2 |
MAX30105 SDA | GPIO21 |
MAX30105 SCL | GPIO22 |

## Pin Connections (Receiver ESP32)

| LoRa | ESP32 |
|------|------|
SS | GPIO5 |
RST | GPIO14 |
DIO0 | GPIO2 |

## LoRa Configuration

Frequency: **433 MHz**  
Sync Word: **0xA5**  

These settings must match on transmitter and receiver.

## Transmitted Data Format

Example packet:

AirQuality:245.3, Temp:28.5, Humidity:63.2, SpO2:97, Oxygen Concentration:20.91


Fields:

- AirQuality → MQ135 PPM
- Temp → °C
- Humidity → %
- SpO₂ → %
- Oxygen Concentration → %

## Operation

### Transmitter

1. Sensors initialized  
2. MQ135 calibrated  
3. MAX30105 samples collected  
4. SpO₂ computed  
5. Environmental data read  
6. Data packet sent via LoRa  

### Receiver

1. LoRa initialized  
2. Waits for packets  
3. Reads payload  
4. Outputs to Serial  

## Libraries Required

- LoRa
- MAX30105
- DHT sensor library
- MQ135
- Wire
- SPI

## Applications

- Remote health monitoring
- Environmental sensing networks
- Smart agriculture
- Air quality monitoring
- Biomedical IoT
- Disaster monitoring systems


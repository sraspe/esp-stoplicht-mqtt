# Stoplicht controller voor het SOC
Requires libraries ESPWIFI and Pubsubclient
only listnes for a binary state 1/0 on topic stoplicht

#Basic MQTT stoplicht
Use ESP8266 for hardware
PIN0 is green
PIN2 is red

## Status table
|State | desc | Green | Red|
|0 |   off    | Off |Off|
|1 | Critical | Off | Blink Continuously|
|2 | Urgent   | Off | ON|
|3 | Warning  | On | Blink Continously|
|4 | Notify   | On | Blink 3x|
|5 | Prepare  | Blink Continuously | Off|
|6 | Party    | Blink Continously | Blink Continously|
|7 | OK       |On | Off|

# Azure RTOS NetXDuo For Arduino

This is a port of Azure RTOS NetXDuo to Arduino as a Library. For more information about Azure RTOS, please visit [Microsoft Doc](https://docs.microsoft.com/en-us/azure/rtos/netx-duo/) and source code on [Github](https://github.com/azure-rtos/netxduo).

## Hardware support

The port and provided demo is verified on following board and Arduino Core. 

| Board | Chip | Architecture | Verified Arduino Core | Network |
| - | - | - | - | - |
| [B-L4S5I-IOT01A](https://www.st.com/en/evaluation-tools/b-l4s5i-iot01a.html) | [STM32L4S5](https://www.st.com/zh/microcontrollers-microprocessors/stm32l4r5-s5.html) | Cortex-M4F | [stm32duino/Arduino_Core_STM32 2.0.0](https://github.com/stm32duino/Arduino_Core_STM32) | [Inventek ISM43362-M3G-L44 module](https://www.inventeksys.com/ism4336-m3g-l44-e-embedded-serial-to-wifi-module/)

## Version

The version of this library is evolving independent to Azure RTOS NetX Duo version. Here is a tracking table:

| Library version | ThreadX version |
| - | - |
| v1.0.0 | [v6.1.8](https://github.com/azure-rtos/netxduo/tree/v6.1.8_rel)  |

## Azure IoT Example

A [example code](./examples/demo_azure_iot/demo_azure_iot.ino) is provided together with this library to show user how to use [Azure IoT middleware for Azure RTOS](https://github.com/azure-rtos/netxduo/tree/master/addons/azure_iot) addons in NetXDuo to  to Azure IoT. 

Refer to this [page](https://github.com/azure-rtos/netxduo/tree/master/addons/azure_iot/samples) to setup resources and credentials

> For WiFi based board, user should also define WiFi SSID and WiFi Password in the beginning part of .ino. 

## License

This repository inherit Azure RTOS license from Microsoft. See [LICENSE.txt](./LICENSE.txt) and [LICENSED-HARDWARE.txt](./LICENSED-HARDWARE.txt).

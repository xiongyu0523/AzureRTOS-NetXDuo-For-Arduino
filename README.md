# Azure RTOS NetXDuo For Arduino

This is a port of Azure RTOS NetXDuo to Arduino as a Library. For more information about Azure RTOS, please visit Microsoft Doc and source code on Github.

## Hardware support

The port and provided demo is verified on following board and Arduino Core. 

| Board | Chip | Architecture | Verified Arduino Core | 
| - | - | - | -|
| [B-L4S5I-IOT01A](https://www.st.com/en/evaluation-tools/b-l4s5i-iot01a.html) | [STM32L4S5](https://www.st.com/zh/microcontrollers-microprocessors/stm32l4r5-s5.html) | Cortex-M4F | [stm32duino/Arduino_Core_STM32 2.0.0](https://github.com/stm32duino/Arduino_Core_STM32)

## Version

The version of this library is evolving independent to Azure RTOS NetX Duo version. Here is a tracking table:

| Library version | ThreadX version |
| - | - |
| v1.0.0 | [v6.1.8](https://github.com/azure-rtos/netxduo/tree/v6.1.8_rel)  |

## License

This repository inherit Azure RTOS license from Microsoft. See [LICENSE.txt](./LICENSE.txt) and [LICENSED-HARDWARE.txt](./LICENSED-HARDWARE.txt).

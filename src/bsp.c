#ifdef ARDUINO_B_L4S5I_IOT01A
#include "../ports/bsp/b_l4s5i_iot01a/es_wifi_io.c"
#include "../ports/bsp/b_l4s5i_iot01a/es_wifi.c"
#include "../ports/bsp/b_l4s5i_iot01a/wifi.c"
#include "../ports/bsp/b_l4s5i_iot01a/board_setup.c"
#include "../ports/bsp/b_l4s5i_iot01a/wifi_setup.c"
#include "../ports/bsp/b_l4s5i_iot01a/nx_driver_stm32l4.c"
#include "../ports/bsp/b_l4s5i_iot01a/sample_network.c"
#else
#error "Board is not supported by this library"
#endif
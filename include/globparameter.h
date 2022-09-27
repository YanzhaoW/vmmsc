#pragma once
#ifndef GLOBPARAMETER
#define GLOBPARAMETER

#include <vector>
#include <string>
#define CONFIG_DIR "configs"

#define RINGS_PER_DAQ 12 // max number, can be actived/deactivated
#define FENS_PER_RING 4 // max number, can be actived/deactivated
#define FECS_PER_DAQ 8 // max number, can be actived/deactivated
#define HYBRIDS_PER_FEC 8 // max number, can be actived/deactivated
#define VMMS_PER_HYBRID 2 // max number, can be actived/deactivated

#define VMM_CHANNELS 64

extern std::vector<int> g_hybrid_i2c;
extern int g_clock_source;
extern int g_slow_control;
extern bool g_use_config_check;
extern double g_clock_period;
extern double g_time_factor;
extern bool g_connection_ok;
extern std::string g_card_name;
#endif // GLOBPARAMETER

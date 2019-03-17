#ifndef SUSTAIN_PFCNW_CONSTANTS_H
#define SUSTAIN_PFCNW_CONSTANTS_H

#ifdef _WIN32
#include <sdkddkver.h>
#endif


namespace pfc {

//! Networking Constants
constexpr short g_pfc_registry_reg_port = 30001;
constexpr short g_pfc_registry_announce_port = 30002;
};

#endif //SUSTAIN_PFCNW_CONSTANTS_H

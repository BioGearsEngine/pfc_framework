#ifndef SUSTAIN_PFCNW_CONSTANTS_H
#define SUSTAIN_PFCNW_CONSTANTS_H

#include <sdkddkver.h>

#ifdef _WIN32
#define _WIN32_WINT _WIN32_WINNT_WIN10
#endif

namespace pfc {

//! Networking Constants
constexpr short g_pfc_registry_multicast_port = 30001;

};

#endif //SUSTAIN_PFCNW_CONSTANTS_H

#ifndef SUSTAIN_PFCNW_CONSTANTS_H
#define SUSTAIN_PFCNW_CONSTANTS_H

/**************************************************************************************
copyright 2019 applied research associates, inc.
licensed under the apache license, version 2.0 (the "license"); you may not use
this file except in compliance with the license. you may obtain a copy of the license
at:
http://www.apache.org/licenses/license-2.0
unless required by applicable law or agreed to in writing, software distributed under
the license is distributed on an "as is" basis, without warranties or
conditions of any kind, either express or implied. see the license for the
specific language governing permissions and limitations under the license.
**************************************************************************************/


#ifdef _WIN32
#include <sdkddkver.h>
#endif


namespace pfc {

//! Networking Constants
constexpr short g_pfc_registry_reg_port = 30001;
constexpr short g_pfc_registry_announce_port = 30002;
};

#endif //SUSTAIN_PFCNW_CONSTANTS_H

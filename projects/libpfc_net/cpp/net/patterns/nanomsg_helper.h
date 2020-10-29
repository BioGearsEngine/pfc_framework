#ifndef SUSTAIN_FRAMEWORK_NET_NANOMSG_HELPER_H
#define SUSTAIN_FRAMEWORK_NET_NANOMSG_HELPER_H

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

/*! \file */

#include <sustain/framework/util/Error.h>

#include <nanomsg/nn.h>

namespace pfc {

//!  
//!  Converts nanomsg Errors to PFC Errors
//!  \param code [IN] Known NanoMSG Error
//!  \return Error - The convert PFC error message
inline Error nano_to_Error(int code)
{
  Error l_ec;
  switch (code) {

  case EAFNOSUPPORT:
    l_ec = Error::PFC_ADDRESS_FAMILY_NOT_SUPPORTED;
    break;
  case EINVAL:
    l_ec = Error::PFC_PROTOCOL_NOT_SUPPORTED;
    break;
  case EMFILE:
    l_ec = Error::PFC_SOCKET_LIMIT_REACHED;
    break;
  case ETERM:
    l_ec = Error::PFC_LIBRARY_SHUTDOWN;
    break;
  case EBADF:
    l_ec = Error::PFC_INVALID_SOCKET;
    break;
  case ENAMETOOLONG:
    l_ec = Error::PFC_BIND_ERROR;
    break;
  case EPROTONOSUPPORT:
    l_ec = Error::PFC_PROTOCOL_NOT_SUPPORTED;
    break;
  case EADDRNOTAVAIL:
    l_ec = Error::PFC_INVALID_ENDPOINT;
    break;
  case ENODEV:
    l_ec = Error::PFC_BIND_ERROR;
    break;
  case EADDRINUSE:
    l_ec = Error::PFC_ADDRESS_IN_USE;
    break;
  case ENOTSUP:
    l_ec = Error::PFC_BAD_OPERATION;
    break;
  case EFSM:
    l_ec = Error::PFC_BAD_OPERATION;
    break;
  case EAGAIN:
    l_ec = Error::PFC_BAD_OPERATION;
    break;
  case EINTR:
    l_ec = Error::PFC_INTERUPT;
    break;
  case ETIMEDOUT:
    l_ec = Error::PFC_TIMEOUT;
    break;
  default:
    break;
  }
  return l_ec;
}
//-------------------------------------------------------------------------------
}
#endif //SUSTAIN_FRAMEWORK_NET_NANOMSG_HELPER_H

#ifndef SUSTAIN_FRAMEWORK_NET_NANOMSG_HELPER_H
#define SUSTAIN_FRAMEWORK_NET_NANOMSG_HELPER_H

#include <sustain/framework/util/Error.h>

#include <nanomsg/nn.h>

namespace pfc {
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
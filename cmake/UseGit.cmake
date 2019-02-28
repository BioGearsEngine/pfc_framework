#Macros for using GIT -- Requires Git

if(__sustain_git_modlue_support)
  return()
endif()
set(__sustain_git_modlue_support YES)


function(git_describe VERSION_MAJOR VERSION_MINOR VERSION_PATCH VERSION_SHA1 VERSION_SHORT)
  if( NOT GIT_FOUND )
    find_package(GIT REQUIRED)
  endif()

execute_process(COMMAND
    "${GIT_EXECUTABLE} describe --tags --dirty --long"
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    RESULT_VARIABLE __git_error
    OUTPUT_VARIABLE __git_output
    ERROR_QUIET
    OUTPUT_STRIP_TRAILING_WHITESPACE)


if(NOT __git_error )
    message(STATUS "${__git_error}")
endif()

string(REGEX REPLACE "^v([0-9]+)\\..*" "\\1" __major "${__git_output}")
string(REGEX REPLACE "^v[0-9]+\\.([0-9]+).*" "\\1" __minor "${__git_output}")
string(REGEX REPLACE "^v[0-9]+\\.[0-9]+\\.([0-9]+).*" "\\1" __patch "${__git_output}")
string(REGEX REPLACE "^v[0-9]+\\.[0-9]+\\.[0-9]+g(.*)" "\\1" __sha1 "${__git_output}")
string(REGEX REPLACE "^v[0-9]+\\.[0-9]+\\.[0-9]+g.*-(dirty)" "\\1" __dirty "${__git_output}")
set(__version_short "${__major}.${__minor}.${_patch}")

set(GIT_MAJOR_VERSION ${__major} PARENT_SCOPE)
set(GIT_MINOR_VERSION ${__minor} PARENT_SCOPE)
set(GIT_PATCH_VERSION ${__patch} PARENT_SCOPE)
set(GIT_SHA1_VERSION ${__sha1} PARENT_SCOPE)
set(GIT_REV_RESULT ${__git_output} PARENT_SCOPE)

endfunction()


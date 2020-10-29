enable_language(CXX)

if(WIN32)
  set(LIBRARY_INSTALL_DIR bin)
else()
  set(LIBRARY_INSTALL_DIR lib)
endif()

set(CMAKE_STATIC_LIBRARY_PREFIX "lib")
set(CMAKE_STATIC_LIBRARY_SUFFIX "_st${CMAKE_STATIC_LIBRARY_SUFFIX}")
set(CMAKE_SHARED_LIBRARY_PREFIX "lib")

  
set(OUTPUT_PREFIX ${CMAKE_BINARY_DIR}/outputs)
if(UNIX)
    foreach(OUTPUTCONFIG Release Debug)
        string(TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG_UPPER)
        set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG_UPPER} "${OUTPUT_PREFIX}/lib")
        set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG_UPPER} "${OUTPUT_PREFIX}/lib")
        set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG_UPPER} "${OUTPUT_PREFIX}/bin")
    endforeach(OUTPUTCONFIG)
else()
    foreach(OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES})
        string(TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG_UPPER)
        set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG_UPPER} "${OUTPUT_PREFIX}/${OUTPUTCONFIG}/lib")
        set(CMAKE_PDB_OUTPUT_DIRECTORY_${OUTPUTCONFIG_UPPER} "${OUTPUT_PREFIX}/${OUTPUTCONFIG}/lib")
        set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG_UPPER} "${OUTPUT_PREFIX}/${OUTPUTCONFIG}/bin")
        set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG_UPPER} "${OUTPUT_PREFIX}/${OUTPUTCONFIG}/bin")
    endforeach(OUTPUTCONFIG)
endif()

set(Boost_USE_STATIC_LIBS  OFF)
find_package(Boost COMPONENTS system thread program_options log REQUIRED)

###############################################################################
# Step 2: Project Includes are generally order dependent.  So manually maintain
#         this list
###############################################################################
add_subdirectory(libpfc_net)
add_subdirectory(registry_server)
add_subdirectory(service_pubsub)
add_subdirectory(service_reqrep)
add_subdirectory(service_survey)
add_subdirectory(unit_test)
###############################################################################
# Step 3: Global Doxygen configuration 
#         you might need to tweak this if you have multiple doxy files
###############################################################################
option(BUILD_DOC "Build documentation" ON)

# check if Doxygen is installed
find_package(Doxygen)

set(DOXYGEN_GENERATE_LATEX YES)
set(DOXYGEN_PROJECT_BRIEF "An oepn source reference implementation of the SUSTAIN Networking protocol")
set(DOXYGEN_GENERATE_HTML YES)
set(DOXYGEN_GENERATE_MAN YES)
set(DOXYGEN_GENERATE_PDF YES)
set(DOXYGEN_DISABLE_INDEX NO)
set(DOXYGEN_GENERATE_TREEVIEW YES)
set(DOXYGEN_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/doxygen)
if (DOXYGEN_FOUND)
  doxygen_add_docs(generate_doxygen
    libpfc_net/cpp   libpfc_net/include
    COMMENT generating networking specification)
else (DOXYGEN_FOUND)
  message("Doxygen need to be installed to generate the doxygen documentation")
endif (DOXYGEN_FOUND)

CMAKE_MINIMUM_REQUIRED(VERSION 3.8.0) 
CMAKE_POLICY(VERSION 3.8.0) 

enable_language(CXX)
find_package(GTest)
option(${PROJECT_NAME}_BUILD_TEST        "Select to Build Tests." ${GTest_FOUND})

if(${PROJECT_NAME}_BUILD_TEST)
  
  ###############################################################################
  # Unit Test
  # Step 1:     Assign master and sub_test variables to disable unit test
  #             I suggest this for projects with 100s of test. 
  #             TODO: Create a Macro to control this
  #                   import unit test from Project/cmake/unit.cmake
  ##################################################################V#############
  option(UNITTEST_sustain-pfcnw "Enable libpfc_nw UnitTest " ON)

  ###############################################################################
  # Requirments
  ###############################################################################
  find_package(GTest REQUIRED)
  
  file(GLOB_RECURSE UNITTEST_HEADERS "src/*.h")
  file(GLOB_RECURSE UNITTEST_SOURCES "src/*.cpp")
  
  ##################################################################V#############
  # Step 2:     Assign source files based on control variables
  #             TODO: Create a Macro to control this
  #                   import unit test from Project/cmake/unit.cmake
  ##################################################################V#############
  if(UNITTEST_sustain-pfcnw)
   add_source_files(SUSTAIN_PFCNW_UNITTEST_HEADERS LOCATION ${PROJECT_SOURCE_DIR}/projects/libpfc_net/unit_test/
                 REGEX "test_pfc_nw_*.h"  SOURCE_GROUP  "pfc_nw\\")
   add_source_files(SUSTAIN_PFCNW_UNITTEST_SOURCES LOCATION ${PROJECT_SOURCE_DIR}/projects/libpfc_net/unit_test/
                 REGEX "test_pfc_nw_*.cpp"  SOURCE_GROUP  "pfc_nw\\")

  endif()
  
list(APPEND UNITTEST_LIBRARIES GTest::GTest GTest::Main
  
  )
message(STATUS "
SUSTAIN_PFCNW_UNITTEST_HEADERS=${SUSTAIN_PFCNW_UNITTEST_HEADERS}
SUSTAIN_PFCNW_UNITTEST_SOURCES=${SUSTAIN_PFCNW_UNITTEST_SOURCES}
  ")
  add_executable(unittest
    ${SUSTAIN_PFCNW_UNITTEST_HEADERS}
    ${SUSTAIN_PFCNW_UNITTEST_SOURCES}
  )
 
  ##################################################################V#############
  # Step 3:     Define Disable Macros based on sub test variables
  #             TODO: Create a Macro to control this
  #                   import unit test from Project/cmake/unit.cmake
  ##################################################################V#############
  function( setup_unittest )
  cmake_parse_arguments( "_l" 
                        ""
                        "GROUP"
                        "TESTS"
                        ${ARGN}
                        )

  if(UNITTEST_${_l_GROUP})
  	  foreach ( test_name IN LISTS _l_TESTS )
	  	option(UNITTEST_${_l_GROUP}_${test_name} "Enable test_core_angles" ON)
	  	mark_as_advanced(UNITTEST_${_l_GROUP}_${test_name})
	  	if (  NOT UNITTEST_${_l_GROUP}_${test_name})
		  add_compile_definitions( DISABLE_SUSTAIN_${test_name}_TEST )
	endif()
	  endforeach()
 endif()
  endfunction() 


  set(PFC_NW_TEST_LIST ${SUSTAIN_PFCNW_UNITTEST_SOURCES})
  list(TRANSFORM PFC_NW_TEST_LIST REPLACE ".*\\/test_pfc_nw_(.*).cpp" "\\1")
  setup_unittest( GROUP SUSTAIN_TEST 
                  TESTS 
                    ${PFC_NW_TEST_LIST})
 
  set_target_properties(unittest PROPERTIES
                        DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX}
                        CXX_STANDARD 14
                        FOLDER Other
                        PROJECT_LABEL "Unit Test"
                        VS_DEBUGGER_WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
                      )

  target_include_directories(unittest
      PRIVATE
      ${CMAKE_CURRENT_SOURCE_DIR}/src
      ${GTEST_INCLUDE_DIR}
  )

  if(WIN32)
  target_compile_options(unittest
      PRIVATE "-D_CRT_SECURE_NO_WARNINGS"
  )
  endif()

  if (${PROJECT_NAME}_BUILD_STATIC)
    add_definitions("-D${PROJECT_NAME}_BUILT_AS_STATIC")
  endif()

  target_link_libraries(unittest
    ${UNITTEST_LIBRARIES}
    ${bio_LIBRARIES}
    sustain::pfc_nw
  )
endif(${PROJECT_NAME}_BUILD_TEST)

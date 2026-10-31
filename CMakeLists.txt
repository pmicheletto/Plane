cmake_minimum_required(VERSION 3.14)
project(PitchAndRoll LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

# --- Embed web/simulation.html into the binary as a C++ raw string -----------
# The page is written next to the CSV at run time with the data inlined, so the
# executable is fully self-contained. Re-run automatically when the HTML changes.
set(SIM_HTML ${CMAKE_SOURCE_DIR}/web/simulation.html)
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ${SIM_HTML})
file(READ ${SIM_HTML} SIM_HTML_CONTENT)
file(WRITE ${CMAKE_BINARY_DIR}/generated/WebAssets.h
  "#pragma once\n// Generated from web/simulation.html — do not edit.\n"
  "static const char* const kSimulationHtml = R\"~~~(${SIM_HTML_CONTENT})~~~\";\n")

# --- Core library --------------------------------------------------------------
add_library(attitude
  src/AttitudeEstimator.cpp
  src/AccelLogReader.cpp
  src/EstimationWriter.cpp
  src/SimulationPage.cpp
  src/Browser.cpp
)
target_include_directories(attitude PUBLIC src ${CMAKE_BINARY_DIR}/generated)
target_compile_options(attitude PRIVATE -Wall -Wextra -Wpedantic)

# --- Executable ----------------------------------------------------------------
add_executable(pitch_roll main.cpp)
target_link_libraries(pitch_roll PRIVATE attitude)

# --- Tests ---------------------------------------------------------------------
enable_testing()
add_executable(test_attitude_estimator tests/test_attitude_estimator.cpp)
target_link_libraries(test_attitude_estimator PRIVATE attitude)
set(ATTITUDE_TEST_CASES
  legacy_regression_first_row
  level_sensor_gives_zero_angles
  scale_invariance
  an3461_roll_45deg
  an3461_pitch_signs
  legacy_pitch_ignores_x_axis
  upside_down_sensor_reads_180_roll
  rad_to_deg
)
foreach(case IN LISTS ATTITUDE_TEST_CASES)
  add_test(NAME attitude_estimator.${case} COMMAND test_attitude_estimator ${case})
endforeach()

add_executable(test_simulation_page tests/test_simulation_page.cpp)
target_link_libraries(test_simulation_page PRIVATE attitude)
add_test(NAME simulation_page COMMAND test_simulation_page)

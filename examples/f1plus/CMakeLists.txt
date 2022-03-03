
        ##########    Copyright (C) 2015 Vincenzo Pacella
        ##      ##    Distributed under MIT license, see file LICENSE
        ##      ##    or <http://opensource.org/licenses/MIT>
        ##      ##
##########      ############################################################# shaduzlabs.com #######

# ------------------------------------------------------------------------------------------------ #
#  F1 Plus - Standalone Traktor Kontrol F1                                                         #
# ------------------------------------------------------------------------------------------------ #

project(f1plus)

set(VERSION_RESOURCES "")

SET(
  Examples_f1plus_FILES
    main.cpp
    F1Plus.h
    F1Plus.cpp
    ${VERSION_RESOURCES}
)

add_executable(
  ${PROJECT_NAME}
  ${Examples_f1plus_FILES}
)

set_target_properties(
  ${PROJECT_NAME}
  PROPERTIES
    OUTPUT_NAME           ${PROJECT_NAME}
    OUTPUT_NAME_DEBUG     ${PROJECT_NAME}${DEBUG_SUFFIX}
)

if(RTMIDI_LOCAL_BUILD)
  target_link_libraries( ${PROJECT_NAME} PUBLIC rtmidi)
else()
  target_link_libraries( ${PROJECT_NAME} PUBLIC ${RTMIDI_LIBRARIES})
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE cabl-static)

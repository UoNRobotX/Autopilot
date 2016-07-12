# Find our globally shared libraries:
FIND_PACKAGE(CATCH REQUIRED)
FIND_PACKAGE(YAML-CPP REQUIRED)
FIND_PACKAGE(Eigen3 REQUIRED)
FIND_PACKAGE(Boost COMPONENTS system REQUIRED)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/libopengnc/include)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/gnc/include)

# Set include directories and libraries:
INCLUDE_DIRECTORIES(SYSTEM ${CATCH_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(SYSTEM ${YAML-CPP_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(SYSTEM ${Eigen3_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(SYSTEM ${Boost_INCLUDE_DIRS})

SET(NUCLEAR_ADDITIONAL_SHARED_LIBRARIES
    ${YAML-CPP_LIBRARIES}
    ${Boost_LIBRARIES}
    -ldl
    -lbacktrace
)

find_package(PkgConfig)

PKG_CHECK_MODULES(PC_GR_GUPPI gnuradio-guppi)

FIND_PATH(
    GR_GUPPI_INCLUDE_DIRS
    NAMES gnuradio/guppi/api.h
    HINTS $ENV{GUPPI_DIR}/include
        ${PC_GUPPI_INCLUDEDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/include
          /usr/local/include
          /usr/include
)

FIND_LIBRARY(
    GR_GUPPI_LIBRARIES
    NAMES gnuradio-guppi
    HINTS $ENV{GUPPI_DIR}/lib
        ${PC_GUPPI_LIBDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/lib
          ${CMAKE_INSTALL_PREFIX}/lib64
          /usr/local/lib
          /usr/local/lib64
          /usr/lib
          /usr/lib64
          )

include("${CMAKE_CURRENT_LIST_DIR}/gnuradio-guppiTarget.cmake")

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(GR_GUPPI DEFAULT_MSG GR_GUPPI_LIBRARIES GR_GUPPI_INCLUDE_DIRS)
MARK_AS_ADVANCED(GR_GUPPI_LIBRARIES GR_GUPPI_INCLUDE_DIRS)

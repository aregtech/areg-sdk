#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "areg::areg" for configuration "Debug"
set_property(TARGET areg::areg APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(areg::areg PROPERTIES
  IMPORTED_IMPLIB_DEBUG "${_IMPORT_PREFIX}/lib/areg.lib"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/bin/areg.dll"
  )

list(APPEND _cmake_import_check_targets areg::areg )
list(APPEND _cmake_import_check_files_for_areg::areg "${_IMPORT_PREFIX}/lib/areg.lib" "${_IMPORT_PREFIX}/bin/areg.dll" )

# Import target "areg::aregextend" for configuration "Debug"
set_property(TARGET areg::aregextend APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(areg::aregextend PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "CXX;RC"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib/aregextend.lib"
  )

list(APPEND _cmake_import_check_targets areg::aregextend )
list(APPEND _cmake_import_check_files_for_areg::aregextend "${_IMPORT_PREFIX}/lib/aregextend.lib" )

# Import target "areg::areglogger" for configuration "Debug"
set_property(TARGET areg::areglogger APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(areg::areglogger PROPERTIES
  IMPORTED_IMPLIB_DEBUG "${_IMPORT_PREFIX}/lib/areglogger.lib"
  IMPORTED_LINK_DEPENDENT_LIBRARIES_DEBUG "areg::areg"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/bin/areglogger.dll"
  )

list(APPEND _cmake_import_check_targets areg::areglogger )
list(APPEND _cmake_import_check_files_for_areg::areglogger "${_IMPORT_PREFIX}/lib/areglogger.lib" "${_IMPORT_PREFIX}/bin/areglogger.dll" )

# Import target "areg::aregsqlite3" for configuration "Debug"
set_property(TARGET areg::aregsqlite3 APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(areg::aregsqlite3 PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "C"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib/aregsqlite3.lib"
  )

list(APPEND _cmake_import_check_targets areg::aregsqlite3 )
list(APPEND _cmake_import_check_files_for_areg::aregsqlite3 "${_IMPORT_PREFIX}/lib/aregsqlite3.lib" )

# Import target "areg::logcollector" for configuration "Debug"
set_property(TARGET areg::logcollector APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(areg::logcollector PROPERTIES
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/tools/areg/logcollector.exe"
  )

list(APPEND _cmake_import_check_targets areg::logcollector )
list(APPEND _cmake_import_check_files_for_areg::logcollector "${_IMPORT_PREFIX}/tools/areg/logcollector.exe" )

# Import target "areg::logobserver" for configuration "Debug"
set_property(TARGET areg::logobserver APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(areg::logobserver PROPERTIES
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/tools/areg/logobserver.exe"
  )

list(APPEND _cmake_import_check_targets areg::logobserver )
list(APPEND _cmake_import_check_files_for_areg::logobserver "${_IMPORT_PREFIX}/tools/areg/logobserver.exe" )

# Import target "areg::mtrouter" for configuration "Debug"
set_property(TARGET areg::mtrouter APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(areg::mtrouter PROPERTIES
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/tools/areg/mtrouter.exe"
  )

list(APPEND _cmake_import_check_targets areg::mtrouter )
list(APPEND _cmake_import_check_files_for_areg::mtrouter "${_IMPORT_PREFIX}/tools/areg/mtrouter.exe" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)

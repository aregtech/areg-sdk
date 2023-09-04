# Project names
macro_declare_project(24_generated          "generated")
macro_declare_project(24_publisher          "publisher")
macro_declare_project(24_subscribermulti    "subscribermulti")

addStaticLib(${project_generated}           "${generated_src}")
addExecutableEx(${project_publisher}   	    "${publisher_src}"       "${project_generated}")
addExecutableEx(${project_subscribermulti}  "${subscribermulti_src}" "${project_generated}")

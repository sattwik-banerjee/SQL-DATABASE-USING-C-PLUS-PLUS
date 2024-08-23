# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/Users/sattwikbanerjee/CS008ASSIGNMENTS/99_99_final_project-sattwik171/build/_deps/googletest-src"
  "/Users/sattwikbanerjee/CS008ASSIGNMENTS/99_99_final_project-sattwik171/build/_deps/googletest-build"
  "/Users/sattwikbanerjee/CS008ASSIGNMENTS/99_99_final_project-sattwik171/build/_deps/googletest-subbuild/googletest-populate-prefix"
  "/Users/sattwikbanerjee/CS008ASSIGNMENTS/99_99_final_project-sattwik171/build/_deps/googletest-subbuild/googletest-populate-prefix/tmp"
  "/Users/sattwikbanerjee/CS008ASSIGNMENTS/99_99_final_project-sattwik171/build/_deps/googletest-subbuild/googletest-populate-prefix/src/googletest-populate-stamp"
  "/Users/sattwikbanerjee/CS008ASSIGNMENTS/99_99_final_project-sattwik171/build/_deps/googletest-subbuild/googletest-populate-prefix/src"
  "/Users/sattwikbanerjee/CS008ASSIGNMENTS/99_99_final_project-sattwik171/build/_deps/googletest-subbuild/googletest-populate-prefix/src/googletest-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/Users/sattwikbanerjee/CS008ASSIGNMENTS/99_99_final_project-sattwik171/build/_deps/googletest-subbuild/googletest-populate-prefix/src/googletest-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/Users/sattwikbanerjee/CS008ASSIGNMENTS/99_99_final_project-sattwik171/build/_deps/googletest-subbuild/googletest-populate-prefix/src/googletest-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()

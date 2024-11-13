# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\QMusicPlayer_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\QMusicPlayer_autogen.dir\\ParseCache.txt"
  "QMusicPlayer_autogen"
  )
endif()

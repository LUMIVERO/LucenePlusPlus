####################################
# get dependencies
####################################

find_package(Boost COMPONENTS
  date_time
  filesystem
  iostreams
  regex
  system
  thread
  serialization
  REQUIRED
)

set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_LIBS ${LUCENE_USE_STATIC_BOOST_LIBS})

set(lucene_boost_libs
  ${Boost_LIBRARIES}
  ${Boost_FILESYSTEM_LIBRARIES}
  ${Boost_IOSTREAMS_LIBRARIES}
  ${Boost_REGEX_LIBRARIES}
  ${Boost_SYSTEM_LIBRARIES}
  ${Boost_THREAD_LIBRARIES}
  ${Boost_SERIALIZATION_LIBRARIES}
)

find_package(ZLIB REQUIRED)
find_package(Threads REQUIRED)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(locker_SOURCES cfg.c db.c gpg.c inih/ini.c 2p.c)

add_executable(locker ${locker_SOURCES})

target_link_libraries(locker readline gpgme sqlite3)

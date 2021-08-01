set(INI_PARSER_SRCS ini_parser.cc)

add_executable(ini_parser ${INI_PARSER_SRCS})

target_link_libraries(ini_parser -lINIReader)

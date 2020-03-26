project(bpintegrity)

set(EOSIO_WASM_OLD_BEHAVIOR "Off")
find_package(eosio.cdt)

add_contract( bpintegrity bpintegrity bpintegrity.cpp )
target_include_directories( bpintegrity PUBLIC ${CMAKE_SOURCE_DIR}/../include )
target_ricardian_directory( bpintegrity ${CMAKE_SOURCE_DIR}/../ricardian )


#target_compile_options( bpintegrity PUBLIC -Os -finline-hint-functions -fno-elide-constructors -O2)
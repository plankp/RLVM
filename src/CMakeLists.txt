find_package(FLEX)
find_package(BISON)

BISON_TARGET(RlvmParser rasm.y ${CMAKE_CURRENT_BINARY_DIR}/rasm.tab.c)
FLEX_TARGET(RlvmScanner rasm.l ${CMAKE_CURRENT_BINARY_DIR}/rasm.lex.c)
ADD_FLEX_BISON_DEPENDENCY(RlvmScanner RlvmParser)

file(GLOB SOURCES "*.c")
add_library(rlvmlib
    ${SOURCES}
    ${BISON_RlvmParser_OUTPUTS}
    ${FLEX_RlvmScanner_OUTPUTS})

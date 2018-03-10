# -*- mode:cmake -*-
if(NOT UNIX)
  message(WARNING "Unsupported operating system")
endif()

set(ANTLR4_GRAMMAR_GENERATED_SRC
   ${PROJECT_SOURCE_DIR}/demo/generated/TLexer.cpp 
   ${PROJECT_SOURCE_DIR}/demo/generated/TParser.cpp
   ${PROJECT_SOURCE_DIR}/demo/generated/TParserBaseListener.cpp
   ${PROJECT_SOURCE_DIR}/demo/generated/TParserBaseVisitor.cpp
   ${PROJECT_SOURCE_DIR}/demo/generated/TParserListener.cpp
   ${PROJECT_SOURCE_DIR}/demo/generated/TParserVisitor.cpp
 )

foreach( src_file ${ANTLR4_GRAMMAR_GENERATED_SRC} )
      set_source_files_properties(
          ${src_file}
          PROPERTIES
          GENERATED TRUE
          )
endforeach( src_file ${ANTLR4_GRAMMAR_GENERATED_SRC} )

add_custom_target(GenerateParser DEPENDS ${ANTLR4_GRAMMAR_GENERATED_SRC})
add_custom_command(OUTPUT ${ANTLR4_GRAMMAR_GENERATED_SRC}
   COMMAND 
   ${CMAKE_COMMAND} -E make_directory ${PROJECT_SOURCE_DIR}/demo/generated/
   COMMAND
   "${Java_JAVA_EXECUTABLE}" -jar ${ANTLR_JAR_LOCATION} -Werror -Dlanguage=Cpp -listener -visitor -o ${PROJECT_SOURCE_DIR}/demo/generated/ -package antlrcpptest ${PROJECT_SOURCE_DIR}/grammar/TLexer.g4 ${PROJECT_SOURCE_DIR}/grammar/TParser.g4
   WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
   DEPENDS ${PROJECT_SOURCE_DIR}/grammar/TLexer.g4 ${PROJECT_SOURCE_DIR}/grammar/TParser.g4
)

include_directories(
  ${PROJECT_SOURCE_DIR}/runtime/src
  ${PROJECT_SOURCE_DIR}/runtime/src/misc
  ${PROJECT_SOURCE_DIR}/runtime/src/atn
  ${PROJECT_SOURCE_DIR}/runtime/src/dfa
  ${PROJECT_SOURCE_DIR}/runtime/src/tree
  ${PROJECT_SOURCE_DIR}/runtime/src/support
  ${PROJECT_SOURCE_DIR}/demo/generated
)

set(ANTLR4_GRAMMAR_SRC 
	${PROJECT_SOURCE_DIR}/demo/main.cpp
	${ANTLR4_GRAMMAR_GENERATED_SRC}
)

if (NOT CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
	set(ADDITIONAL_FLAGS "-Wno-overloaded-virtual")
else()
	set(ADDITIONAL_FLAGS "-MP /wd4251")
endif ()

foreach( src_file ${ANTLR4_GRAMMAR_SRC} )
	set_source_files_properties(
		${src_file}
		PROPERTIES
		COMPILE_FLAGS "${COMPILE_FLAGS} ${ADDITIONAL_FLAGS}"
	)
endforeach( src_file ${ANTLR4_GRAMMAR_SRC} )

add_executable(ANTLR4GrammarTest
	${ANTLR4_GRAMMAR_SRC}
)
#add_precompiled_header(ANTLR4GrammarTest ${PROJECT_SOURCE_DIR}/runtime/src/antlrcpp-Prefix.h)

# Make a Visual Studio folder in this project to house the grammars.
message("${PROJECT_SOURCE_DIR}/grammar/TLexer.g4")
source_group("grammar" FILES ${PROJECT_SOURCE_DIR}/demo/main.cpp)
#source_group("grammar" FILES "${PROJECT_SOURCE_DIR}/grammar/TLexer.g4")
#source_group("grammar" FILES "${PROJECT_SOURCE_DIR}/grammar/TParser.g4")
#file(GLOB_RECURSE LEX_PARSER_RULE_FILES 
#    LIST_DIRECTORIES false
#    "${PROJECT_SOURCE_DIR}/grammar/*.g4*"
#)
#foreach(SRC_FILE IN ITEMS ${LEX_PARSER_RULE_FILES})
#    get_filename_component(SOURCE_PATH "${SRC_FILE}" PATH)
#    file(RELATIVE_PATH SOURCE_RELATIVE_PATH "${PROJECT_SOURCE_DIR}" "${SOURCE_PATH}")
#    string(REPLACE "/" "\\" GROUP_PATH "${SOURCE_RELATIVE_PATH}")
#    source_group("${GROUP_PATH}" FILES "${SRC_FILE}")
#endforeach()

# Add an MSVC compile option.
if (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
	target_compile_options(ANTLR4GrammarTest PRIVATE "/MT$<$<CONFIG:Debug>:d>")
endif()

add_dependencies(ANTLR4GrammarTest GenerateParser)

target_link_libraries(ANTLR4GrammarTest antlr4_static)

install(TARGETS ANTLR4GrammarTest 
	DESTINATION "share" 
	COMPONENT dev 
)


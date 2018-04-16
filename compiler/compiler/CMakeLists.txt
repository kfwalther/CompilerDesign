# -*- mode:cmake -*-
if(NOT UNIX)
	message(WARNING "Unsupported operating system")
endif()

# Set the C++ standard to C++17.
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(ANTLR4_GRAMMAR_GENERATED_SRCS
   ${PROJECT_SOURCE_DIR}/compiler/generated/TLexer.cpp 
   ${PROJECT_SOURCE_DIR}/compiler/generated/TParser.cpp
   ${PROJECT_SOURCE_DIR}/compiler/generated/TParserBaseListener.cpp
   ${PROJECT_SOURCE_DIR}/compiler/generated/TParserBaseVisitor.cpp
   ${PROJECT_SOURCE_DIR}/compiler/generated/TParserListener.cpp
   ${PROJECT_SOURCE_DIR}/compiler/generated/TParserVisitor.cpp
 )

set(ANTLR4_GRAMMAR_GENERATED_HDRS
	${PROJECT_SOURCE_DIR}/compiler/generated/TLexer.h 
	${PROJECT_SOURCE_DIR}/compiler/generated/TParser.h
	${PROJECT_SOURCE_DIR}/compiler/generated/TParserBaseListener.h
	${PROJECT_SOURCE_DIR}/compiler/generated/TParserBaseVisitor.h
	${PROJECT_SOURCE_DIR}/compiler/generated/TParserListener.h
	${PROJECT_SOURCE_DIR}/compiler/generated/TParserVisitor.h
)
set(ANTLR4_GRAMMAR_GENERATED_CODE ${ANTLR4_GRAMMAR_GENERATED_HDRS} ${ANTLR4_GRAMMAR_GENERATED_SRCS})

foreach(src_file ${ANTLR4_GRAMMAR_GENERATED_SRCS})
	set_source_files_properties(
		${src_file}
		PROPERTIES
		GENERATED TRUE
	)
endforeach(src_file ${ANTLR4_GRAMMAR_GENERATED_SRCS})

add_custom_target(GenerateParser DEPENDS ${ANTLR4_GRAMMAR_GENERATED_SRCS})
add_custom_command(OUTPUT ${ANTLR4_GRAMMAR_GENERATED_SRCS}
	COMMAND 
	${CMAKE_COMMAND} -E make_directory ${PROJECT_SOURCE_DIR}/compiler/generated/
	COMMAND
	"${Java_JAVA_EXECUTABLE}" -jar ${ANTLR_JAR_LOCATION} -Werror -Dlanguage=Cpp -listener -visitor -o ${PROJECT_SOURCE_DIR}/compiler/generated/ -package AntlrGrammarGenerated ${PROJECT_SOURCE_DIR}/grammar/TLexer.g4 ${PROJECT_SOURCE_DIR}/grammar/TParser.g4
	WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
	DEPENDS ${PROJECT_SOURCE_DIR}/grammar/TLexer.g4 ${PROJECT_SOURCE_DIR}/grammar/TParser.g4
)

include_directories(
	${PROJECT_SOURCE_DIR}/antlr4/runtime/src
	${PROJECT_SOURCE_DIR}/antlr4/runtime/src/misc
	${PROJECT_SOURCE_DIR}/antlr4/runtime/src/atn
	${PROJECT_SOURCE_DIR}/antlr4/runtime/src/dfa
	${PROJECT_SOURCE_DIR}/antlr4/runtime/src/tree
	${PROJECT_SOURCE_DIR}/antlr4/runtime/src/support
	${PROJECT_SOURCE_DIR}/compiler/generated
	${PROJECT_SOURCE_DIR}/compiler
)

set(ANTLR4_GRAMMAR_LOCAL_HDRS 
	${PROJECT_SOURCE_DIR}/compiler/AstNode.h
	${PROJECT_SOURCE_DIR}/compiler/AstVisitor.h
	${PROJECT_SOURCE_DIR}/compiler/AstVisitorImpl.h
	${PROJECT_SOURCE_DIR}/compiler/Compiler.h
	${PROJECT_SOURCE_DIR}/compiler/ErrorHandler.h
	${PROJECT_SOURCE_DIR}/compiler/LanguageDefinitions.h
	${PROJECT_SOURCE_DIR}/compiler/ParseTreeListenerImpl.h
	${PROJECT_SOURCE_DIR}/compiler/ParseTreeVisitorImpl.h
	${PROJECT_SOURCE_DIR}/compiler/Scope.h
	${PROJECT_SOURCE_DIR}/compiler/SymbolTable.h
	${PROJECT_SOURCE_DIR}/compiler/SymbolTableManager.h
	${PROJECT_SOURCE_DIR}/compiler/SemanticAnalyzer.h
)
set(ANTLR4_GRAMMAR_LOCAL_SRCS 
	${PROJECT_SOURCE_DIR}/compiler/main.cpp
	${PROJECT_SOURCE_DIR}/compiler/AstNode.cpp
	${PROJECT_SOURCE_DIR}/compiler/AstVisitorImpl.cpp
	${PROJECT_SOURCE_DIR}/compiler/Compiler.cpp
	${PROJECT_SOURCE_DIR}/compiler/ErrorHandler.cpp
	${PROJECT_SOURCE_DIR}/compiler/ParseTreeListenerImpl.cpp
	${PROJECT_SOURCE_DIR}/compiler/ParseTreeVisitorImpl.cpp
	${PROJECT_SOURCE_DIR}/compiler/Scope.cpp
	${PROJECT_SOURCE_DIR}/compiler/SymbolTable.cpp
	${PROJECT_SOURCE_DIR}/compiler/SymbolTableManager.cpp
	${PROJECT_SOURCE_DIR}/compiler/SemanticAnalyzer.cpp
)
set(ANTLR4_GRAMMAR_LOCAL_CODE ${ANTLR4_GRAMMAR_LOCAL_HDRS} ${ANTLR4_GRAMMAR_LOCAL_SRCS})
set(ANTLR4_GRAMMAR_RULE_FILES ${PROJECT_SOURCE_DIR}/grammar/TLexer.g4 ${PROJECT_SOURCE_DIR}/grammar/TParser.g4)

set(ANTLR4_GRAMMAR_ALL_SRCS 
	${ANTLR4_GRAMMAR_LOCAL_SRCS}
	${ANTLR4_GRAMMAR_GENERATED_SRCS}
)

if(NOT CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
	set(ADDITIONAL_FLAGS "-Wno-overloaded-virtual")
else()
	set(ADDITIONAL_FLAGS "-MP /wd4251")
endif()

foreach( src_file ${ANTLR4_GRAMMAR_ALL_SRCS} )
	set_source_files_properties(
		${src_file}
		PROPERTIES
		COMPILE_FLAGS "${COMPILE_FLAGS} ${ADDITIONAL_FLAGS}"
	)
endforeach( src_file ${ANTLR4_GRAMMAR_ALL_SRCS} )

add_executable(ANTLR4GrammarTest
	${ANTLR4_GRAMMAR_GENERATED_CODE}
	${ANTLR4_GRAMMAR_LOCAL_CODE}
	${ANTLR4_GRAMMAR_RULE_FILES}
)
#add_precompiled_header(ANTLR4GrammarTest ${PROJECT_SOURCE_DIR}/antlr4/runtime/src/antlrcpp-Prefix.h)

# Make a Visual Studio folder in this project to house the grammars.
source_group("grammar" FILES ${ANTLR4_GRAMMAR_RULE_FILES})
source_group("compiler" FILES ${ANTLR4_GRAMMAR_LOCAL_CODE})
source_group("compiler\\generated" FILES ${ANTLR4_GRAMMAR_GENERATED_CODE})
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
	target_compile_options(ANTLR4GrammarTest PRIVATE "/MD$<$<CONFIG:Debug>:d>")
endif()

add_dependencies(ANTLR4GrammarTest GenerateParser)

# NOTE: Cant change this to shared, program wont run. Can't use this with Visitors either...
target_link_libraries(ANTLR4GrammarTest antlr4_shared)

# Copy the dependent libraries to the executable directory after the build completes.
add_custom_command(TARGET ANTLR4GrammarTest POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_BINARY_DIR}/lib/$<CONFIGURATION>"
        $<TARGET_FILE_DIR:ANTLR4GrammarTest>)

install(TARGETS ANTLR4GrammarTest 
	DESTINATION "share" 
	COMPONENT dev 
)


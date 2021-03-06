cmake_minimum_required(VERSION 2.8)

#      The Kraken project (kisio Digital corp.)
#      https://www.navitia.io/
#                        ___
#                     .-'   `'.
#                    /         \
#                    |         ;
#                    |         |           ___.--,
#           _.._     |0) ~ (0) |    _.---'`__.-( (_.
#    __.--'`_.. '.__.\    '--. \_.-' ,.--'`     `""`
#   ( ,.--'`   ',__ /./;   ;, '.__.'`    __
#   _`) )  .---.__.' / |   |\   \__..--""  """--.,_
#  `---' .'.''-._.-'`_./  /\ '.  \ _.-~~~````~~~-._`-.__.'
#        | |  .' _.-' |  |  \  \  '.               `~---`
#         \ \/ .'     \  \   '. '-._)
#          \/ /        \  \    `=.__`~-.
#          / /\         `) )    / / `"".`\
#    , _.-'.'\ \        / /    ( (     / /
#     `--~`   ) )    .-'.'      '.'.  | (
#            (/`    ( (`          ) )  '-;
#             `      '-;         (-'
project(Kraken)

# List of directories to search for CMake modules
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules")

# project version,
# get git revision as the version number
include(GetGitRevisionDescription)
git_describe(GIT_REVISION)

# Compilation flags (Debug and Release)
include(CompilationFlags)

# Finds and loads settings
set(LOG4CPLUS_FIND_REQUIRED true)
find_package(log4cplus)
find_package(GoogleTcmalloc)
find_package(Protoc)

# Load in the needed CMake modules
include(UseBoost)
include(UsePqxx)
include(ThirdParty)

# Tests environment
include(TestsEnv)

# Add current compilation dir to include path to handle config.h
include_directories(SYSTEM "${CMAKE_CURRENT_BINARY_DIR}")
include_directories("${CMAKE_SOURCE_DIR}")

add_subdirectory(utils)
add_subdirectory(type)
add_subdirectory(ptreferential)
add_subdirectory(autocomplete)
add_subdirectory(ed)
add_subdirectory(fare)
add_subdirectory(proximity_list)
add_subdirectory(kraken)
add_subdirectory(vptranslator)
add_subdirectory(lz4_filter)
add_subdirectory(routing)
add_subdirectory(georef)
add_subdirectory(time_tables)
add_subdirectory(jormungandr)
add_subdirectory(tyr)
add_subdirectory(sql)
add_subdirectory(disruption)
add_subdirectory(calendar)
add_subdirectory(tests)
add_subdirectory(scripts)
add_subdirectory(cities)
add_library(linenoise linenoise/linenoise.c)
add_subdirectory(tools)

include(CheckIncludeFiles)
include(CheckIncludeFileCXX)
set(CMAKE_REQUIRED_INCLUDES "/usr/include")
CHECK_INCLUDE_FILES(iconv.h HAVE_ICONV_H)
CHECK_INCLUDE_FILE_CXX("log4cplus/loggingmacros.h" HAVE_LOGGINGMACROS_H)

# Usefull to share cmake global var in codes
include(LoadConfig)

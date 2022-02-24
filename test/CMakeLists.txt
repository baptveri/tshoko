PROJECT (FileSaverTest)

LINK_DIRECTORIES(${LIB_DIR}/sndfile/macos/lib)
INCLUDE_DIRECTORIES(${LIB_DIR}/sndfile/macos/include)

ADD_EXECUTABLE(FileSaverTest main.c)
TARGET_LINK_LIBRARIES(FileSaverTest PRIVATE sndfile file_saver)

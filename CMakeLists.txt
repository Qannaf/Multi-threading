cmake_minimum_required(VERSION 2.8.12)

project(test)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

#+------------------------------
# Add the headers(/*.h)
set(hdr "include/")
include_directories(${hdr})

#+------------------------------
# Add the sources (/*.cpp)
file(GLOB USER_CU "src/*.cpp"  )




# pthread
set(PTHREAD_DIR "C:/Program Files/pthread/Pre-built.2" CACHE PATH "Location of libraries" FORCE)
set(PTHREAD_LIBS "pthreadVC2.lib")
set(PTHREAD_LIBS_DYN "pthreadVC2.dll")

include_directories(${PTHREAD_DIR}/include)
link_directories(${PTHREAD_DIR}/lib/x64)
link_directories(${PTHREAD_DIR}/dll/x64)


add_executable(${PROJECT_NAME} ${USER_CU})

target_link_libraries (${PROJECT_NAME} ${PTHREAD_LIBS} ${PTHREAD_LIBS_DYN})
						



set( PCL_LIB_DLL_DEBUG "${PTHREAD_DIR}/dll/x64/pthreadVC2.dll"  )
	message("Files .dll are ${PCL_LIB_DLL_DEBUG}" )	

	file(COPY ${PCL_LIB_DLL_DEBUG}
		 DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Debug)
	message("Done coping .dll files")

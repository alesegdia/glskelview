set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")

# SET SOURCES AND STUFF
file(GLOB_RECURSE CPPs *.cpp)
file(GLOB_RECURSE HPPs *.h)

# include_directories(core)

#set(GLFW3_DEPS -lrt -lX11 -lXxf86vm -lXrandr -lpthread -lXi)
#set(GLFW3_DEPS -lglew32 -lgdi32)
#set(SFML_STATIC_LIBRARIES TRUE)

# FIND LIBRARIES
find_package(OpenGL REQUIRED)
#find_package(GLFW3 REQUIRED)
find_package(X11 REQUIRED)
find_package(GLEW REQUIRED)
find_package(FLTK REQUIRED)

# NAME THE TARGETS
add_executable(practicavg ${CPPs} ${HPPs})

# OPENGL
if(OPENGL_FOUND)
	include_directories(${OPENGL_INCLUDE_DIR})
	target_link_libraries(practicavg ${OPENGL_LIBRARIES})
endif(OPENGL_FOUND)

if(GLEW_FOUND)
	include_directories(${GLEW_INCLUDE_PATH})
	target_link_libraries(practicavg ${GLEW_LIBRARY})
endif(GLEW_FOUND)

if(FLTK_FOUND)
	include_directories(${FLTK_INCLUDE_DIR})
	target_link_libraries(practicavg ${FLTK_LIBRARIES})
	target_link_libraries(practicavg -ldl -lrt -lX11 -lXxf86vm -lXrandr -lpthread -lXi)
endif(FLTK_FOUND)

# GLFW
#if(GLFW3_FOUND)
#	include_directories(${GLFW3_INCLUDE_PATH})
#	target_link_libraries(practicavg ${GLFW3_LIBRARY})
#	target_link_libraries(practicavg ${GLFW3_DEPS})
#	message(${GLFW3_INCLUDE_PATH})
#	message(${GLFW3_LIBRARY})
#else(GLFW3_FOUND)
#	message("GLFW NOT FOUND!!")
#endif(GLFW3_FOUND)

# OPENGL
#if(X11_FOUND)
#	include_directories(${XLIB_INCLUDE_DIR})
#	target_link_libraries(practicavg ${X11_LIBRARIES})
#endif(X11_FOUND)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_HOME_DIRECTORY})


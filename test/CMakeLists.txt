include(mentor_test)

message(STATUS "Configuring tests...")

file(GLOB TEST_SOURCES *.cpp)

foreach(src ${TEST_SOURCES})
    get_filename_component(name ${src} NAME_WE)
    mentor_add_test(
        ${name}
        SOURCES ${src}
    )
endforeach()

message(STATUS "[mentor] Tests configuration succeed!")

include(mentor_test)

file(GLOB TEST_SOURCES *.cpp)

message(STATUS "[mentor] Configuring Codec tests...")

foreach(src ${TEST_SOURCES})
    get_filename_component(name ${src} NAME_WE)
    mentor_add_test(
        ${name}
        SOURCES ${src}
    )
endforeach()

message(STATUS "[mentor] Codec tests configuration succeed!")

add_executable(mentor_CUI)
target_compile_features(mentor_CUI PRIVATE cxx_std_20)
target_link_libraries(mentor_CUI PRIVATE mentor_lib)

target_sources(mentor_CUI
  PRIVATE
    Util.cpp
    main.cpp
)
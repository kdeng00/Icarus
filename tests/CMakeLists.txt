# Create CMakeFiles.txt for icarus tests



# Using Catch2
find_package(Catch2 CONFIG REQUIRED)
target_link_libraries(main PRIVATE Catch2::Catch2)


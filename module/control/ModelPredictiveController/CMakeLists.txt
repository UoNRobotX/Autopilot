FIND_PACKAGE(BLAS REQUIRED)
FIND_PACKAGE(LAPACK REQUIRED)

# Build our NUClear module
NUCLEAR_MODULE(LIBRARIES ${BLAS_LIBRARIES} ${LAPACK_LIBRARIES})
add_definitions(-DINT_LONG_INT -DOS_LIN)

cmake_minimum_required(VERSION 2.8)

include(CheckFunctionExists)
CHECK_FUNCTION_EXISTS(mremap HAVE_MREMAP)
CHECK_FUNCTION_EXISTS(fallocate HAVE_FALLOCATE)
CHECK_FUNCTION_EXISTS(posix_fallocate HAVE_POSIX_FALLOCATE)

include(CheckIncludeFile)
CHECK_INCLUDE_FILE("endian.h" HAVE_ENDIAN_H)
CHECK_INCLUDE_FILE("machine/endian.h" HAVE_MACHINE_ENDIAN_H)
CHECK_INCLUDE_FILE("sys/endian.h" HAVE_SYS_ENDIAN_H)
CHECK_INCLUDE_FILE("libkern/OSByteOrder.h" HAVE_KERN_OSBYTEORDER_H)

include(CheckSymbolExists)
CHECK_SYMBOL_EXISTS(be64toh "sys/endian.h" HAVE_BE64TOH)
CHECK_SYMBOL_EXISTS(betoh64 "sys/endian.h" HAVE_BETOH64)
CHECK_SYMBOL_EXISTS(F_PREALLOCATE "fcntl.h" HAVE_F_PREALLOCATE)

CONFIGURE_FILE(QtChromosomeViz_v2/bartekm_code/include/config.h.in ../QtChromosomeViz_v2/bartekm_code/include/config.h)

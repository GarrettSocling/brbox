# this one is important
SET(CMAKE_SYSTEM_NAME Linux)
#this one not so much
#SET(CMAKE_SYSTEM_VERSION 1)

# specify the cross compiler
SET(CMAKE_C_COMPILER   /opt/brbox-raspi1-arm/usr/bin/arm-buildroot-linux-gnueabi-gcc)
SET(CMAKE_CXX_COMPILER /opt/brbox-raspi1-arm/usr/bin/arm-buildroot-linux-gnueabi-g++)

# where is the target environment 
SET(CMAKE_FIND_ROOT_PATH  /opt/brbox-raspi1-arm /opt/brbox-raspi1-arm/usr/arm-buildroot-linux-gnueabi/sysroot)

# search for programs in the build host directories
SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
# for libraries and headers in the target directories
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

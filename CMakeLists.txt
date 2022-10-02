cmake_minimum_required (VERSION 3.7)
project (pros)

set(CMAKE_CXX_STANDARD 14)

set (CMAKE_CXX_COMPILER "${CMAKE_CURRENT_LIST_DIR}/Toolchain/i686/bin/i686-elf-g++")
set (CMAKE_CXX_FLAGS "-m32 -O2 -ggdb -nostdlib -mno-red-zone -Wall -Wextra -Wno-unused-function -Wno-unused-variable -Wno-unused-parameter -fstack-protector-all -fno-threadsafe-statics -fno-builtin -fno-rtti")
set (CMAKE_EXE_LINKER_FLAGS "-m32 -T${CMAKE_CURRENT_LIST_DIR}/Kernel/linker.ld")
set (CMAKE_ASM_COMPILER "${CMAKE_CURRENT_LIST_DIR}/Toolchain/i686/bin/i686-elf-as")
set (CMAKE_ASM_FLAGS "--32")

enable_language (ASM)

file (GLOB_RECURSE kernel_CPP_SOURCES "Kernel/src/*.cpp")
file (GLOB_RECURSE kernel_ASM_SOURCES "Kernel/src/*.S")

add_executable (pros.bin
        ${kernel_CPP_SOURCES}
        ${kernel_ASM_SOURCES}
        )

target_include_directories(pros.bin
        PRIVATE
        Kernel/include
        )

add_custom_command(TARGET pros.bin POST_BUILD
        COMMAND mkdir -p iso/boot/grub
        COMMAND cp pros.bin iso/boot
        MAIN_DEPENDENCY pros.bin)

configure_file(boot/grub/grub.cfg.in iso/boot/grub/grub.cfg @ONLY)
add_custom_command(TARGET pros.bin POST_BUILD
        COMMAND grub-mkrescue --output=pros.iso iso
        COMMAND rm -rf iso
        MAIN_DEPENDENCY pros.bin)
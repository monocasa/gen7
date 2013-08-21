SET( CMAKE_CXX_FLAGS "-m64 -mcmodel=kernel -std=c++0x -D__64BIT__ -I${PROJECT_SOURCE_DIR} -I${CMAKE_CURRENT_LIST_DIR}/stdlib/include -ffreestanding -nostdlib -mno-red-zone -fno-rtti -fno-exceptions -nostdinc -fno-stack-protector" )
SET( CMAKE_EXE_LINKER_FLAGS "-nodefaultlibs -T${CMAKE_CURRENT_LIST_DIR}/linker.ld" )
SET( CMAKE_ASM_NASM_OBJECT_FORMAT "elf64" )

ENABLE_LANGUAGE( ASM_NASM )

ADD_LIBRARY( crossvmm STATIC vmm/Boot.asm
                             vmm/cxxabiv1.cpp
                             vmm/DcRealMemory.cpp
                             vmm/Hypercall.asm
                             vmm/InterruptManager.cpp
                             vmm/Isr.asm
                             vmm/Main.cpp
                             vmm/memcpy.cpp
                             vmm/MemoryManager.cpp
                             vmm/PpcCpu.cpp
                             vmm/printf.cpp
                             vmm/Sh4aCpu.cpp
                             vmm/Shtuff.asm
                             vmm/XenonRealMemory.cpp )

ADD_CUSTOM_COMMAND( TARGET crossvmm
                    POST_BUILD
                    COMMAND ld -nodefaultlibs -T${CMAKE_CURRENT_LIST_DIR}/Linker.ld -o ${PROJECT_BINARY_DIR}/crossvmm.bin ${PROJECT_BINARY_DIR}/lib/libcrossvmm.a )


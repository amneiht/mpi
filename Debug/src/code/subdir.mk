################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/code/add.c \
../src/code/jpre.c \
../src/code/piplesort.c \
../src/code/pointjmp.c 

OBJS += \
./src/code/add.o \
./src/code/jpre.o \
./src/code/piplesort.o \
./src/code/pointjmp.o 

C_DEPS += \
./src/code/add.d \
./src/code/jpre.d \
./src/code/piplesort.d \
./src/code/pointjmp.d 


# Each subdirectory must supply rules for building sources it contributes
src/code/%.o: ../src/code/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	mpicc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



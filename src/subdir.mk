################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
./src/VolatileBulletinBoardFunctions.c \
./src/VolatileBulletinBoard.c \
./src/VolatileBulletinBoardUsers.c 

OBJS += \
./src/VolatileBulletinBoardFunctions.o \
./src/VolatileBulletinBoard.o \
./src/VolatileBulletinBoardUsers.o 

C_DEPS += \
./src/VolatileBulletinBoardFunctions.d \
./src/VolatileBulletinBoard.d \
./src/VolatileBulletinBoardUsers.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ./src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


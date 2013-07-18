################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Calibrate.cpp \
../Disparities.cpp \
../GreyDecode.cpp \
../GreyThresh.cpp \
../Main.cpp \
../Rectify.cpp \
../SolveProjection.cpp \
../Utils.cpp 

OBJS += \
./Calibrate.o \
./Disparities.o \
./GreyDecode.o \
./GreyThresh.o \
./Main.o \
./Rectify.o \
./SolveProjection.o \
./Utils.o 

CPP_DEPS += \
./Calibrate.d \
./Disparities.d \
./GreyDecode.d \
./GreyThresh.d \
./Main.d \
./Rectify.d \
./SolveProjection.d \
./Utils.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -Iopencv/include/opencv -O3 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



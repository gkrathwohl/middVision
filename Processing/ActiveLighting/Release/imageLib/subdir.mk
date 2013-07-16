################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../imageLib/Convert.cpp \
../imageLib/Convolve.cpp \
../imageLib/Image.cpp \
../imageLib/ImageIO.cpp \
../imageLib/ImageIOpng.cpp \
../imageLib/RefCntMem.cpp \
../imageLib/codeIO.cpp \
../imageLib/flowIO.cpp 

OBJS += \
./imageLib/Convert.o \
./imageLib/Convolve.o \
./imageLib/Image.o \
./imageLib/ImageIO.o \
./imageLib/ImageIOpng.o \
./imageLib/RefCntMem.o \
./imageLib/codeIO.o \
./imageLib/flowIO.o 

CPP_DEPS += \
./imageLib/Convert.d \
./imageLib/Convolve.d \
./imageLib/Image.d \
./imageLib/ImageIO.d \
./imageLib/ImageIOpng.d \
./imageLib/RefCntMem.d \
./imageLib/codeIO.d \
./imageLib/flowIO.d 


# Each subdirectory must supply rules for building sources it contributes
imageLib/%.o: ../imageLib/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



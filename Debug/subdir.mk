################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../askdbfunc.cpp \
../server.cpp \
../t.cpp \
../test2db.cpp \
../testdb.cpp \
../testsock.cpp 

O_SRCS += \
../askdbfunc.o 

OBJS += \
./askdbfunc.o \
./server.o \
./t.o \
./test2db.o \
./testdb.o \
./testsock.o 

CPP_DEPS += \
./askdbfunc.d \
./server.d \
./t.d \
./test2db.d \
./testdb.d \
./testsock.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -lpq -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



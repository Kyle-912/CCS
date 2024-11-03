################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
MiscFunctions/Signals/src/%.obj: ../MiscFunctions/Signals/src/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"C:/ti/ccs1280/ccs/tools/compiler/ti-cgt-arm_20.2.7.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me -Ooff --include_path="C:/ProgramData/Microsoft/Windows/Start Menu/Programs/Texas Instruments/Code Composer Studio 12.8.0/CCS/CCS/Lab5" --include_path="C:/ti/ccs1280/ccs/tools/compiler/ti-cgt-arm_20.2.7.LTS/include" --include_path="/home/administrator/.ti/SW-TM4C-2.2.0.295" --define=ccs="ccs" --define=PART_TM4C123GH6PM -g --c99 --gcc --printf_support=nofloat --diag_warning=225 --diag_wrap=off --display_error_number --abi=eabi --preproc_with_compile --preproc_dependency="MiscFunctions/Signals/src/$(basename $(<F)).d_raw" --obj_directory="MiscFunctions/Signals/src" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '



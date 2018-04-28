################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
ISRs.obj: ../ISRs.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C6000 Compiler'
	"C:/ti/ccsv6/tools/compiler/c6000_7.4.14/bin/cl6x" -mv6740 --abi=coffabi -g --include_path="C:/ti/ccsv6/tools/compiler/c6000_7.4.14/include" --display_error_number --diag_warning=225 --diag_wrap=off --printf_support=full --preproc_with_compile --preproc_dependency="ISRs.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

LCDK_Support_DSP.obj: ../LCDK_Support_DSP.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C6000 Compiler'
	"C:/ti/ccsv6/tools/compiler/c6000_7.4.14/bin/cl6x" -mv6740 --abi=coffabi -g --include_path="C:/ti/ccsv6/tools/compiler/c6000_7.4.14/include" --display_error_number --diag_warning=225 --diag_wrap=off --printf_support=full --preproc_with_compile --preproc_dependency="LCDK_Support_DSP.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

StartUp.obj: ../StartUp.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C6000 Compiler'
	"C:/ti/ccsv6/tools/compiler/c6000_7.4.14/bin/cl6x" -mv6740 --abi=coffabi -g --include_path="C:/ti/ccsv6/tools/compiler/c6000_7.4.14/include" --display_error_number --diag_warning=225 --diag_wrap=off --printf_support=full --preproc_with_compile --preproc_dependency="StartUp.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

assem.obj: ../assem.asm $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C6000 Compiler'
	"C:/ti/ccsv6/tools/compiler/c6000_7.4.14/bin/cl6x" -mv6740 --abi=coffabi -g --include_path="C:/ti/ccsv6/tools/compiler/c6000_7.4.14/include" --display_error_number --diag_warning=225 --diag_wrap=off --printf_support=full --preproc_with_compile --preproc_dependency="assem.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

autocorr.obj: ../autocorr.asm $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C6000 Compiler'
	"C:/ti/ccsv6/tools/compiler/c6000_7.4.14/bin/cl6x" -mv6740 --abi=coffabi -g --include_path="C:/ti/ccsv6/tools/compiler/c6000_7.4.14/include" --display_error_number --diag_warning=225 --diag_wrap=off --printf_support=full --preproc_with_compile --preproc_dependency="autocorr.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

flip.obj: ../flip.asm $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C6000 Compiler'
	"C:/ti/ccsv6/tools/compiler/c6000_7.4.14/bin/cl6x" -mv6740 --abi=coffabi -g --include_path="C:/ti/ccsv6/tools/compiler/c6000_7.4.14/include" --display_error_number --diag_warning=225 --diag_wrap=off --printf_support=full --preproc_with_compile --preproc_dependency="flip.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

libmfcc.obj: ../libmfcc.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C6000 Compiler'
	"C:/ti/ccsv6/tools/compiler/c6000_7.4.14/bin/cl6x" -mv6740 --abi=coffabi -g --include_path="C:/ti/ccsv6/tools/compiler/c6000_7.4.14/include" --display_error_number --diag_warning=225 --diag_wrap=off --printf_support=full --preproc_with_compile --preproc_dependency="libmfcc.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

main.obj: ../main.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C6000 Compiler'
	"C:/ti/ccsv6/tools/compiler/c6000_7.4.14/bin/cl6x" -mv6740 --abi=coffabi -g --include_path="C:/ti/ccsv6/tools/compiler/c6000_7.4.14/include" --display_error_number --diag_warning=225 --diag_wrap=off --printf_support=full --preproc_with_compile --preproc_dependency="main.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

svm.obj: ../svm.cpp $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C6000 Compiler'
	"C:/ti/ccsv6/tools/compiler/c6000_7.4.14/bin/cl6x" -mv6740 --abi=coffabi -g --include_path="C:/ti/ccsv6/tools/compiler/c6000_7.4.14/include" --display_error_number --diag_warning=225 --diag_wrap=off --printf_support=full --preproc_with_compile --preproc_dependency="svm.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

vectors.obj: ../vectors.asm $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C6000 Compiler'
	"C:/ti/ccsv6/tools/compiler/c6000_7.4.14/bin/cl6x" -mv6740 --abi=coffabi -g --include_path="C:/ti/ccsv6/tools/compiler/c6000_7.4.14/include" --display_error_number --diag_warning=225 --diag_wrap=off --printf_support=full --preproc_with_compile --preproc_dependency="vectors.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '



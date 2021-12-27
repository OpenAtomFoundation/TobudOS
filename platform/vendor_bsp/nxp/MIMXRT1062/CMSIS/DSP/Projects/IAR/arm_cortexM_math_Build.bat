@echo off

mkdir Temp
set TMP=Temp
set TEMP=Temp
rem set IAREXE=..\..\..\..\..\common\bin\iarbuild.exe
set IAREXE=iarbuild.exe

echo.
echo Building DSP Libraries ARM

echo   Building DSP Library for Cortex-M0 Little Endian
%IAREXE% arm_cortexM_math.ewp -build cortexM0l

echo   Building DSP Library for Cortex-M3 Little Endian
%IAREXE% arm_cortexM_math.ewp -build cortexM3l

echo   Building DSP Library for Cortex-M4 Little Endian
%IAREXE% arm_cortexM_math.ewp -build cortexM4l

echo   Building DSP Library for Cortex-M4 Little Endian with single precision FPU
%IAREXE% arm_cortexM_math.ewp -build cortexM4lf

echo   Building DSP Library for Cortex-M7 Little Endian
%IAREXE% arm_cortexM_math.ewp -build cortexM7l

echo   Building DSP Library for Cortex-M7 Little Endian with single precision FPU
%IAREXE% arm_cortexM_math.ewp -build cortexM7lfsp

echo   Building DSP Library for Cortex-M7 Little Endian with double precision FPU
%IAREXE% arm_cortexM_math.ewp -build cortexM7lfdp

echo   Building DSP Library for ARMv8-M Baseline Little Endian
%IAREXE% arm_cortexM_math.ewp -build ARMv8MBLl

echo   Building DSP Library for ARMv8-M Mainline Little Endian
%IAREXE% arm_cortexM_math.ewp -build ARMv8MMLl

echo   Building DSP Library for ARMv8-M Mainline Little Endian with single precision FPU
%IAREXE% arm_cortexM_math.ewp -build ARMv8MMLlfsp

echo   Building DSP Library for ARMv8-M Mainline Little Endian with double precision FPU
%IAREXE% arm_cortexM_math.ewp -build ARMv8MMLlfdp

echo   Building DSP Library for ARMv8-M Mainline Little Endian with DSP instructions
%IAREXE% arm_cortexM_math.ewp -build ARMv8MMLld

echo   Building DSP Library for ARMv8-M Mainline Little Endian with DSP instructions, single precision FPU
%IAREXE% arm_cortexM_math.ewp -build ARMv8MMLldfsp

echo   Building DSP Library for ARMv8-M Mainline Little Endian with DSP instructions, double precision FPU
%IAREXE% arm_cortexM_math.ewp -build ARMv8MMLldfdp


REM big endian libraries

echo   Building DSP Library for Cortex-M0 Big Endian
%IAREXE% arm_cortexM_math.ewp -build cortexM0b

echo   Building DSP Library for Cortex-M3 Big Endian
%IAREXE% arm_cortexM_math.ewp -build cortexM3b

echo   Building DSP Library for Cortex-M4 Big Endian
%IAREXE% arm_cortexM_math.ewp -build cortexM4b

echo   Building DSP Library for Cortex-M4 Big Endian with single precision FPU
%IAREXE% arm_cortexM_math.ewp -build cortexM4bf

echo   Building DSP Library for Cortex-M7 Big Endian
%IAREXE% arm_cortexM_math.ewp -build cortexM7b

echo   Building DSP Library for Cortex-M7 Big Endian with single precision FPU
%IAREXE% arm_cortexM_math.ewp -build cortexM7bfsp

echo   Building DSP Library for Cortex-M7 Big Endian with double precision FPU
%IAREXE% arm_cortexM_math.ewp -build cortexM7bfdp

echo Copy libs to CMSIS\lib\IAR
mkdir ..\..\lib\IAR
copy ReleaseM0BE\Exe\*.a ..\..\lib\IAR\ /Y
copy ReleaseM0LE\Exe\*.a ..\..\lib\IAR\ /Y
copy ReleaseM3BE\Exe\*.a ..\..\lib\IAR\ /Y
copy ReleaseM3LE\Exe\*.a ..\..\lib\IAR\ /Y
copy ReleaseM4BE\Exe\*.a ..\..\lib\IAR\ /Y
copy ReleaseM4BE_FPU\Exe\*.a ..\..\lib\IAR\ /Y
copy ReleaseM4LE\Exe\*.a ..\..\lib\IAR\ /Y
copy ReleaseM4LE_FPU\Exe\*.a ..\..\lib\IAR\ /Y
copy ReleaseM7BE\Exe\*.a ..\..\lib\IAR\ /Y
copy ReleaseM7BE_FPU_DP\Exe\*.a ..\..\lib\IAR\ /Y
copy ReleaseM7BE_FPU_SP\Exe\*.a ..\..\lib\IAR\ /Y
copy ReleaseM7LE\Exe\*.a ..\..\lib\IAR\ /Y
copy ReleaseM7LE_FPU_DP\Exe\*.a ..\..\lib\IAR\ /Y
copy ReleaseM7LE_FPU_SP\Exe\*.a ..\..\lib\IAR\ /Y
copy ARMv8MBLl\Exe\*.a ..\..\lib\IAR\ /Y
copy ARMv8MMLl\Exe\*.a ..\..\lib\IAR\ /Y
copy ARMv8MMLld\Exe\*.a ..\..\lib\IAR\ /Y
copy ARMv8MMLldfdp\Exe\*.a ..\..\lib\IAR\ /Y
copy ARMv8MMLldfsp\Exe\*.a ..\..\lib\IAR\ /Y
copy ARMv8MMLlfdp\Exe\*.a ..\..\lib\IAR\ /Y
copy ARMv8MMLlfsp\Exe\*.a ..\..\lib\IAR\ /Y

echo.
echo   Deleting intermediate files
for /d %%a in (Temp ARMv8M* ReleaseM*) do rmdir /s /q "%%~a"
del /Q *.bak
del /Q *.dep

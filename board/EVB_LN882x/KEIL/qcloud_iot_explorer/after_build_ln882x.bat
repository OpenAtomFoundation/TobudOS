
python ..\..\..\..\platform\vendor_bsp\LN\ln882x\tools\user_cmd\after_build_soc.py firmware_XIP

..\..\..\..\platform\vendor_bsp\LN\ln882x\tools\bin\mkimage.exe ln882x flashimage ..\..\..\..\platform\vendor_bsp\LN\ln882x\lib\boot_ram_ln882x.bin firmware_XIP.bin flashimage.bin release=1 crp_enable=0 app_version=10 hw_version=0

..\..\..\..\platform\vendor_bsp\LN\ln882x\tools\bin\fpu_patch.exe .\firmware_XIP.asm .\flashimage.bin



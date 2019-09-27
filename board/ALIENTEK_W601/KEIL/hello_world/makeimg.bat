@rem 参数0: exe
@rem 参数1: 输入bin文件 ,原始文件或者压缩档文件
@rem 参数2: 输出文件(目标生成文件）
@rem 参数3: 输入文件类型,0是旧参数布局的image文件，3是新参数布局的image文件
@rem 参数4: 是否压缩文件：0：plain文件，1：压缩类型文件
@rem 参数5: 版本号文件
@rem 参数6：升级文件再FLASH里的存放位置（相对位置）
@rem 参数7：升级后的文件启动位置（相对位置）
@rem 参数8：原始bin文件

@echo off

@rem 设置bin文件名字（这个名字由MDK魔术棒下->Output->Name of Executable决定）
set PATH_NAME=TencentOS_tiny
@rem 设置bin文件编译路径
set PATH_OUT=..\OBJ
@rem 设置TOOLS路径
set PATH_TOOLS=..\TOOLS
@rem 设置BIN存放路径(给W601升级用的文件路径)
set PATH_BIN=..\BIN

copy 	%PATH_OUT%\%PATH_NAME%.bin	%PATH_BIN%
cd 	%PATH_TOOLS%

copy	%PATH_BIN%\version.txt 	%PATH_BIN%\version_bk.txt

makeimg.exe 	"%PATH_BIN%\%PATH_NAME%.bin" 	"%PATH_BIN%\%PATH_NAME%.img"	0 0	"%PATH_BIN%\version.txt" 90000 10100
makeimg_all.exe 	"%PATH_BIN%\secboot.img" 	"%PATH_BIN%\%PATH_NAME%.img" 	"%PATH_BIN%\%PATH_NAME%.FLS"
@del "%PATH_BIN%\%PATH_NAME%.img"



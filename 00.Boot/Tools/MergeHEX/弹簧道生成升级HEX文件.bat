::弹簧道主控板，电机板生成发布HEX版本
::复制并重命名文件：release_dianji.hex -> dianji_2.hex BOOT_DianJi.hex -> dianji_1.hex;
::release_zhukong.hex -> zhukong_2.hex BOOT_ZhuKong.hex -> Zhukong_1.hex;
::然后，合并hex文件，生成指定文件名的发布版本
@echo off
::合并HEX后生成的发布版本文件名
set RELEASE_ZHUKONG=01Zhukong-28.0315.hex
set RELEASE_DIANJI=02Dianji-48.0315.hex
::删除所有的hex文件，可以指定删除目录的文件。否则，以后重命名的时候会报错。必须指定/s，指定当前目录及其子目录
del *.hex /s
::复制指定位置的文件到目录中，/Y为强制覆盖不提示，/-Y为提示你确认信息（YES/NO/ALL）；/D为只复制源文件比目标文件新的文件
::电机板hex处理
copy ..\..\02.Dianji_IAP\PURE\BOOT_DianJi.hex		.\hex\		/Y/D
copy ..\..\..\02.dianji\PURE\release_dianji.hex 	.\hex\		/Y/D
ren hex\BOOT_DianJi.hex		dianji_1.hex
ren hex\release_dianji.hex	dianji_2.hex
::主控板hex处理
copy ..\..\01.Zhukong_IAP\PURE\BOOT_ZhuKong.hex		.\hex\		/Y/D
copy ..\..\..\01.zhukon\PURE\release_zhukong.hex 	.\hex\		/Y/D
ren hex\BOOT_ZhuKong.hex		zhukong_1.hex
ren hex\release_zhukong.hex		zhukong_2.hex
::先删除之前生成的文件，保证每次都刷新并执行
::del output\*.hex 
merge\mergehex.exe --merge hex\zhukong_1.hex	hex\zhukong_2.hex --output	output\%RELEASE_ZHUKONG%
merge\mergehex.exe --merge hex\dianji_1.hex		hex\dianji_2.hex --output	output\%RELEASE_DIANJI%
::如果是直接点击bat文件使用，需要pause暂停程序执行，观察屏幕的执行输出，特别是有时候执行出错的情况；但是如果在KEIL中，配置
::自定义工具使用，不需要，因为执行输出的信息会在Build Output窗口输出显示，如果添加了pause，需要我们手动关闭弹出的窗口。
::pause

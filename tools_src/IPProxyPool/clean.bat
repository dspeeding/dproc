:: 文 件 名：clean.bat
:: 系    统：dproc
:: 项    目：
:: 作    者：Dong XiaoLin
:: 功    能：通过脚本清理工程编译的中间文件
:: 创建日期：2017.01.16
:: 修    改：----2017.01.16----修改该批处理文件----
::
:: 备    注：无

@echo off

:MAIN

del *.sdf /a /s /f
del *.suo /a /s /f
del *.aps /a /s /f
del *.log /a /s /f
del *.db /a /s /f

rd .vs /s /q
rd bin /s /q
rd obj /s /q

CALL :FUNC_Clean .\


pause
exit

GOTO :EOF 

:FUNC_Clean

echo         --------------清理 %1项目 开始----------
IF EXIST %1\debug (
	rd %1\debug /s /q
)
IF EXIST %1\release (
	rd %1\release /s /q
)
IF EXIST %1\bin (
	rd %1\bin /s /q
)
IF EXIST %1\obj (
	rd %1\obj /s /q
)
IF EXIST %1\X64 (
	rd %1\X64 /s /q
)
IF EXIST %1\ipch (
	rd %1\ipch /s /q
)

echo         --------------清理 %1项目 成功----------

GOTO :EOF 
:: �� �� ����clean.bat
:: ϵ    ͳ��dproc
:: ��    Ŀ��
:: ��    �ߣ�Dong XiaoLin
:: ��    �ܣ�ͨ���ű������̱�����м��ļ�
:: �������ڣ�2017.01.16
:: ��    �ģ�----2017.01.16----�޸ĸ��������ļ�----
::
:: ��    ע����

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

echo         --------------���� %1��Ŀ ��ʼ----------
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

echo         --------------���� %1��Ŀ �ɹ�----------

GOTO :EOF 
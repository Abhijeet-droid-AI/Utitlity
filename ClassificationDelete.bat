set TC_ROOT=C:\plm\teamcenter\tc13
set TC_DATA=\\gptcdv-ep-atc04\apps\TCDATA
call %TC_DATA%\tc_profilevars.bat

call C:\Users\ayesha.shaik\source\repos\ClassificationUtilitiesTC13\x64\Release\ClassificationDelete.exe -u=tata_user -p=JE#Dev2023 -g=dba -f=C:\Users\ayesha.shaik\source\repos\ClassificationUtilitiesTC13\ClassificationDelete\DeleteInput.txt -log=C:\Users\ayesha.shaik\source\repos\ClassificationUtilitiesTC13\ClassificationDelete\Logs\

pause 

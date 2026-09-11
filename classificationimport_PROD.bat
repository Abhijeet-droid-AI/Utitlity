echo on
title classificationImporter

set TC_DEBUG=ON
set TC_KEEP_SYSTEM_LOG=true
set FMS_HOME=D:\apps\siemens\teamcenter\TC13DEV\tcroot\tccs
set TC_ROOT=D:\apps\siemens\teamcenter\TC13DEV\tcroot
set TC_DATA=V:\tcdata
call %TC_DATA%\tc_profilevars.bat

cd C:\TTL\TC\Utilities\Dhanashree\classificationImportUtilityexe\classificationImportUtility


call C:\TTL\TC\Utilities\Dhanashree\classificationImportUtilityexe\classificationImportUtility\ClassificationImport.exe -u=gpdm_migration -p=gpdm_migrat10n -g=dba -f="C:\TTL\TC\Utilities\Dhanashree\classificationImportUtilityexe\classificationImportUtility\test.csv" -log=C:\TTL\TC\Utilities\Dhanashree\classificationImportUtilityexe\classificationImportUtility\Logs\

pause
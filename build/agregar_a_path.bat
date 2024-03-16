@echo off
setlocal EnableDelayedExpansion

REM Ruta al directorio del arduino-cli
set "arduinoCliDir=\arduino-cli_0.35.3_Windows_64bit"

REM Agregar el directorio al PATH
set "path=%path%;%arduinoCliDir%"

echo Directorio del arduino-cli agregado al PATH correctamente.
echo.
echo Cierra esta ventana y abre una nueva ventana del símbolo del sistema para que los cambios surtan efecto.
pause

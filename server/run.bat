@echo off

net start mosquitto

cd /d "%~dp0"

call venv\Scripts\activate.bat

python cli.py

pause
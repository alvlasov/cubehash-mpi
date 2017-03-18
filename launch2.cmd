@echo off
cd C:\MPIFolder
mpiexec -n 5 Debug/HashMPI.exe
pause
exit
param ([string]$FilePath = "split_chunk_1.vcd")
clear

if (-not (Test-Path $FilePath)) { Write-Host "CRITICAL ERROR: Target data file missing." -ForegroundColor Red; Exit }

Write-Host "==========================================" -ForegroundColor Cyan
Write-Host "   EXECUTING WINDOWS CMAKE BUILD ENGINE   " -ForegroundColor Cyan
Write-Host "==========================================" -ForegroundColor Cyan

if (-not (Test-Path build)) { New-Item -ItemType Directory -Path build | Out-Null }
cd build
cmake ..
cmake --build . --config Release
if (-not $? ) { Write-Host "BUILD FAILED" -ForegroundColor Red; cd ..; Exit }
cd ..

Write-Host "`nLAUNCHING CONSOLIDATED MATRIX VIA LIBRARY IMPLEMENTATIONS...`n" -ForegroundColor Green
$Process = Start-Process ".\build\apps\benchmark_runner.exe" -ArgumentList "`"$FilePath`"" -NoNewWindow -PassThru -Wait

Write-Host "`n------------------------------------------" -ForegroundColor Cyan
Write-Host "  Windows Kernel OS Resource Telemetry     " -ForegroundColor Cyan
Write-Host "------------------------------------------" -ForegroundColor Cyan
if ($Process) {
    Write-Host "Peak Memory Consumption : $([math]::round($Process.PeakWorkingSet64 / 1MB, 2)) MB"
    Write-Host "Total Active CPU Burn   : $($Process.TotalProcessorTime.ToString('hh\:mm\:ss\.fff'))"
}
Write-Host "------------------------------------------" -ForegroundColor Cyan
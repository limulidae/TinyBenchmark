Set-Location -Path $PSScriptRoot

& xperf -stop 2>$null
& xperf -stop tiny 2>$null

Remove-Item *.etl
Remove-Item *.csv

#powercfg -setacvalueindex scheme_current sub_processor procthrottlemin 50
#powercfg -setacvalueindex scheme_current sub_processor procthrottlemax 50
#powercfg -setactive scheme_current

for ($i = 0; $i -lt 100; $i++) {
    $dir = "$i"
    New-Item -Path $dir -ItemType Directory -Force
    Set-Location -Path $dir

    $user_fn    = ${i}.ToString() + "_tiny.etl"
    $kernel_fn  = ${i}.ToString() + "_kernel.etl"
    $merged_fn  = ${i}.ToString() + "_merged.etl"
    $merged_csv = ${i}.ToString() + "_merged.csv"

    Get-Process "tinybenchmark" -ErrorAction SilentlyContinue | Stop-Process -ErrorAction SilentlyContinue

    & xperf -on SysProf+IDLE_STATES+POWER+CSWITCH+DISPATCHER
    & xperf -start tiny -on Microsoft-Windows-Kernel-Pep+Microsoft-Windows-Kernel-Processor-Power:0xC2:4 -f $user_fn

    & ..\tinybenchmark.exe

    & xperf -stop -d $kernel_fn
    & xperf -stop tiny
    & xperf -merge $user_fn $kernel_fn $merged_fn 2>$null
    & xperf -i $merged_fn -o $merged_csv

    Remove-Item $user_fn -ErrorAction SilentlyContinue
    Remove-Item $kernel_fn -ErrorAction SilentlyContinue

    Start-Sleep -Seconds 3
    Set-Location -Path ..
}

#powercfg -setacvalueindex scheme_current sub_processor procthrottlemin 5
#powercfg -setacvalueindex scheme_current sub_processor procthrottlemax 100
#powercfg -setactive scheme_current

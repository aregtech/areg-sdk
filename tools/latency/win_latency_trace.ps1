<#
.SYNOPSIS
    Phase-0 latency-trace harness driver for areg-sdk example 30 (Windows 11, MSVC Release).

.DESCRIPTION
    Builds (optional) example 30 + mtrouter with AREG_LATENCY_TRACE enabled, then runs the
    ping-pong benchmark and collects:
      - the per-stage latency-trace tables (printed to STDERR at clean process exit), and
      - the consumer's pp result table (printed to STDOUT).
    Results are written to a single markdown file for review / cross-agent exchange.

    IMPORTANT (atexit): the trace table is emitted by std::atexit, which runs ONLY on a clean
    exit. All three processes are therefore quit via their console "-q" command, never killed.

.NOTES
    Measure on native Windows only. Do NOT use WSL numbers.
    This script is a best-effort starting point; verify behaviour on the real machine and adjust
    timings / paths as needed (see the coordination doc 2026-06-05-4-windows-latency-coordination.md).

.EXAMPLE
    powershell -ExecutionPolicy Bypass -File tools\latency\win_latency_trace.ps1 -Build
    powershell -ExecutionPolicy Bypass -File tools\latency\win_latency_trace.ps1 -Mode pp64 -Count 10000 -Warmup 1000
#>
param(
    [string]$Mode       = "pp64",
    [int]   $Count      = 10000,
    [int]   $Warmup     = 1000,
    [int]   $RunWaitSec = 30,                 # seconds to let the run finish before sending -q
    [switch]$Build,
    [string]$BinDir     = "",                 # auto-detected if empty
    [string]$Generator  = "Visual Studio 17 2022",
    [string]$Arch       = "x64",
    [string]$OutFile    = ""                  # results markdown; auto-named if empty
)

$ErrorActionPreference = "Stop"
$repo = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)   # tools\latency -> repo root
Write-Host "Repo root: $repo"

# ---------------------------------------------------------------------------
# 1) Build (optional) with the trace flag enabled.
# ---------------------------------------------------------------------------
if ($Build) {
    $bd = Join-Path $repo "build-win-lt"
    Write-Host "Configuring $bd with /DAREG_LATENCY_TRACE ..."
    & cmake -B $bd -G $Generator -A $Arch -DAREG_TESTS=OFF -DAREG_EXAMPLES=ON -DCMAKE_CXX_FLAGS="/DAREG_LATENCY_TRACE"
    if ($LASTEXITCODE -ne 0) { throw "cmake configure failed" }
    Write-Host "Building mtrouter, 30_pubconsumer, 30_pubprovider (Release) ..."
    & cmake --build $bd --config Release --target mtrouter 30_pubconsumer 30_pubprovider
    if ($LASTEXITCODE -ne 0) { throw "cmake build failed" }
}

# ---------------------------------------------------------------------------
# 2) Locate the binaries.
# ---------------------------------------------------------------------------
if ([string]::IsNullOrEmpty($BinDir)) {
    $candidate = Get-ChildItem -Path (Join-Path $repo "product\build") -Recurse -Filter "30_pubconsumer.exe" -ErrorAction SilentlyContinue |
                 Sort-Object LastWriteTime -Descending | Select-Object -First 1
    if ($null -eq $candidate) { throw "Could not find 30_pubconsumer.exe under product\build. Build first (-Build)." }
    $BinDir = $candidate.DirectoryName
}
Write-Host "Bin dir: $BinDir"

$mtr  = Join-Path $BinDir "mtrouter.exe"
$prov = Join-Path $BinDir "30_pubprovider.exe"
$cons = Join-Path $BinDir "30_pubconsumer.exe"
foreach ($e in @($mtr,$prov,$cons)) { if (-not (Test-Path $e)) { throw "Missing: $e" } }

if ([string]::IsNullOrEmpty($OutFile)) {
    $stamp = Get-Date -Format "yyyyMMdd_HHmmss"
    $OutFile = Join-Path $repo ("product\tasks\latency-results-win-{0}-{1}.md" -f $Mode,$stamp)
}

# ---------------------------------------------------------------------------
# Helper: start a process with stdin pipe + async stdout/stderr drained to files.
# ---------------------------------------------------------------------------
function Start-Captured {
    param([string]$Exe, [string]$OutPath, [string]$ErrPath)

    $psi = New-Object System.Diagnostics.ProcessStartInfo
    $psi.FileName               = $Exe
    $psi.WorkingDirectory       = (Split-Path $Exe)
    $psi.UseShellExecute        = $false
    $psi.RedirectStandardInput  = $true
    $psi.RedirectStandardOutput = $true
    $psi.RedirectStandardError  = $true

    $p = New-Object System.Diagnostics.Process
    $p.StartInfo = $psi
    [void]$p.Start()

    # Drain pipes asynchronously to files so they never fill up and deadlock the child.
    $outFs = [System.IO.File]::Create($OutPath)
    $errFs = [System.IO.File]::Create($ErrPath)
    $tOut  = $p.StandardOutput.BaseStream.CopyToAsync($outFs)
    $tErr  = $p.StandardError.BaseStream.CopyToAsync($errFs)

    return [pscustomobject]@{ Proc=$p; OutFs=$outFs; ErrFs=$errFs; TOut=$tOut; TErr=$tErr; Out=$OutPath; Err=$ErrPath }
}

function Send-Line { param($Ctx, [string]$Line) ; $Ctx.Proc.StandardInput.WriteLine($Line); $Ctx.Proc.StandardInput.Flush() }

function Stop-Clean {
    param($Ctx, [int]$TimeoutMs = 8000)
    try { Send-Line $Ctx "-q" } catch {}
    if (-not $Ctx.Proc.WaitForExit($TimeoutMs)) { Write-Warning "Process $($Ctx.Proc.Id) did not exit cleanly; trace may be missing." }
    try { $Ctx.TOut.Wait(2000) | Out-Null } catch {}
    try { $Ctx.TErr.Wait(2000) | Out-Null } catch {}
    $Ctx.OutFs.Flush(); $Ctx.OutFs.Close()
    $Ctx.ErrFs.Flush(); $Ctx.ErrFs.Close()
}

# ---------------------------------------------------------------------------
# 3) Run: mtrouter -> provider -> consumer, drive pp benchmark, quit all cleanly.
# ---------------------------------------------------------------------------
$tmp = Join-Path $env:TEMP ("areg_lt_{0}" -f (Get-Date -Format "HHmmss"))
New-Item -ItemType Directory -Force -Path $tmp | Out-Null
Push-Location $BinDir
Get-ChildItem -Path $BinDir -Filter "latency_*.csv" -ErrorAction SilentlyContinue | Remove-Item -Force -ErrorAction SilentlyContinue

Write-Host "Starting mtrouter ..."
$cMtr = Start-Captured $mtr (Join-Path $tmp "mtr.out") (Join-Path $tmp "mtr.err")
Start-Sleep -Seconds 2
Write-Host "Starting provider ..."
$cPrv = Start-Captured $prov (Join-Path $tmp "prov.out") (Join-Path $tmp "prov.err")
Start-Sleep -Seconds 2
Write-Host "Starting consumer ..."
$cCon = Start-Captured $cons (Join-Path $tmp "cons.out") (Join-Path $tmp "cons.err")
Start-Sleep -Seconds 2

Write-Host "Driving benchmark: -m $Mode -c $Count -w $Warmup -s"
Send-Line $cCon "-m $Mode"; Start-Sleep -Milliseconds 500
Send-Line $cCon "-c $Count"; Start-Sleep -Milliseconds 500
Send-Line $cCon "-w $Warmup"; Start-Sleep -Milliseconds 500
Send-Line $cCon "-s"
Write-Host "Waiting $RunWaitSec s for the run to complete ..."
Start-Sleep -Seconds $RunWaitSec

# Quit cleanly (order: consumer, provider, mtrouter) so each fires its atexit trace dump.
Write-Host "Quitting consumer / provider / mtrouter cleanly ..."
Stop-Clean $cCon
Stop-Clean $cPrv
Stop-Clean $cMtr
Pop-Location

# ---------------------------------------------------------------------------
# 4) Collect results into one markdown file.
# ---------------------------------------------------------------------------
function Get-TraceTable { param([string]$Path)
    if (-not (Test-Path $Path)) { return "(no file)" }
    $txt = Get-Content $Path -Raw
    $m = [regex]::Match($txt, "(?s)=+ AREG LATENCY TRACE.*?=+\r?\n")
    if ($m.Success) { return $m.Value } else { return "(no trace table found in $Path)" }
}
function Get-Tail { param([string]$Path,[int]$N=40)
    if (-not (Test-Path $Path)) { return "(no file)" }
    return (Get-Content $Path -Tail $N | Out-String)
}

$csv = Get-ChildItem -Path $BinDir -Filter "latency_*.csv" -ErrorAction SilentlyContinue | Select-Object -First 1

$sb = New-Object System.Text.StringBuilder
[void]$sb.AppendLine("# Windows latency-trace results - $Mode (count=$Count, warmup=$Warmup)")
[void]$sb.AppendLine("")
[void]$sb.AppendLine("- Host: $($env:COMPUTERNAME)  |  Date: $(Get-Date -Format s)")
[void]$sb.AppendLine("- BinDir: ``$BinDir``")
[void]$sb.AppendLine("- CSV produced: $([bool]$csv) $(if($csv){'(' + $csv.Name + ')'})")
[void]$sb.AppendLine("")
[void]$sb.AppendLine("## Consumer - per-stage trace (STDERR)")
[void]$sb.AppendLine('```'); [void]$sb.AppendLine((Get-TraceTable $cCon.Err).Trim()); [void]$sb.AppendLine('```')
[void]$sb.AppendLine("## Provider - per-stage trace (STDERR)")
[void]$sb.AppendLine('```'); [void]$sb.AppendLine((Get-TraceTable $cPrv.Err).Trim()); [void]$sb.AppendLine('```')
[void]$sb.AppendLine("## mtrouter - per-stage trace (STDERR)")
[void]$sb.AppendLine('```'); [void]$sb.AppendLine((Get-TraceTable $cMtr.Err).Trim()); [void]$sb.AppendLine('```')
[void]$sb.AppendLine("## Consumer - pp result table (STDOUT tail)")
[void]$sb.AppendLine('```'); [void]$sb.AppendLine((Get-Tail $cCon.Out 50).Trim()); [void]$sb.AppendLine('```')

Set-Content -Path $OutFile -Value $sb.ToString() -Encoding UTF8
Write-Host ""
Write-Host "==================== RESULTS ===================="
Write-Host (Get-Content $OutFile -Raw)
Write-Host "Written to: $OutFile"
Write-Host "Raw logs in: $tmp"

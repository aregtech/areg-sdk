# =============================================================================
#  bitness.ps1
#
#  Runs mtrouter, the provider and the consumer of areg-bitness-test from a 32-bit
#  and a 64-bit build, in every mix, and checks that each process reads the other's
#  sample exactly. Windows PowerShell 5.1 or PowerShell 7; tests/bitness/bitness.sh
#  on Linux, macOS and Cygwin.
#
#  Usage:  bitness.ps1 --bin64 <dir> [--bin32 <dir>] [--timeout <s>]
#  Without --bin32 only the 64-bit build runs, which checks the test itself.
#  Exit:   0 = every run passed, 1 = a run failed, 2 = bad argument or missing file.
# =============================================================================

$ErrorActionPreference = 'Stop'

$Bin = @{ '64' = ''; '32' = '' }
$Timeout = 60

function Stop-Run([string]$Message) {
    [Console]::Error.WriteLine("error: $Message")
    exit 2
}

for ($i = 0; $i -lt $args.Count; $i++) {
    switch ($args[$i]) {
        '--bin64'   { $i++; $Bin['64'] = $args[$i] }
        '--bin32'   { $i++; $Bin['32'] = $args[$i] }
        '--timeout' { $i++; $Timeout = [int]$args[$i] }
        default     { Stop-Run "unknown argument: $($args[$i])" }
    }
}

if (-not $Bin['64']) { Stop-Run '--bin64 is required' }
foreach ($key in @('64', '32')) {
    if ($Bin[$key]) {
        if (-not (Test-Path $Bin[$key] -PathType Container)) { Stop-Run "no directory $($Bin[$key])" }
        $Bin[$key] = (Resolve-Path $Bin[$key]).Path
    }
}

# A port nothing listens on.
function Get-FreePort {
    $listener = [System.Net.Sockets.TcpListener]::new([System.Net.IPAddress]::Loopback, 0)
    $listener.Start()
    $port = $listener.LocalEndpoint.Port
    $listener.Stop()
    return $port
}

# True while something accepts connections on the port.
function Test-Listening([int]$Port) {
    $client = [System.Net.Sockets.TcpClient]::new()
    try {
        $client.Connect('127.0.0.1', $Port)
        return $true
    } catch {
        return $false
    } finally {
        $client.Dispose()
    }
}

# The router's areg.init with this run's port and logging off.
function Write-Config([string]$Dir, [string]$Path, [int]$Port) {
    $text = Get-Content -Raw (Join-Path $Dir 'config\areg.init')
    $keys = [ordered]@{
        'router::*::port::tcpip'    = "$Port"
        'router::*::address::tcpip' = '127.0.0.1'
        'service::*::list'          = 'router'
        'log::*::enable'            = 'false'
    }
    foreach ($key in $keys.Keys) {
        $pattern = '(?m)^(\s*' + [regex]::Escape($key) + '\s*=)[^\r\n]*'
        $text = [regex]::Replace($text, $pattern, '$1 ' + $keys[$key])
    }
    Set-Content -Path $Path -Value $text -Encoding ASCII
}

# Starts a process from its bin directory with its output and its input redirected.
function Start-Child([string]$Dir, [string]$Exe, [string]$Arguments) {
    $info = [System.Diagnostics.ProcessStartInfo]::new((Join-Path $Dir $Exe), $Arguments)
    $info.WorkingDirectory = $Dir
    $info.UseShellExecute = $false
    $info.RedirectStandardInput = $true
    $info.RedirectStandardOutput = $true
    $info.RedirectStandardError = $true
    $info.CreateNoWindow = $true
    return [System.Diagnostics.Process]::Start($info)
}

function Stop-Child($Proc) {
    if (($null -ne $Proc) -and (-not $Proc.HasExited)) {
        $Proc.Kill()
        $Proc.WaitForExit()
    }
}

# Prints what is wrong in one process's output: its bitness, a missing line, a differing field.
function Get-Problems([string]$Text, [string]$Side, [string]$Bits, [string[]]$Lines) {
    $found = @()
    if ($Text -notmatch "bitness: $Side is $Bits-bit") { $found += "the $Side is not the $Bits-bit build" }
    foreach ($line in $Lines) {
        if (-not $Text.Contains("bitness: $line")) { $found += "${Side}: `"$line`" missing" }
    }
    foreach ($line in ($Text -split "`r?`n")) {
        if ($line -match 'differs') { $found += "${Side}: $($line.Trim())" }
    }
    return $found
}

# One run of router, provider and consumer bitness; returns its problems.
function Invoke-Mix([string]$RouterBits, [string]$ProviderBits, [string]$ConsumerBits) {
    $routerDir = $Bin[$RouterBits]; $providerDir = $Bin[$ProviderBits]; $consumerDir = $Bin[$ConsumerBits]
    foreach ($file in @((Join-Path $routerDir 'mtrouter.exe'), (Join-Path $providerDir 'areg-bitness-test.exe'), (Join-Path $consumerDir 'areg-bitness-test.exe'))) {
        if (-not (Test-Path $file)) { return @("$file not found") }
    }

    $work = Join-Path ([System.IO.Path]::GetTempPath()) ('areg-bitness-' + [guid]::NewGuid().ToString('N').Substring(0, 8))
    New-Item -ItemType Directory -Path $work | Out-Null
    $port = Get-FreePort
    $config = Join-Path $work 'areg.init'
    Write-Config $routerDir $config $port

    $problems = @()
    $router = $null; $provider = $null; $consumer = $null
    try {
        # The console quits when its stdin ends; the redirected input stays open until the kill.
        $router = Start-Child $routerDir 'mtrouter.exe' "--console `"--load=$config`""
        $router.BeginOutputReadLine()
        $router.BeginErrorReadLine()
        $deadline = (Get-Date).AddSeconds(10)
        while (-not (Test-Listening $port) -and ((Get-Date) -lt $deadline)) { Start-Sleep -Milliseconds 100 }
        if (-not (Test-Listening $port)) { return @("mtrouter did not listen on $port") }

        $provider = Start-Child $providerDir 'areg-bitness-test.exe' "provider `"$config`""
        $providerOut = $provider.StandardOutput.ReadToEndAsync()
        $consumer = Start-Child $consumerDir 'areg-bitness-test.exe' "consumer `"$config`""
        $consumerOut = $consumer.StandardOutput.ReadToEndAsync()

        if (-not $consumer.WaitForExit($Timeout * 1000)) {
            $problems += "the consumer did not finish in $Timeout s"
            Stop-Child $consumer
        } elseif ($consumer.ExitCode -ne 0) {
            $problems += "the consumer exited $($consumer.ExitCode)"
        }
    } finally {
        Stop-Child $consumer
        Stop-Child $provider
        Stop-Child $router
    }

    $problems += Get-Problems $consumerOut.Result 'consumer' $ConsumerBits @('response ok', 'attribute ok', 'broadcast ok')
    $problems += Get-Problems $providerOut.Result 'provider' $ProviderBits @('provider request ok')
    return $problems
}

$mixes = @(@('64', '64', '64'), @('64', '64', '32'), @('64', '32', '64'), @('32', '64', '32'), @('32', '32', '32'))
$total = 0
$failed = 0
foreach ($mix in $mixes) {
    if (($mix -contains '32') -and (-not $Bin['32'])) { continue }
    $total++
    $problems = @(Invoke-Mix $mix[0] $mix[1] $mix[2])
    $label = "router $($mix[0])-bit, provider $($mix[1])-bit, consumer $($mix[2])-bit"
    if ($problems.Count -eq 0) {
        Write-Output "PASS: $label"
    } else {
        Write-Output "FAIL: $label"
        foreach ($problem in $problems) { Write-Output "    $problem" }
        $failed++
    }
}

Write-Output "$($total - $failed) of $total runs passed"
if ($failed -gt 0) { exit 1 }
exit 0

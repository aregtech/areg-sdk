# =============================================================================
#  setup-project.ps1
#
#  Creates a ready-to-build Areg project outside the SDK. Windows PowerShell 5.1
#  or PowerShell 7, no Python. It writes the same project as
#  tools/agent/setup_project.py: one of the recipes under docs/agent/recipes/,
#  copied and renamed. The agent files (AGENTS.md, design.json, scenarios.json)
#  are written by the Python tool only.
#
#  Usage:  setup-project.ps1 [--name NAME] [--root DIR] [--mode local|ipc|pubsub]
#                            [--sdk-root DIR] [--tag TAG] [--force]
#  Asks for a missing name, mode or directory when run on a console.
#  Exit:   0 = created, 1 = bad argument or refused, 2 = a required tool is missing.
# =============================================================================

$ErrorActionPreference = 'Stop'

$Here = Split-Path -Parent $MyInvocation.MyCommand.Path
$FallbackTag = 'master'

# The recipe and the CMake name tokens of each mode, as in setup_project.py.
$Modes = @{
    'local' = @{ Recipe = '01-local-single-process'
                 Tokens = @('hello_local={name}') }
    'ipc'   = @{ Recipe = '02-ipc-two-processes'
                 Tokens = @('hello_provider={name}_provider', 'hello_consumer={name}_consumer', 'hello_ipc={name}') }
    'pubsub' = @{ Recipe = '03-attributes-and-broadcast'
                  Tokens = @('gen_monitor=gen_{name}', '"monitor"="{name}"', 'macro_declare_executable(monitor =macro_declare_executable({name} ') }
}

function Stop-Setup([string]$Message, [int]$Code = 1) {
    [Console]::Error.WriteLine("error: $Message")
    exit $Code
}

function Show-Usage {
    Write-Output 'usage: setup-project.ps1 [--name NAME] [--root DIR] [--mode local|ipc|pubsub]'
    Write-Output '                         [--sdk-root DIR] [--tag TAG] [--force]'
    Write-Output ''
    Write-Output 'Create a ready-to-build Areg project.'
    Write-Output ''
    Write-Output '  --name NAME      project name; a C identifier'
    Write-Output '  --root DIR       directory to create; defaults to ./<name>'
    Write-Output '  --mode MODE      local: one process, two threads. ipc: two processes.'
    Write-Output '                   pubsub: local, whose interface also declares attributes'
    Write-Output '                   and broadcasts'
    Write-Output '  --sdk-root DIR   use this local SDK copy instead of fetching from GitHub'
    Write-Output '  --tag TAG        SDK git tag to fetch'
    Write-Output '  --force          scaffold into a directory that is not empty; files of the'
    Write-Output '                   same name are overwritten, nothing is deleted'
}

function Read-Answer([string]$Prompt, [string]$Default) {
    Write-Host -NoNewline "$Prompt [$Default]: "
    $answer = [Console]::ReadLine()
    if ([string]::IsNullOrWhiteSpace($answer)) { return $Default }
    return $answer.Trim()
}

function Write-Text([string]$Path, [string]$Text) {
    [IO.File]::WriteAllText($Path, $Text, (New-Object Text.UTF8Encoding $false))
}

function Read-Lf([string]$Path) {
    return ([IO.File]::ReadAllText($Path)) -replace "`r`n", "`n"
}

$Name = $null; $Root = $null; $Mode = $null; $SdkRoot = $null; $Tag = $null; $Force = $false

$i = 0
while ($i -lt $args.Count) {
    $option = [string]$args[$i]
    if ($option -ceq '--force') { $Force = $true; $i += 1; continue }
    if ($option -ceq '-h' -or $option -ceq '--help') { Show-Usage; exit 0 }
    if (@('--name', '--root', '--mode', '--sdk-root', '--tag') -cnotcontains $option) {
        Stop-Setup "unknown option: $option (see --help)"
    }
    if ($i + 1 -ge $args.Count) { Stop-Setup "$option needs a value" }
    $value = [string]$args[$i + 1]
    switch -CaseSensitive ($option) {
        '--name'     { $Name = $value }
        '--root'     { $Root = $value }
        '--mode'     { $Mode = $value }
        '--sdk-root' { $SdkRoot = $value }
        '--tag'      { $Tag = $value }
    }
    $i += 2
}

$AgentDocs = $null
foreach ($candidate in @((Join-Path $Here '..\docs\agent'), (Join-Path $Here '..\..\share\areg\sdk\docs\agent'))) {
    if (Test-Path (Join-Path $candidate 'recipes') -PathType Container) {
        $AgentDocs = (Resolve-Path $candidate).ProviderPath
        break
    }
}
if (-not $AgentDocs) {
    Stop-Setup "agent documentation not found next to $Here. Expected it at ..\docs\agent (a clone) or ..\..\share\areg\sdk\docs\agent (an installation)."
}

$DefaultTag = $FallbackTag
try {
    $stated = (Get-Content -Raw -Path (Join-Path $AgentDocs 'api.json') | ConvertFrom-Json).sdk.fetch_ref
    if ($stated) { $DefaultTag = $stated }
} catch { }
if (-not $Tag) { $Tag = $DefaultTag }

if (-not [Console]::IsInputRedirected) {
    if (-not $Name) { $Name = Read-Answer 'Project name' 'areg_hello' }
    if (-not $Mode) { $Mode = Read-Answer 'Mode (local, ipc, pubsub)' 'local' }
    if (-not $Root) { $Root = Read-Answer 'Project directory' "./$Name" }
} else {
    if (-not $Name) { Stop-Setup '--name is required when there is no terminal to ask on' }
    if (-not $Mode) { $Mode = 'local' }
    if (-not $Root) { $Root = "./$Name" }
}

if (-not $Modes.ContainsKey($Mode) -or $Mode -cne $Mode.ToLower()) { Stop-Setup "unknown mode: $Mode (local, ipc or pubsub)" }
if ($Name -cnotmatch '^[A-Za-z_][A-Za-z0-9_]*$') {
    Stop-Setup "project name `"$Name`" is not a C identifier; it becomes a CMake target"
}

if ($SdkRoot) {
    if (-not (Test-Path (Join-Path $SdkRoot 'areg.cmake') -PathType Leaf)) { Stop-Setup "no areg.cmake under --sdk-root $SdkRoot" }
    $SdkRoot = ([IO.Path]::GetFullPath((Resolve-Path $SdkRoot).ProviderPath)).TrimEnd('\', '/') -replace '\\', '/'
}

if (-not (Get-Command cmake -ErrorAction SilentlyContinue)) { Stop-Setup 'not found on PATH: cmake' 2 }
if (-not $SdkRoot -and -not (Get-Command git -ErrorAction SilentlyContinue)) { Stop-Setup 'not found on PATH: git' 2 }

$Root = [IO.Path]::GetFullPath([IO.Path]::Combine((Get-Location).ProviderPath, $Root))
if ((Test-Path $Root -PathType Container) -and (Get-ChildItem -Force -Path $Root | Select-Object -First 1) -and -not $Force) {
    Stop-Setup "$Root exists and is not empty. Pass --force to scaffold into it anyway, or use a different --root."
}

$Source = Join-Path (Join-Path $AgentDocs 'recipes') $Modes[$Mode].Recipe
if (-not (Test-Path $Source -PathType Container)) { Stop-Setup "recipe not found: $Source" }

New-Item -ItemType Directory -Force -Path $Root | Out-Null
$SourceFull = (Resolve-Path $Source).ProviderPath.TrimEnd('\')
foreach ($file in Get-ChildItem -Recurse -File -Force -Path $SourceFull) {
    $target = Join-Path $Root $file.FullName.Substring($SourceFull.Length + 1)
    New-Item -ItemType Directory -Force -Path (Split-Path -Parent $target) | Out-Null
    Copy-Item -Force -Path $file.FullName -Destination $target
}

$NewLine = [Environment]::NewLine
$Tokens = New-Object System.Collections.ArrayList
foreach ($pair in $Modes[$Mode].Tokens) {
    $parts = $pair.Split('=', 2)
    [void]$Tokens.Add(@($parts[0], $parts[1].Replace('{name}', $Name)))
}
foreach ($cmake in @((Join-Path $Root 'CMakeLists.txt'), (Join-Path $Root 'src\CMakeLists.txt'))) {
    $text = Read-Lf $cmake
    foreach ($token in $Tokens) { $text = $text.Replace($token[0], $token[1]) }
    Write-Text $cmake ($text -replace "`n", $NewLine)
}

$Top = Join-Path $Root 'CMakeLists.txt'
$text = Read-Lf $Top
$old = "    FetchContent_Declare(areg`n        GIT_REPOSITORY https://github.com/aregtech/areg-sdk.git`n        GIT_TAG `"$DefaultTag`")`n    FetchContent_MakeAvailable(areg)"
if (-not $text.Contains($old)) { Stop-Setup 'recipe CMakeLists.txt does not carry the expected FetchContent block' }
if ($SdkRoot) {
    $new = "    FetchContent_Declare(areg SOURCE_DIR `"$SdkRoot`")`n    FetchContent_MakeAvailable(areg)"
} else {
    $new = "    FetchContent_Declare(areg`n        GIT_REPOSITORY https://github.com/aregtech/areg-sdk.git`n        GIT_TAG `"$Tag`")`n    FetchContent_MakeAvailable(areg)"
}
$text = $text.Replace($old, $new)
$include = 'include_directories("${CMAKE_CURRENT_SOURCE_DIR}")'
if (-not $text.Contains($include)) {
    $at = $text.IndexOf('add_subdirectory(src)')
    if ($at -ge 0) { $text = $text.Insert($at, $include + "`n") }
}
Write-Text $Top ($text -replace "`n", $NewLine)

Write-Text (Join-Path $Root '.gitignore') ("build/`nbuild-*/`nproduct/`n*.sqlog`n*.log`n" -replace "`n", $NewLine)

if ($Mode -ceq 'ipc') {
    $Provider = "${Name}_provider"
    $Consumer = "${Name}_consumer"

    $runSh = @'
#!/usr/bin/env bash
# Starts the router and the applications in the order they need.
set -eu
case "$(uname -s)" in Darwin) SFX=.mac ;; *) SFX=.elf ;; esac
BIN=./build/bin
# Every process this script starts in the background. The consumer runs
# in the foreground and ends on its own; the router and the provider do
# not, so a script that leaves either behind adds one orphan per run.
# An orphan holds this script's stdout open, and "./run.sh | tail" then
# never ends: the output looks like a hung application.
STARTED=""
stop() { [ -n "$STARTED" ] && kill $STARTED 2>/dev/null || true; }
trap stop EXIT INT TERM
# --service is the unattended mode; the console default draws a live
# status display that only makes sense on a terminal.
"$BIN/mtrouter$SFX" --service &
STARTED="$!"
# Wait for the router to accept a connection. Only one process can hold
# 8181, so this also catches one that was already running.
for _ in $(seq 1 100); do
    (exec 3<>/dev/tcp/127.0.0.1/8181) 2>/dev/null && break
    sleep 0.1
done
"$BIN/@PROVIDER@$SFX" &
STARTED="$STARTED $!"
sleep 1
"$BIN/@CONSUMER@$SFX"
'@
    $runBat = @'
@echo off
rem Starts the router and the applications in the order they need, and
rem stops the ones it started once the consumer ends.
setlocal
set BIN=build\bin
rem The router runs in console mode, in a window of its own: --service is
rem the Service Control Manager and returns at once from a command line.
for /f %%p in ('powershell -NoProfile -Command "(Start-Process -FilePath '%BIN%\mtrouter.exe' -PassThru).Id"') do set ROUTER=%%p
rem Waits for the router to listen on 8181.
for /l %%i in (1,1,20) do (
    netstat -an | findstr /r /c:":8181 .*LISTENING" >nul && goto ready
    timeout /t 1 /nobreak >nul
)
:ready
for /f %%p in ('powershell -NoProfile -Command "(Start-Process -FilePath '%BIN%\@PROVIDER@.exe' -PassThru).Id"') do set PROVIDER=%%p
timeout /t 1 /nobreak >nul
"%BIN%\@CONSUMER@.exe"
set RC=%errorlevel%
taskkill /pid %PROVIDER% /t /f >nul 2>&1
taskkill /pid %ROUTER% /t /f >nul 2>&1
exit /b %RC%
'@
    $runSh = ($runSh -replace "`r`n", "`n").Replace('@PROVIDER@', $Provider).Replace('@CONSUMER@', $Consumer)
    $runBat = ($runBat -replace "`r`n", "`n").Replace('@PROVIDER@', $Provider).Replace('@CONSUMER@', $Consumer)
    Write-Text (Join-Path $Root 'run.sh') ($runSh.TrimEnd("`n") + "`n")
    Write-Text (Join-Path $Root 'run.bat') (($runBat.TrimEnd("`n") + "`n") -replace "`n", "`r`n")
}

Write-Output "created $Root ($Mode mode)"
Write-Output '  Build it:'
Write-Output "    cd $Root"
Write-Output '    cmake -B build'
Write-Output '    cmake --build build -j'
if ($Mode -ceq 'ipc') {
    Write-Output '  Run it: run.bat starts mtrouter, the provider and the consumer in order.'
} else {
    Write-Output "  Run it: $Name in build\bin\."
}
Write-Output '  Building with an AI coding agent? Install Python 3 and create the project with'
Write-Output '  tools\agent\setup_project.py instead: it also writes AGENTS.md and design.json,'
Write-Output '  and the agent tools generate, check and test the code for the agent.'
exit 0

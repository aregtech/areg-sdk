# One cold agent run, measured, against a clean snapshot of this checkout.
#
# The Windows version of run-benchmark.sh: the same options, the same run directory
# and the same isolation guards. Runs on Windows PowerShell 5.1 and PowerShell 7.
#
#   .\run-benchmark.ps1 --help

$ErrorActionPreference = 'Continue'

$HERE = $PSScriptRoot
$SDK  = ''
$Utf8 = New-Object System.Text.UTF8Encoding $false

$USAGE = @'
One cold agent run, measured, against a clean snapshot of this checkout.

  run-benchmark.ps1 [label] [options]

  label              suffix: e -> .\<date>e-<project>, made in the current directory.
                     Optional; the next unused letter for today is chosen.

  --framework NAME   areg | grpc                          (default: areg)
  --agent NAME       claude | copilot | codex | gemini    (default: claude)
  --task PATH        task file, absolute or relative to the SDK
                       (default: examples/ai-benchmark/prompt-coffeemachine.md)
  --wrapper PATH     the wrapper the prompt is built from, absolute or relative to
                     the SDK. Everything after its "--- PROMPT BEGINS BELOW THIS
                     LINE" marker is the prompt, with <areg-sdk>, <runner>, <task>,
                     <project> and <mode> substituted
                       (default: <framework>-<key>-prompt.txt beside a task named
                        prompt-<key>.md; without one, areg uses
                        areg-ai-prompt-template.txt and grpc refuses to start)
  --project NAME     C identifier: directory and CMake project name
                       (default: <key> of the task, e.g. coffeemachine)
  --mode MODE        ipc | local | pubsub, areg only      (default: ipc)
  --model NAME       model ID or alias accepted by the selected CLI; not a whitelist
                       (default: sonnet for Claude; the CLI default otherwise)
  --effort LEVEL     low | medium | high, one vocabulary for every CLI: it becomes
                     Claude's --effort, Copilot's --reasoning-effort or Codex's
                     model_reasoning_effort. Gemini has no such setting and refuses it
                       (default: medium for Claude; the CLI default otherwise)
  --attempts N       the build-and-fix and run-and-fix bound (default: 15, which is
                     what every published run used; the runbook's own bound is 3).
                     Any number other than the runbook's adds one rule to the prompt,
                     the same for both arms. 0 removes the bound, and is warned about:
                     the spend is unbounded.
  --debrief          append a diagnostic pass: what the run could not find. It costs
                     requests on purpose, so such a run is never compared with one
                     made without it.
  --recipes MODE     none | copy, areg only               (default: none)
                       none: no example source may be copied; every file is written
                       or generated. copy: a documented recipe may be copied.
  --web MODE         on | off: whether the agent may search and fetch pages
                       (default: off for areg, on for grpc)
                     areg ships its documentation in the snapshot, and that snapshot
                     is what a run measures; an arm that may browse can read the
                     published copy instead, and no manifest can tell. gRPC ships its
                     documentation on the web, and its prompt says so, so an arm
                     denied the web has no documentation at all and the comparison
                     measures recall rather than the framework.
  --verify MODE      none | probes | sanitize             (default: probes)
                       the hidden acceptance probes of verify_run.py, run on the
                       project after the agent ends. sanitize adds a rebuild under
                       ASan and UBSan.
  --sdk DIR          the areg-sdk checkout: the directory that holds AGENTS.md
                       (default: $env:AREG_SDK_ROOT, else the checkout this script
                        sits in; asked for when neither answers)
  --grpc DIR         the directory holding protoc and grpc_cpp_plugin, or the prefix
                     they are installed under, whose headers and libraries CMake is
                     then told about. gRPC arm only
                       (default: $env:AREG_GRPC_ROOT, else the PATH; asked for when
                        neither answers)
  --dry-run          stage the run directory and print the prompt, start nothing
  --allow-installed-areg
                     proceed although find_package(areg) finds an installed package,
                     which would shadow the snapshot
  -h, --help         this text

  Examples:
    .\run-benchmark.ps1                                 areg, the coffee machine
    .\run-benchmark.ps1 --framework grpc --attempts 15  gRPC, the coffee machine
    .\run-benchmark.ps1 --task examples/ai-benchmark/prompt-tempalarm.md
    .\run-benchmark.ps1 --task examples/ai-benchmark/prompt-atm.md --attempts 15
    .\run-benchmark.ps1 --task examples/ai-benchmark/prompt-printscan.md --attempts 15
    .\run-benchmark.ps1 f --model opus --effort high --dry-run
    .\run-benchmark.ps1 --sdk C:\src\areg-sdk --framework grpc --grpc C:\grpc
    .\run-benchmark.ps1 --agent copilot --model gpt-5.6-terra
    .\run-benchmark.ps1 --agent codex --model gpt-5.4 --effort high
    .\run-benchmark.ps1 --agent gemini --model gemini-2.5-flash
    .\run-benchmark.ps1 --verify sanitize               areg, probes and sanitizers

  The run directory is made in the current directory, so start the run where the
  measurements should land, and never inside the checkout. It holds meta.txt,
  prompt.txt, result.json[l], run.err, the fingerprints and sdk/, the snapshot the
  agent read. The agent works in work/, which starts empty, and is given sdk/ and
  nothing else: never the run directory, which names the source checkout.

  Install and authenticate only the selected CLI. Model availability depends on
  that CLI and your account; README.md explains model names and comparable metrics.
'@

# The snapshot, the fingerprint manifest and its check, shared with run-benchmark.sh.
$HELPER = @'
import glob, hashlib, os, shutil, subprocess, sys


def md5(path):
    digest = hashlib.md5()
    with open(path, 'rb') as handle:
        for block in iter(lambda: handle.read(1 << 20), b''):
            digest.update(block)
    return digest.hexdigest()


def snapshot(source, target):
    names = subprocess.run(['git', '-C', source, 'ls-files', '-z', '--cached', '--others',
                            '--exclude-standard'], stdout=subprocess.PIPE, check=True).stdout
    copied = 0
    for name in names.split(b'\0'):
        relative = name.decode('utf-8')
        path = os.path.join(source, relative)
        if (not relative or not os.path.isfile(path)
                or relative == 'examples/ai-benchmark/verify_run.py'):
            continue
        destination = os.path.join(target, relative)
        os.makedirs(os.path.dirname(destination), exist_ok=True)
        shutil.copy2(path, destination)
        copied += 1
    print(copied)


def manifest(root, output, patterns):
    lines = []
    for pattern in patterns:
        for path in sorted(glob.glob(os.path.join(root, pattern))):
            relative = os.path.relpath(path, root).replace(os.sep, '/')
            lines.append('{}  {}\n'.format(md5(path), relative))
    with open(output, 'w', newline='\n') as handle:
        handle.write(''.join(lines))


def check(root, listing):
    changed = []
    with open(listing) as handle:
        for line in handle:
            digest, relative = line.rstrip('\n').split('  ', 1)
            path = os.path.join(root, relative)
            if not os.path.isfile(path) or md5(path) != digest:
                changed.append(relative + ': FAILED')
    print('\n'.join(changed))


command = sys.argv[1]
if command == 'snapshot':
    snapshot(sys.argv[2], sys.argv[3])
elif command == 'manifest':
    manifest(sys.argv[2], sys.argv[3], sys.argv[4:])
elif command == 'check':
    check(sys.argv[2], sys.argv[3])
'@

# A path is never searched for. It is given, inherited, found where this script sits,
# or asked for: a guessed path measures a tree nobody chose.
function Read-Path([string]$What)
{
    if ([Console]::IsInputRedirected) {
        Stop-Run ("$What`n  There is no terminal to ask on. Pass the option.")
    }
    [Console]::Error.Write("run-benchmark: $What`n> ")
    $answer = [Console]::ReadLine()
    if (-not $answer) { Stop-Run 'nothing given; pass the option' }
    return $answer.Trim('"').Trim()
}

# The SDK is the directory that holds AGENTS.md, and that is what is checked.
function Resolve-Sdk([string]$Given)
{
    $candidate = ''
    if ($Given) { $candidate = $Given }
    elseif ($env:AREG_SDK_ROOT) { $candidate = $env:AREG_SDK_ROOT }
    elseif (Test-Path -LiteralPath (Join-Path $HERE '..\..\AGENTS.md') -PathType Leaf) {
        $candidate = Join-Path $HERE '..\..'
    }
    else {
        $candidate = Read-Path 'the areg-sdk checkout is unknown: give the directory that holds AGENTS.md, or pass --sdk'
    }
    if (-not (Test-Path -LiteralPath $candidate -PathType Container)) { Stop-Run "--sdk $candidate is not a directory" }
    $full = [IO.Path]::GetFullPath($candidate).TrimEnd('\')
    if (-not (Test-Path -LiteralPath (Join-Path $full 'AGENTS.md') -PathType Leaf)) {
        Stop-Run "$full holds no AGENTS.md, so it is not an areg-sdk checkout; pass --sdk"
    }
    return $full
}

# protoc and grpc_cpp_plugin, and the prefix its headers and libraries are under.
function Resolve-Grpc([string]$Given)
{
    $candidate = ''
    if ($Given) { $candidate = $Given }
    elseif ($env:AREG_GRPC_ROOT) { $candidate = $env:AREG_GRPC_ROOT }
    elseif ((Get-Command protoc -ErrorAction SilentlyContinue) -and
            (Get-Command grpc_cpp_plugin -ErrorAction SilentlyContinue)) { return }
    else {
        $candidate = Read-Path 'protoc and grpc_cpp_plugin are not on the PATH: give the directory that holds them (or the install prefix), or pass --grpc'
    }
    if (-not (Test-Path -LiteralPath $candidate -PathType Container)) { Stop-Run "--grpc $candidate is not a directory" }
    $full = [IO.Path]::GetFullPath($candidate).TrimEnd('\')
    $bin = $full
    if (-not (Test-Path -LiteralPath (Join-Path $bin 'protoc.exe')) -and
        -not (Test-Path -LiteralPath (Join-Path $bin 'protoc'))) { $bin = Join-Path $full 'bin' }
    if (-not (Test-Path -LiteralPath (Join-Path $bin 'protoc.exe')) -and
        -not (Test-Path -LiteralPath (Join-Path $bin 'protoc'))) {
        Stop-Run "no protoc in $full or $full\bin; see $HERE\INSTALL-grpc.md"
    }
    $env:PATH = "$bin;$env:PATH"
    # A prefix carries the headers and libraries too, so CMake is told where to look
    # rather than left to find whichever copy the machine happens to have first.
    if ($bin -ne $full) {
        $env:CMAKE_PREFIX_PATH = if ($env:CMAKE_PREFIX_PATH) { "$full;$env:CMAKE_PREFIX_PATH" } else { $full }
    }
}

# One vocabulary for every CLI: low, medium, high. Each spells it differently, and the
# spelling lives here so a comparison is never quietly reading three different scales.
function Get-EffortArgs([string]$Which, [string]$Level)
{
    if (-not $Level) { return @() }
    switch ($Which) {
        'claude'  { return @('--effort', $Level) }
        'copilot' { return @('--reasoning-effort', $Level) }
        'codex'   { return @('--config', "model_reasoning_effort='$Level'") }
    }
    return @()
}

function Stop-Run([string]$Message)
{
    [Console]::Error.WriteLine("run-benchmark: $Message")
    exit 2
}

function Write-Text([string]$Path, [string]$Text)
{
    [IO.File]::WriteAllText($Path, $Text, $Utf8)
}

function Add-Text([string]$Path, [string]$Text)
{
    [IO.File]::AppendAllText($Path, $Text, $Utf8)
}

function Get-Utc([string]$Format)
{
    return [DateTime]::UtcNow.ToString($Format)
}

function To-Slash([string]$Path)
{
    return $Path -replace '\\', '/'
}

# A path inside the checkout is read relative to it.
function Resolve-InSdk([string]$Path)
{
    if ([IO.Path]::IsPathRooted($Path)) { return [IO.Path]::GetFullPath($Path) }
    return [IO.Path]::GetFullPath((Join-Path $SDK $Path))
}

# python3, python or the py launcher, skipping the Microsoft Store stub.
function Find-Python
{
    foreach ($name in 'python3', 'python') {
        $found = Get-Command $name -CommandType Application -ErrorAction SilentlyContinue |
                 Where-Object { $_.Source -notlike '*\WindowsApps\*' } | Select-Object -First 1
        if ($found) { return @($found.Source) }
    }
    $launcher = Get-Command py -CommandType Application -ErrorAction SilentlyContinue |
                Select-Object -First 1
    if ($launcher) { return @($launcher.Source, '-3') }
}

function Invoke-Python
{
    $prefix = @($script:Python | Select-Object -Skip 1)
    $global:LASTEXITCODE = -1
    $output = & $script:Python[0] @prefix @args
    $script:PythonCode = $LASTEXITCODE
    return ($output -join "`n")
}

function Invoke-Helper
{
    $output = Invoke-Python $script:HelperPath @args
    if ($script:PythonCode -ne 0) { Stop-Run "the snapshot helper failed: $($args[0])" }
    return $output
}

# The path of an installed areg package find_package would take, or an empty string.
function Find-InstalledAreg
{
    $probe = Join-Path ([IO.Path]::GetTempPath()) ('areg-probe-' + [Guid]::NewGuid().ToString('N'))
    New-Item -ItemType Directory -Path $probe | Out-Null
    Write-Text (Join-Path $probe 'CMakeLists.txt') @'
cmake_minimum_required(VERSION 3.20)
project(probe NONE)
find_package(areg CONFIG QUIET)
if (areg_FOUND)
    message(NOTICE "AREG_INSTALLED_AT ${areg_DIR}")
endif()
'@
    $output = (& cmake -S $probe -B (Join-Path $probe 'b') 2>&1 | ForEach-Object { "$_" }) -join "`n"
    Remove-Item -Recurse -Force $probe -ErrorAction SilentlyContinue
    foreach ($line in ($output -split "`n")) {
        if ($line -match 'AREG_INSTALLED_AT (.*)$') { return $Matches[1].Trim() }
    }
    return ''
}

function Main([string[]]$Arguments)
{
    $Framework = 'areg'; $Task = 'examples/ai-benchmark/prompt-coffeemachine.md'; $Wrapper = ''
    $Project = ''; $Mode = 'ipc'; $Agent = 'claude'; $Model = ''; $Effort = ''
    $Attempts = '15'; $Debrief = $false; $Recipes = 'none'; $Label = ''; $Dry = $false
    $AllowInstalled = $false; $Verify = 'probes'
    $SdkOpt = ''; $GrpcOpt = ''; $Web = ''

    # A bare first word is the label. Anything starting with a dash is an option.
    $index = 0
    if ($Arguments.Count -gt 0) {
        if ($Arguments[0] -in '-h', '--help') { Write-Output $USAGE; exit 0 }
        if (-not $Arguments[0].StartsWith('-')) { $Label = $Arguments[0]; $index = 1 }
    }

    while ($index -lt $Arguments.Count) {
        $option = $Arguments[$index]
        $valued = '--framework', '--agent', '--task', '--wrapper', '--project', '--mode', '--model',
                  '--effort', '--attempts', '--recipes', '--sdk', '--grpc', '--verify', '--web'
        if ($option -cin $valued) {
            if ($index + 1 -ge $Arguments.Count) { Stop-Run "$option needs a value" }
            $value = $Arguments[$index + 1]
            switch -casesensitive ($option) {
                '--framework' { $Framework = $value }
                '--agent'     { $Agent = $value }
                '--task'      { $Task = $value }
                '--wrapper'   { $Wrapper = $value }
                '--project'   { $Project = $value }
                '--mode'      { $Mode = $value }
                '--model'     {
                    if (-not $value) { Stop-Run '--model needs a non-empty value' }
                    $Model = $value
                }
                '--effort'    { $Effort = $value }
                '--attempts'  { $Attempts = $value }
                '--recipes'   { $Recipes = $value }
                '--sdk'       { $SdkOpt = $value }
                '--grpc'      { $GrpcOpt = $value }
                '--verify'    { $Verify = $value }
                '--web'       { $Web = $value }
            }
            $index += 2
            continue
        }
        switch -casesensitive ($option) {
            '-h'                     { Write-Output $USAGE; exit 0 }
            '--help'                 { Write-Output $USAGE; exit 0 }
            '--debrief'              { $Debrief = $true }
            '--dry-run'              { $Dry = $true }
            '--allow-installed-areg' { $AllowInstalled = $true }
            default                  { Stop-Run "unknown option: $option (run with --help)" }
        }
        $index += 1
    }

    # The key of prompt-<key>.md names the project and the wrapper.
    $Key = Split-Path $Task -Leaf
    if ($Key.EndsWith('.md')) { $Key = $Key.Substring(0, $Key.Length - 3) }
    if ($Key.StartsWith('prompt-')) { $Key = $Key.Substring(7) }
    if (-not $Project) { $Project = $Key -replace '[^A-Za-z0-9_]', '' }

    if ($Framework -cnotin 'areg', 'grpc') { Stop-Run "--framework must be areg or grpc, not '$Framework'" }
    if ($Mode -cnotin 'ipc', 'local', 'pubsub') { Stop-Run "--mode must be ipc, local or pubsub, not '$Mode'" }
    if ($Agent -cnotin 'claude', 'copilot', 'codex', 'gemini') { Stop-Run "--agent must be claude, copilot, codex or gemini, not '$Agent'" }
    if ($Effort -cnotin '', 'low', 'medium', 'high') { Stop-Run "--effort must be low, medium or high, not '$Effort'" }
    if ($Agent -eq 'claude') {
        if (-not $Model) { $Model = 'sonnet' }
        if (-not $Effort) { $Effort = 'medium' }
    }
    elseif ($Agent -eq 'gemini' -and $Effort) { Stop-Run '--effort is not supported by Gemini CLI; omit it' }
    if ($Recipes -cnotin 'none', 'copy') { Stop-Run "--recipes must be none or copy, not '$Recipes'" }
    if ($Verify -cnotin 'none', 'probes', 'sanitize') { Stop-Run "--verify must be none, probes or sanitize, not '$Verify'" }
    if ($Web -cnotin '', 'on', 'off') { Stop-Run "--web must be on or off, not '$Web'" }
    # Each arm gets its own framework's documentation, in the place that documentation
    # lives: the snapshot for areg, the web for gRPC.
    if (-not $Web) { $Web = if ($Framework -eq 'grpc') { 'on' } else { 'off' } }
    if ($Attempts -notmatch '^[0-9]+$') { Stop-Run "--attempts must be a whole number, not '$Attempts'" }
    if ($Project -cnotmatch '^[A-Za-z_][A-Za-z0-9_]*$') { Stop-Run "--project must be a C identifier, not '$Project'" }

    if (-not (Get-Command $Agent -ErrorAction SilentlyContinue)) { Stop-Run "$Agent not found: install the selected CLI and log in" }
    $script:Python = @(Find-Python)
    if ($script:Python.Count -eq 0) { Stop-Run 'python3 not found' }
    $script:SDK = Resolve-Sdk $SdkOpt
    $SDK = $script:SDK
    & git -C $SDK rev-parse HEAD 2>&1 | Out-Null
    if ($LASTEXITCODE -ne 0) { Stop-Run "$SDK is not a git checkout; the snapshot is taken from its file list" }

    $TaskAbs = Resolve-InSdk $Task
    if (-not (Test-Path -LiteralPath $TaskAbs -PathType Leaf)) { Stop-Run "no such task file: $TaskAbs" }

    if (-not $Wrapper) {
        $Wrapper = Join-Path (Split-Path $TaskAbs -Parent) "$Framework-$Key-prompt.txt"
        if (-not (Test-Path -LiteralPath $Wrapper -PathType Leaf)) {
            if ($Framework -ne 'areg') { Stop-Run "no $Framework wrapper for this task: $Wrapper. Write one, or pass --wrapper" }
            $Wrapper = Join-Path $HERE 'areg-ai-prompt-template.txt'
        }
    }
    $WrapperAbs = Resolve-InSdk $Wrapper
    if (-not (Test-Path -LiteralPath $WrapperAbs -PathType Leaf)) { Stop-Run "no such wrapper file: $WrapperAbs" }

    if ($Framework -eq 'grpc') {
        # The toolchain is the operator's job; an agent that installs one measures it.
        Resolve-Grpc $GrpcOpt
        if (-not (Get-Command protoc -ErrorAction SilentlyContinue)) { Stop-Run "protoc not found; see $HERE\INSTALL-grpc.md" }
        if (-not (Get-Command grpc_cpp_plugin -ErrorAction SilentlyContinue)) { Stop-Run "grpc_cpp_plugin not found; see $HERE\INSTALL-grpc.md" }
    }
    elseif (-not $AllowInstalled) {
        $foundAreg = Find-InstalledAreg
        if ($foundAreg) {
            Stop-Run ("find_package(areg) resolves to an installed package at $foundAreg.`n" +
                      "  The scaffolded project calls find_package before FetchContent, so the run would`n" +
                      "  build against that package instead of the snapshot. Uninstall it, or pass`n" +
                      "  --allow-installed-areg to measure it deliberately.")
        }
    }

    if ($Attempts -eq '0') {
        [Console]::Error.WriteLine('run-benchmark: WARNING --attempts 0 removes the fix bound; the spend is unbounded')
    }

    $suffix = if ($Framework -eq 'grpc') { "grpc-$Project" } else { $Project }

    # The run is made where it is started, so no path is assumed and no home is
    # searched. The one rule that remains is the old one.
    $HostDir = (Get-Location -PSProvider FileSystem).ProviderPath.TrimEnd('\')
    $sdkPrefix = $SDK.TrimEnd('\') + '\'
    if (($HostDir + '\').StartsWith($sdkPrefix, [StringComparison]::OrdinalIgnoreCase)) {
        Stop-Run "the current directory $HostDir is inside the checkout; a run must not write where it reads. Start it somewhere else."
    }

    $today = Get-Utc 'yyyyMMdd'
    if (-not $Label) {
        foreach ($letter in [char[]]'abcdefghijklmnopqrstuvwxyz') {
            if (-not (Test-Path -LiteralPath (Join-Path $HostDir "$today$letter-$suffix"))) { $Label = "$letter"; break }
        }
        if (-not $Label) { Stop-Run "every label a..z is used for $suffix today; pass one explicitly" }
    }

    $Run = Join-Path $HostDir "$today$Label-$suffix"
    if (Test-Path -LiteralPath $Run) { Stop-Run "run directory already exists: $Run" }
    $Work = Join-Path $Run 'work'
    $Snap = Join-Path $Run 'sdk'
    New-Item -ItemType Directory -Force -Path $Work | Out-Null

    $script:HelperPath = Join-Path ([IO.Path]::GetTempPath()) ("run-benchmark-helper-$PID.py")
    Write-Text $script:HelperPath $HELPER

    # What was measured: the revision, the uncommitted state, and the copy itself.
    $head = ((& git -C $SDK rev-parse HEAD) -join '').Trim()
    Write-Text (Join-Path $Run 'sdk-head.txt') "$head`n"
    $status = @(& git -C $SDK status --porcelain)
    Write-Text (Join-Path $Run 'sdk-before.txt') ($(if ($status.Count) { ($status -join "`n") + "`n" } else { '' }))

    $manifestPath = Join-Path $Run 'sdk-md5.txt'
    if ($Framework -eq 'grpc') {
        # Only the scenario runner and task are staged: the areg corpus must not exist inside the run.
        New-Item -ItemType Directory -Force -Path (Join-Path $Snap 'tools\agent') | Out-Null
        Copy-Item -LiteralPath (Join-Path $SDK 'tools\agent\run_scenarios.py') -Destination (Join-Path $Snap 'tools\agent\run_scenarios.py')
        Copy-Item -LiteralPath $TaskAbs -Destination (Join-Path $Snap 'task.md')
        $copied = '2'
        Invoke-Helper manifest $Snap $manifestPath 'tools/agent/run_scenarios.py' 'task.md' | Out-Null
        $allowed = @(
            [IO.Path]::GetFullPath((Join-Path $Snap 'tools\agent\run_scenarios.py')),
            [IO.Path]::GetFullPath((Join-Path $Snap 'task.md'))
        )
        $stray = @(Get-ChildItem -LiteralPath $Snap -Recurse -File |
                   Where-Object { [IO.Path]::GetFullPath($_.FullName) -notin $allowed } |
                   Select-Object -First 5 | ForEach-Object { $_.FullName })
        if ($stray.Count) { Stop-Run ("the gRPC arm staged more than the scenario runner and task:`n" + ($stray -join "`n")) }
        if (Select-String -LiteralPath (Join-Path $Snap 'task.md') -Pattern 'areg' -Quiet) {
            Stop-Run "$TaskAbs names areg; the gRPC arm must not be told of it"
        }
    }
    else {
        $copied = (Invoke-Helper snapshot $SDK $Snap).Trim()
        Invoke-Helper manifest $Snap $manifestPath 'AGENTS.md' 'docs/agent/*.md' 'docs/agent/*.json' `
            'docs/agent/.budgets' 'tools/agent/*.py' 'tools/agent/evals/tasks.json' `
            'conf/cmake/functions.cmake' 'examples/ai-benchmark/*.md' 'examples/ai-benchmark/*.txt' | Out-Null
    }

    $SnapF = To-Slash $Snap

    # A task inside the checkout is read from the snapshot; one outside it as given.
    $TaskRun = To-Slash $TaskAbs
    if ($TaskAbs.StartsWith($sdkPrefix, [StringComparison]::OrdinalIgnoreCase)) {
        $TaskRun = $SnapF + '/' + (To-Slash $TaskAbs.Substring($sdkPrefix.Length))
    }

    $AddDir = $Snap
    $Rules = ''
    if ($Framework -eq 'grpc') {
        # The task and runner share the isolated directory granted to the agent.
        $TaskRun = To-Slash (Join-Path $Snap 'task.md')
        $Rules = @"
- **You have gRPC and its public documentation. You do not have example source.**
  No .cpp, .hpp or .proto arrives by copy: every file in the project is written by
  you or generated by protoc.
"@
        $toolchain = @()
        $toolchain += (& protoc --version 2>&1 | ForEach-Object { "$_" })
        $toolchain += (& grpc_cpp_plugin --version 2>&1 | ForEach-Object { "$_" } | Select-Object -First 1)
        if (Get-Command pkg-config -ErrorAction SilentlyContinue) {
            $toolchain += (& pkg-config --modversion grpc++ 2>&1 | ForEach-Object { "$_" })
        } else {
            $toolchain += 'grpc++ pkg-config: none'
        }
        $toolchain += (& cmake --version 2>&1 | ForEach-Object { "$_" } | Select-Object -First 1)
        Write-Text (Join-Path $Run 'toolchain.txt') (($toolchain -join "`n") + "`n")
    }
    elseif ($Recipes -eq 'none') {
        $Rules = @"
- **You have the tools and the documentation. You do not have example source.**
  Treat $SnapF/docs/agent/recipes/ and the applications under $SnapF/examples/ as
  absent: do not open a file in them and do not copy one out of them. The files your
  prompt names are the exception. Where AGENTS.md offers a recipe to copy, read the
  page it names instead. Code shown inside a documentation page is there to be read
  and understood, never to be copied in as a starting point.

- **Every file in the project is written by you or generated by a tool in
  $SnapF/tools/agent/.** No .cpp, .hpp, .siml or .fsml arrives by copy.
"@
    }
    else {
        $Rules = @"
- **Start from working code where AGENTS.md offers it.** A recipe under
  $SnapF/docs/agent/recipes/ may be copied into the project and adapted. Copy it,
  rename it, and make it yours; never leave a file that still builds the recipe's
  application. The applications under $SnapF/examples/ are not for copying.
"@
    }

    if ($Attempts -eq '0') {
        $Rules += @"


- **This task has no fix bound.** Wherever the runbook or this prompt says at most 3
  build-and-fix or run-and-fix cycles, there is no limit. Fix the cause, never the
  symptom, and never loosen what a scenario expects.
"@
    }
    elseif ($Attempts -ne '3') {
        $Rules += @"


- **The maximum fix bound for this task is $Attempts.** Wherever the runbook or this
  prompt says at most 3 build-and-fix cycles or at most 3 run-and-fix cycles, read
  $Attempts. Everything else about the bound is unchanged: fix the cause, never the
  symptom, and never loosen what a scenario expects.
"@
    }

    if ($Debrief) {
        $Rules += @"


Additionally, for this run only -- a diagnostic pass the normal task does not ask
for. Do it last, after the report, and never let it change what you built:

- **Every page you opened, in order, with the request you opened it at and why.**
  Name the row of a routing table that sent you, or say that nothing did.
- **Every question you answered from your own training rather than from a page**,
  and what you would have needed to read to answer it from the documentation.
- **Every place two sources said different things**, naming both, and which one
  you followed.
- **Every marker whose one-line hint was not enough**, and what it should have said.
- **Anything you looked for and could not find** -- a signature, a rule, an example
  -- and where you looked first.
- **Every file under the project's src/ or build/ you opened or searched**, with
  the request, the question it was meant to answer, and whether worksheet.txt
  already answered it.
- **Every page you opened before the first build_project.py call**: what in the
  task or in AGENTS.md made you open it then, rather than after generation.
- **Every tool you ran to learn a syntax, a name or a signature**, and whether its
  answer was enough or you had to look again elsewhere.

Be specific and short: a list, not prose.
"@
    }
    $Rules = $Rules -replace "`r`n", "`n"

    # The prompt is the wrapper a user pastes, with its values substituted.
    $lines = ([IO.File]::ReadAllText($WrapperAbs) -replace "`r`n", "`n") -split "`n"
    $marker = -1
    for ($n = 0; $n -lt $lines.Count; $n++) {
        if ($lines[$n].StartsWith('--- PROMPT BEGINS BELOW THIS LINE')) { $marker = $n; break }
    }
    $body = ''
    if ($marker -ge 0 -and $marker + 1 -lt $lines.Count) {
        $body = (($lines[($marker + 1)..($lines.Count - 1)]) -join "`n") -replace '^\n+', ''
        $body = $body.TrimEnd("`n")
    }
    if (-not $body) { Stop-Run "$WrapperAbs has no '--- PROMPT BEGINS BELOW THIS LINE' marker" }
    $body = $body.Replace('<areg-sdk>', $SnapF).Replace('<runner>', "$SnapF/tools/agent/run_scenarios.py")
    $body = $body.Replace('<task>', $TaskRun).Replace('<project>', $Project).Replace('<mode>', $Mode)
    $promptPath = Join-Path $Run 'prompt.txt'
    Write-Text $promptPath ("$body`n`n$Rules`n")
    if ($Framework -eq 'grpc' -and (Select-String -LiteralPath $promptPath -Pattern 'areg' -Quiet)) {
        Stop-Run "the gRPC prompt names areg; check $WrapperAbs, and that $HostDir does not"
    }

    $meta = Join-Path $Run 'meta.txt'
    $modelLabel = if ($Model) { $Model } else { 'agent-default' }
    $effortLabel = if ($Effort) { $Effort } else { 'agent-default' }
    $isolation = if ($Agent -eq 'claude') { 'snapshot, no skills, no MCP servers' } else {
        'snapshot; CLI user configuration may load (see README.md)'
    }
    # Only claude and copilot are told whether they may browse. What codex and gemini
    # reach is their own sandbox's business, so the line records the intent, not a
    # guarantee, and says which it is.
    if ($Agent -cnotin 'claude', 'copilot') { $isolation = "$isolation; web $Web is not enforced for $Agent" }
    Write-Text $meta (@(
        "framework $Framework", "agent    $Agent", "model    $modelLabel", "effort   $effortLabel", "task     $TaskRun",
        "mode     $Mode", "recipes  $Recipes", "attempts $Attempts",
        "debrief  $(if ($Debrief) { '1' } else { 'no' })",
        "source   $SDK", "sdk      $Snap", "files    $copied", "head     $head",
        "web      $Web", "isolation $isolation",
        "start    $(Get-Utc 'yyyy-MM-ddTHH:mm:ssZ')") -join "`n") + "`n"

    if ($Dry) {
        Write-Output "staged $Run ($copied files in the snapshot)"
        Write-Output '--- prompt.txt ---'
        Write-Output ([IO.File]::ReadAllText($promptPath))
        Remove-Item -LiteralPath $script:HelperPath -ErrorAction SilentlyContinue
        exit 0
    }

    Write-Output "run-benchmark: $Run"
    Write-Output "run-benchmark: $Framework, $Agent, $modelLabel, effort $effortLabel, attempts $Attempts, head $($head.Substring(0, 8)), $copied files"

    $result = Join-Path $Run 'result.json'
    $agentArgs = @()
    # AddDir, never Run: Run is the parent of the snapshot and holds meta.txt, which
    # names the source checkout. Handing that to the gRPC arm tells it what it exists
    # not to know, and handing it to any arm shows one agent what the others cannot see.
    $tools = if ($Web -eq 'off') { 'Bash Read Write Edit Glob Grep' } else { 'Bash Read Write Edit Glob Grep WebSearch WebFetch' }
    switch ($Agent) {
        'claude' {
            $agentArgs = @('-p', '--output-format', 'json',
                          '--disable-slash-commands', '--strict-mcp-config',
                          '--allowedTools', $tools, '--add-dir', $AddDir)
        }
        'copilot' {
            $agentArgs = @('--allow-all-tools', '--disable-builtin-mcps', '--no-custom-instructions',
                          '--no-ask-user',
                          '--add-dir', $AddDir, '--usage-output-file', (Join-Path $Run 'result.json'))
            if ($Web -eq 'off') {
                # Named out rather than allow-listed: the CLI accepts a tool name it
                # does not have, so an allow-list with one typo would disarm the agent
                # halfway through a paid run, while an exclusion that misses is inert.
                $agentArgs += '--excluded-tools=web_search,web_fetch,fetch'
            } else {
                $agentArgs += '--allow-all-urls'
            }
            $result = Join-Path $Run 'run.out'
        }
        'codex' {
            $agentArgs = @('--ask-for-approval', 'never', 'exec', '--sandbox', 'workspace-write',
                          '--skip-git-repo-check', '--add-dir', $AddDir, '--json')
            $result = Join-Path $Run 'result.jsonl'
        }
        'gemini' {
            $agentArgs = @('--output-format', 'json', '--approval-mode', 'yolo', '--include-directories', $AddDir)
        }
    }
    $agentArgs += @(Get-EffortArgs $Agent $Effort)
    if ($Model) { $agentArgs += @('--model', $Model) }
    if ($Agent -eq 'codex') { $agentArgs += '-' }

    # PowerShell invokes native CLIs and npm shims alike. Stream UTF-8, including on 5.1.
    $oldEncoding = $OutputEncoding; $oldConsoleEncoding = [Console]::OutputEncoding
    $oldDefault = $env:BASH_DEFAULT_TIMEOUT_MS; $oldMax = $env:BASH_MAX_TIMEOUT_MS
    $writer = [IO.StreamWriter]::new($result, $false, $Utf8)
    Push-Location $Work
    try {
        $OutputEncoding = $Utf8; [Console]::OutputEncoding = $Utf8
        $env:BASH_DEFAULT_TIMEOUT_MS = '600000'; $env:BASH_MAX_TIMEOUT_MS = '900000'
        $global:LASTEXITCODE = -1
        [IO.File]::ReadAllText($promptPath) | & $Agent @agentArgs 2> (Join-Path $Run 'run.err') |
            ForEach-Object { $writer.WriteLine($_) }
        $code = $LASTEXITCODE
    }
    finally {
        Pop-Location
        $writer.Dispose()
        $OutputEncoding = $oldEncoding; [Console]::OutputEncoding = $oldConsoleEncoding
        $env:BASH_DEFAULT_TIMEOUT_MS = $oldDefault; $env:BASH_MAX_TIMEOUT_MS = $oldMax
    }
    Add-Text $meta ("end      $(Get-Utc 'yyyy-MM-ddTHH:mm:ssZ')`nexit     $code`n")

    Write-Output ''
    $cache = Join-Path $Work 'build\CMakeCache.txt'
    if ($Framework -eq 'areg' -and (Test-Path -LiteralPath $cache)) {
        $lists = Join-Path $Work 'CMakeLists.txt'
        $notFound = Select-String -LiteralPath $cache -SimpleMatch 'areg_DIR:PATH=areg_DIR-NOTFOUND' -Quiet
        $fromSnap = (Test-Path -LiteralPath $lists) -and
                    ((Select-String -LiteralPath $lists -SimpleMatch $SnapF -Quiet) -or
                     (Select-String -LiteralPath $lists -SimpleMatch $Snap -Quiet))
        if ($notFound -and $fromSnap) {
            Write-Output "sources: $Snap, verified (no installed areg was used)"
        } else {
            [Console]::Error.WriteLine("sources: NOT VERIFIED as $Snap; check $cache for areg_DIR")
        }
    }

    # A run that edited the corpus it is measured against is caught rather than scored.
    if ((Test-Path -LiteralPath $manifestPath) -and (Get-Item -LiteralPath $manifestPath).Length -gt 0) {
        $changed = (Invoke-Helper check $Snap $manifestPath).Trim()
        if ($changed) {
            [Console]::Error.WriteLine('corpus:  CHANGED DURING THE RUN -- this measurement is not valid')
            [Console]::Error.WriteLine($changed)
            Add-Text $meta "corpus   changed during the run`n"
            if ($code -eq 0) { $code = 3 }
        } else {
            $count = @(Get-Content -LiteralPath $manifestPath).Count
            Write-Output "corpus:  unchanged, $count file(s) verified"
            Add-Text $meta "corpus   unchanged`n"
        }
    }

    # Which configuration the run actually built. CMAKE_BUILD_TYPE does not answer
    # this, and a run compared across a different one measures the configuration.
    $built = (Invoke-Python (Join-Path $HERE 'build_config.py') $Run)
    if (-not $built) { $built = 'unknown' }
    Add-Text $meta "config   $built`n"

    Write-Output ''
    if ($code -eq 0 -and (Get-Item -LiteralPath $result).Length -eq 0) {
        [Console]::Error.WriteLine("run-benchmark: $Agent returned no output; see $Run\run.err")
        $code = 4
    }
    # A run that failed is the one most worth reading, and the analysis is what says
    # why. It runs whatever the exit code was, and it never changes that code.
    switch ($Agent) {
        'claude'  { Write-Output (Invoke-Python (Join-Path $HERE 'analyze_run.py') $Run '--record') }
        'copilot' { Write-Output (Invoke-Python (Join-Path $HERE 'measure.py') (Join-Path $Run 'result.json')) }
        default   { Write-Output "usage: native metrics in $result; no cross-agent cost conversion" }
    }

    if ($Verify -ne 'none') {
        Write-Output ''
        if ($Verify -eq 'sanitize') {
            Write-Output (Invoke-Python (Join-Path $HERE 'verify_run.py') $Run '--sanitize')
        } else {
            Write-Output (Invoke-Python (Join-Path $HERE 'verify_run.py') $Run)
        }
    }
    Remove-Item -LiteralPath $script:HelperPath -ErrorAction SilentlyContinue
    exit $code
}

Main @($args)

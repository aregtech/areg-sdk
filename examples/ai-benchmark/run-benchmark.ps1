# One cold agent run, measured, against a clean snapshot of this checkout.
#
# The Windows version of run-benchmark.sh: the same options, the same run directory
# and the same isolation guards. Runs on Windows PowerShell 5.1 and PowerShell 7.
#
#   .\run-benchmark.ps1 --help

$ErrorActionPreference = 'Continue'

$HERE = $PSScriptRoot
$SDK  = [IO.Path]::GetFullPath((Join-Path $HERE '..\..'))
$Utf8 = New-Object System.Text.UTF8Encoding $false

$USAGE = @'
One cold agent run, measured, against a clean snapshot of this checkout.

  run-benchmark.ps1 [label] [options]

  label              suffix of the run directory: e -> <out>/<date>e-<project>.
                     Optional; the next unused letter for today is chosen.

  --framework NAME   areg | grpc                          (default: areg)
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
  --model NAME       sonnet | haiku | opus                (default: sonnet)
  --effort LEVEL     low | medium | high                  (default: medium)
  --attempts N       the build-and-fix and run-and-fix bound (default: 3). Any other
                     number adds one rule to the prompt, the same for both arms.
                     0 removes the bound, and is warned about: the spend is unbounded.
  --debrief          append a diagnostic pass: what the run could not find. It costs
                     requests on purpose, so such a run is never compared with one
                     made without it.
  --recipes MODE     none | copy, areg only               (default: none)
                       none: no example source may be copied; every file is written
                       or generated. copy: a documented recipe may be copied.
  --verify MODE      none | probes | sanitize             (default: probes)
                       the hidden acceptance probes of verify_run.py, run on the
                       project after the agent ends. sanitize adds a rebuild under
                       ASan and UBSan.
  --out DIR          where run directories are made
                       (default: $env:AREG_BENCHMARK_RUNS, else ~/runs)
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
    .\run-benchmark.ps1 f --model opus --effort high --out D:\runs --dry-run
    .\run-benchmark.ps1 --verify sanitize               areg, probes and sanitizers

  The run directory holds the measurement: meta.txt, prompt.txt, result.json,
  run.err, the fingerprints and sdk/, the snapshot the agent read. The agent works
  in work/, which starts empty.

  The agent is Claude Code, headless. Another agent is run by hand with the same
  prompt.txt; README.md says which of its numbers compare.
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
    $Project = ''; $Mode = 'ipc'; $Model = 'sonnet'; $Effort = 'medium'
    $Attempts = '3'; $Debrief = $false; $Recipes = 'none'; $Label = ''; $Dry = $false
    $AllowInstalled = $false; $Verify = 'probes'
    $Out = if ($env:AREG_BENCHMARK_RUNS) { $env:AREG_BENCHMARK_RUNS } else { Join-Path $HOME 'runs' }

    # A bare first word is the label. Anything starting with a dash is an option.
    $index = 0
    if ($Arguments.Count -gt 0) {
        if ($Arguments[0] -in '-h', '--help') { Write-Output $USAGE; exit 0 }
        if (-not $Arguments[0].StartsWith('-')) { $Label = $Arguments[0]; $index = 1 }
    }

    while ($index -lt $Arguments.Count) {
        $option = $Arguments[$index]
        $valued = '--framework', '--task', '--wrapper', '--project', '--mode', '--model',
                  '--effort', '--attempts', '--recipes', '--out', '--verify'
        if ($option -cin $valued) {
            if ($index + 1 -ge $Arguments.Count) { Stop-Run "$option needs a value" }
            $value = $Arguments[$index + 1]
            switch -casesensitive ($option) {
                '--framework' { $Framework = $value }
                '--task'      { $Task = $value }
                '--wrapper'   { $Wrapper = $value }
                '--project'   { $Project = $value }
                '--mode'      { $Mode = $value }
                '--model'     { $Model = $value }
                '--effort'    { $Effort = $value }
                '--attempts'  { $Attempts = $value }
                '--recipes'   { $Recipes = $value }
                '--out'       { $Out = $value }
                '--verify'    { $Verify = $value }
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
    if ($Model -cnotin 'sonnet', 'haiku', 'opus') { Stop-Run "--model must be sonnet, haiku or opus, not '$Model'" }
    if ($Effort -cnotin 'low', 'medium', 'high') { Stop-Run "--effort must be low, medium or high, not '$Effort'" }
    if ($Recipes -cnotin 'none', 'copy') { Stop-Run "--recipes must be none or copy, not '$Recipes'" }
    if ($Verify -cnotin 'none', 'probes', 'sanitize') { Stop-Run "--verify must be none, probes or sanitize, not '$Verify'" }
    if ($Attempts -notmatch '^[0-9]+$') { Stop-Run "--attempts must be a whole number, not '$Attempts'" }
    if ($Project -cnotmatch '^[A-Za-z_][A-Za-z0-9_]*$') { Stop-Run "--project must be a C identifier, not '$Project'" }

    if (-not (Get-Command claude -ErrorAction SilentlyContinue)) { Stop-Run 'claude not found: install Claude Code and log in' }
    $script:Python = @(Find-Python)
    if ($script:Python.Count -eq 0) { Stop-Run 'python3 not found' }
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
    New-Item -ItemType Directory -Force -Path $Out | Out-Null
    $Out = [IO.Path]::GetFullPath($Out).TrimEnd('\')
    $sdkPrefix = $SDK.TrimEnd('\') + '\'
    if (($Out + '\').StartsWith($sdkPrefix, [StringComparison]::OrdinalIgnoreCase)) {
        Stop-Run "--out $Out is inside the checkout; a run must not write where it reads"
    }

    $today = Get-Utc 'yyyyMMdd'
    if (-not $Label) {
        foreach ($letter in [char[]]'abcdefghijklmnopqrstuvwxyz') {
            if (-not (Test-Path -LiteralPath (Join-Path $Out "$today$letter-$suffix"))) { $Label = "$letter"; break }
        }
        if (-not $Label) { Stop-Run "every label a..z is used for $suffix today; pass one explicitly" }
    }

    $Run = Join-Path $Out "$today$Label-$suffix"
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
        # Only the scenario runner is staged: the areg corpus must not exist inside the run.
        New-Item -ItemType Directory -Force -Path (Join-Path $Snap 'tools\agent') | Out-Null
        Copy-Item -LiteralPath (Join-Path $SDK 'tools\agent\run_scenarios.py') -Destination (Join-Path $Snap 'tools\agent\run_scenarios.py')
        $copied = '1'
        Invoke-Helper manifest $Snap $manifestPath 'tools/agent/run_scenarios.py' | Out-Null
        $stray = @(Get-ChildItem -LiteralPath $Snap -Recurse -File | Where-Object { $_.Name -ne 'run_scenarios.py' } |
                   Select-Object -First 5 | ForEach-Object { $_.FullName })
        if ($stray.Count) { Stop-Run ("the gRPC arm staged more than the scenario runner:`n" + ($stray -join "`n")) }
        # The task travels with the run.
        Copy-Item -LiteralPath $TaskAbs -Destination (Join-Path $Run 'task.md')
        if (Select-String -LiteralPath (Join-Path $Run 'task.md') -Pattern 'areg' -Quiet) {
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
        $TaskRun = To-Slash (Join-Path $Run 'task.md')
        $AddDir = Join-Path $Snap 'tools\agent'
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
        Stop-Run "the gRPC prompt names areg; check $WrapperAbs, and that --out $Out does not"
    }

    $meta = Join-Path $Run 'meta.txt'
    Write-Text $meta (@(
        "framework $Framework", "model    $Model", "effort   $Effort", "task     $TaskRun",
        "mode     $Mode", "recipes  $Recipes", "attempts $Attempts",
        "debrief  $(if ($Debrief) { '1' } else { 'no' })",
        "source   $SDK", "sdk      $Snap", "files    $copied", "head     $head",
        'isolation snapshot, no skills, no MCP servers',
        "start    $(Get-Utc 'yyyy-MM-ddTHH:mm:ssZ')") -join "`n") + "`n"

    if ($Dry) {
        Write-Output "staged $Run ($copied files in the snapshot)"
        Write-Output '--- prompt.txt ---'
        Write-Output ([IO.File]::ReadAllText($promptPath))
        Remove-Item -LiteralPath $script:HelperPath -ErrorAction SilentlyContinue
        exit 0
    }

    Write-Output "run-benchmark: $Run"
    Write-Output "run-benchmark: $Framework, $Model, effort $Effort, attempts $Attempts, head $($head.Substring(0, 8)), $copied files"

    # A project's first build compiles the whole framework; both arms get the same ceiling.
    $env:BASH_DEFAULT_TIMEOUT_MS = '600000'
    $env:BASH_MAX_TIMEOUT_MS = '900000'
    $claude = (Get-Command claude).Source
    $argumentLine = ('-p --output-format json --model {0} --effort {1} --disable-slash-commands ' +
                     '--strict-mcp-config --allowedTools "Bash Read Write Edit Glob Grep" ' +
                     '--add-dir "{2}"') -f $Model, $Effort, $AddDir
    $agent = Start-Process -FilePath $claude -ArgumentList $argumentLine -WorkingDirectory $Work `
                           -RedirectStandardInput $promptPath `
                           -RedirectStandardOutput (Join-Path $Run 'result.json') `
                           -RedirectStandardError (Join-Path $Run 'run.err') `
                           -NoNewWindow -PassThru
    $null = $agent.Handle
    $agent.WaitForExit()
    $code = $agent.ExitCode
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

    Write-Output ''
    Write-Output (Invoke-Python (Join-Path $HERE 'analyze_run.py') $Run)

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

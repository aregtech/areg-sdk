# =============================================================================
#  setup-environment.ps1
#
#  Installs the tools that build Areg on Windows with winget: the Visual Studio
#  Build Tools with the C++ workload, CMake, Java, Git, and optionally clang-cl
#  and Python. Windows PowerShell 5.1 or PowerShell 7. Shows a menu, prints the
#  exact commands, asks before it runs them, then runs tools\check-env.bat.
#
#  Usage:  setup-environment.ps1 [--all] [--with LIST] [--yes] [--dry-run]
#  Exit:   0 = installed and checked, 1 = bad argument, refused or failed,
#          2 = winget is missing.
# =============================================================================

$ErrorActionPreference = 'Stop'

$Here = Split-Path -Parent $MyInvocation.MyCommand.Path

# Menu items in order; the defaults are the required tools.
$Items = @('msvc', 'clang', 'cmake', 'java', 'git', 'python')
$Labels = @{
    'msvc'   = 'C++ compiler: MSVC (Visual Studio Build Tools, C++ workload)'
    'clang'  = 'C++ compiler: clang-cl (adds to the Build Tools)'
    'cmake'  = 'CMake 3.20+'
    'java'   = 'Java 17+ (runs the code generator)'
    'git'    = 'Git'
    'python' = 'Python 3.10+ (agentic coding only)'
}
$Packages = @{
    'cmake'  = 'Kitware.CMake'
    'java'   = 'EclipseAdoptium.Temurin.21.JRE'
    'git'    = 'Git.Git'
    'python' = 'Python.Python.3.12'
}
$Selected = [System.Collections.ArrayList]@('msvc', 'cmake', 'java', 'git')
$DryRun = $false
$AssumeYes = $false
$NoMenu = $false

function Stop-Setup([string]$Message, [int]$Code = 1) {
    [Console]::Error.WriteLine("error: $Message")
    exit $Code
}

function Show-Usage {
    Write-Output 'usage: setup-environment.ps1 [--all] [--with LIST] [--yes] [--dry-run]'
    Write-Output ''
    Write-Output 'Install the tools that build Areg.'
    Write-Output ''
    Write-Output '  --all          select every item of the menu'
    Write-Output '  --with LIST    select these items, comma separated:'
    Write-Output '                 msvc, clang, cmake, java, git, python'
    Write-Output '  --yes          run the commands without asking'
    Write-Output '  --dry-run      print the commands, run nothing'
    Write-Output '  -h, --help     show this help'
    Write-Output ''
    Write-Output 'Without --all or --with, a menu asks on a console.'
}

for ($i = 0; $i -lt $args.Count; $i++) {
    switch ($args[$i]) {
        '--all'     { $Selected = [System.Collections.ArrayList]@($Items); $NoMenu = $true }
        '--with'    {
            if ($i + 1 -ge $args.Count) { Stop-Setup '--with needs a list' }
            $i++
            $Selected = [System.Collections.ArrayList]@($args[$i] -split ',' | ForEach-Object { $_.Trim() } | Where-Object { $_ })
            $NoMenu = $true
        }
        '--yes'     { $AssumeYes = $true }
        '--dry-run' { $DryRun = $true }
        { $_ -in '-h', '--help' } { Show-Usage; exit 0 }
        default     { Show-Usage; Stop-Setup "unknown argument: $($args[$i])" }
    }
}

foreach ($id in $Selected) {
    if ($Items -notcontains $id) { Stop-Setup "unknown item: $id" }
}

if (-not (Get-Command winget -ErrorAction SilentlyContinue)) {
    Stop-Setup 'winget is missing. Install "App Installer" from the Microsoft Store, or install the tools by hand: docs\wiki\01e-build-environment.md' 2
}

# The Visual Studio installation that already has the C++ tools, if any.
function Find-VcTools {
    $vswhere = Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio\Installer\vswhere.exe'
    if (-not (Test-Path $vswhere)) { return $null }
    $path = & $vswhere -latest -products '*' -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
    if ($path) { return $path }
    return $null
}

# --- Menu --------------------------------------------------------------------
$interactive = -not $NoMenu -and [Environment]::UserInteractive -and -not [Console]::IsInputRedirected
if ($interactive) {
    while ($true) {
        Write-Output ''
        Write-Output 'Areg build environment setup (Windows, winget)'
        Write-Output ''
        for ($n = 0; $n -lt $Items.Count; $n++) {
            $id = $Items[$n]
            $mark = if ($Selected -contains $id) { 'x' } else { ' ' }
            Write-Output ('  {0,2}) [{1}] {2}' -f ($n + 1), $mark, $Labels[$id])
        }
        Write-Output ''
        $answer = Read-Host 'Numbers to toggle (e.g. 2 6), Enter to continue, q to quit'
        if ($answer -eq 'q') { exit 1 }
        if ([string]::IsNullOrWhiteSpace($answer)) { break }
        foreach ($num in ($answer -split '\s+' | Where-Object { $_ })) {
            $index = 0
            if ([int]::TryParse($num, [ref]$index) -and $index -ge 1 -and $index -le $Items.Count) {
                $id = $Items[$index - 1]
                if ($Selected -contains $id) { $Selected.Remove($id) } else { [void]$Selected.Add($id) }
            }
        }
    }
}

# --- Commands ----------------------------------------------------------------
$agree = if ($AssumeYes) { ' --accept-source-agreements --accept-package-agreements' } else { '' }
$commands = [System.Collections.ArrayList]@()
$notes = [System.Collections.ArrayList]@()

$vcPath = Find-VcTools
if ($Selected -contains 'msvc' -or $Selected -contains 'clang') {
    if ($vcPath) {
        [void]$notes.Add("Visual Studio with the C++ tools is already installed: $vcPath")
        if ($Selected -contains 'clang') {
            [void]$notes.Add('For clang-cl, add "C++ Clang tools for Windows" in the Visual Studio Installer.')
        }
    } else {
        $vsArgs = '--wait --passive --add Microsoft.VisualStudio.Workload.VCTools --includeRecommended'
        if ($Selected -contains 'clang') {
            $vsArgs += ' --add Microsoft.VisualStudio.Component.VC.Llvm.Clang --add Microsoft.VisualStudio.Component.VC.Llvm.ClangToolset'
        }
        [void]$commands.Add("winget install --id Microsoft.VisualStudio.2022.BuildTools -e$agree --override `"$vsArgs`"")
    }
}
foreach ($id in @('cmake', 'java', 'git', 'python')) {
    if ($Selected -contains $id) {
        [void]$commands.Add("winget install --id $($Packages[$id]) -e$agree")
    }
}

Write-Output ''
if ($commands.Count -eq 0) {
    Write-Output 'Nothing to install for the selected items.'
} else {
    Write-Output 'These commands install the selected tools:'
    foreach ($cmd in $commands) { Write-Output "  $cmd" }
}
foreach ($note in $notes) { Write-Output "  - $note" }
Write-Output ''

if ($DryRun) {
    Write-Output 'Dry run: nothing was run.'
    exit 0
}

if ($commands.Count -gt 0) {
    if (-not $AssumeYes) {
        if (-not $interactive -and [Console]::IsInputRedirected) {
            Stop-Setup 'no console to confirm on; pass --yes to run the commands'
        }
        $answer = Read-Host 'Run them? [y/N]'
        if ($answer -notin 'y', 'Y', 'yes', 'YES') { Write-Output 'Nothing was run.'; exit 1 }
    }
    foreach ($cmd in $commands) {
        Write-Output "> $cmd"
        & cmd.exe /c $cmd
        if ($LASTEXITCODE -ne 0) { Stop-Setup "the command failed: $cmd" }
    }
    # Tools installed above are not on this session's PATH yet.
    $env:Path = [Environment]::GetEnvironmentVariable('Path', 'Machine') + ';' + [Environment]::GetEnvironmentVariable('Path', 'User')
}

Write-Output 'The compiler is on the PATH only in the "Developer PowerShell for VS" or the'
Write-Output '"x64 Native Tools Command Prompt"; build Areg from there.'
& cmd.exe /c (Join-Path $Here 'check-env.bat')
exit $LASTEXITCODE

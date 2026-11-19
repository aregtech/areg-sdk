# =============================================================================
#  build.ps1
#
#  Configures and builds the Areg SDK, or a CMake project that uses it, in a
#  build directory of its own per configuration:
#  <source>\product\build\<family>-<type>-<processor>[-static].
#  Windows PowerShell 5.1 or PowerShell 7. MSVC, clang-cl and MinGW; Cygwin
#  builds with build.sh in a Cygwin shell.
#
#  Usage:  build.ps1 [options] [-- <CMake configure arguments>]
#  Exit:   0 = done, 1 = bad argument, 2 = a required tool is missing,
#          3 = the configure, the build, the tests or the install failed.
# =============================================================================

$ErrorActionPreference = 'Stop'

$Here = Split-Path -Parent $MyInvocation.MyCommand.Path

function Stop-Build([string]$Message, [int]$Code = 1) {
    [Console]::Error.WriteLine("error: $Message")
    exit $Code
}

function Show-Usage {
    Write-Output 'usage: build.ps1 [options] [-- <CMake configure arguments>]'
    Write-Output ''
    Write-Output 'Configure and build with CMake, one build directory per configuration.'
    Write-Output ''
    Write-Output '  -c, --compiler FAMILY  msvc (default), llvm (clang-cl) or mingw'
    Write-Output '  -t, --type TYPE        release (default), debug, relwithdebinfo, minsizerel'
    Write-Output '  -a, --arch ARCH        x86, x64, arm, aarch64. Default: the host processor;'
    Write-Output '                         with mingw, the processor of the compiler'
    Write-Output '  -s, --static           build areg as a static library'
    Write-Output '  -S, --source DIR       project to build. Default: the current directory when'
    Write-Output '                         it holds a CMakeLists.txt, else this Areg SDK'
    Write-Output '  -B, --build-dir DIR    default: <source>\product\build\<family>-<type>-<arch>[-static]'
    Write-Output '  -G, --generator NAME   CMake generator. Default: Visual Studio for msvc and'
    Write-Output '                         llvm, Ninja (else MinGW Makefiles) for mingw. Ninja'
    Write-Output '                         with msvc or llvm needs a Developer PowerShell'
    Write-Output '  -j, --jobs N           parallel build jobs. Default: all cores'
    Write-Output '      --target NAME      build only this target; repeat for more'
    Write-Output '      --test             run the tests with ctest after the build; fails when'
    Write-Output '                         the project has none'
    Write-Output '      --install          install after the build; the location is'
    Write-Output "                         -DCMAKE_INSTALL_PREFIX=<dir> after '--'. Builds all"
    Write-Output '                         targets, so it does not combine with --target'
    Write-Output '      --clean            remove the build output before building'
    Write-Output '      --configure-only   configure, do not build'
    Write-Output '  -n, --dry-run          print the commands, run nothing'
    Write-Output '  -h, --help             show this help'
    Write-Output ''
    Write-Output "Arguments after '--' go to the CMake configure step, for example:"
    Write-Output '  tools\build.bat -c llvm -t debug -- -DAREG_TESTS=OFF -DAREG_EXAMPLES=OFF'
}

# Splits a command line at the spaces outside double quotes and drops the quotes.
function Split-CommandLine([string]$Line) {
    $result = @(); $current = ''; $quoted = $false; $any = $false
    foreach ($ch in $Line.ToCharArray()) {
        if ($ch -eq '"') { $quoted = -not $quoted; $any = $true }
        elseif (-not $quoted -and [char]::IsWhiteSpace($ch)) {
            if ($any) { $result += $current; $current = ''; $any = $false }
        } else { $current += $ch; $any = $true }
    }
    if ($any) { $result += $current }
    return ,$result
}

# Rejoins the arguments PowerShell splits at a colon: '-DA:' 'X', '-DA' 'BOOL=X', '-DA=C' '/dir'.
function Join-ColonSplits([string[]]$Items) {
    $result = @()
    $k = 0
    while ($k -lt $Items.Count) {
        $item = $Items[$k]
        while ($k + 1 -lt $Items.Count) {
            $next = $Items[$k + 1]
            if ($item.EndsWith(':')) { $item += $next }
            elseif ($item -match '^-[DU][^=]+$' -and -not $next.StartsWith('-')) { $item += ':' + $next }
            elseif ($item -match '^-D[^=]+=[A-Za-z]$' -and $next -match '^[\\/]') { $item += ':' + $next }
            else { break }
            $k += 1
        }
        $result += $item
        $k += 1
    }
    return ,$result
}

function Test-Tool([string]$Name) {
    return [bool](Get-Command $Name -ErrorAction SilentlyContinue)
}

$Family = ''; $Type = 'release'; $Arch = ''; $Static = $false; $Source = ''; $BuildDir = ''
$Generator = ''; $Jobs = ''; $Targets = @(); $RunTests = $false; $RunInstall = $false
$Clean = $false; $ConfigureOnly = $false; $DryRun = $false; $Extra = @()

$ValueOptions = @('-c', '--compiler', '-t', '--type', '-a', '--arch', '-S', '--source', '-B', '--build-dir',
                  '-G', '--generator', '-j', '--jobs', '--target')
# build.bat hands over its command line unparsed; PowerShell itself splits '-DNAME:TYPE=VALUE'.
if ($null -ne $env:AREG_BUILD_ARGS) {
    $Argv = Split-CommandLine $env:AREG_BUILD_ARGS
    Remove-Item Env:AREG_BUILD_ARGS
} else {
    $Argv = Join-ColonSplits @($args | ForEach-Object { [string]$_ })
}

# A '-D' or '-U' argument goes to CMake also before '--', which PowerShell drops in a direct call.
$i = 0
while ($i -lt $Argv.Count) {
    $option = [string]$Argv[$i]
    if ($option -ceq '--') {
        if ($i + 1 -lt $Argv.Count) { $Extra += @($Argv[($i + 1)..($Argv.Count - 1)]) }
        break
    }
    if ($option -cmatch '^-[DU].') { $Extra += $option; $i += 1; continue }
    if (@('-h', '--help') -ccontains $option) { Show-Usage; exit 0 }
    $flag = $true
    if (@('-s', '--static') -ccontains $option)       { $Static = $true }
    elseif ($option -ceq '--test')                    { $RunTests = $true }
    elseif ($option -ceq '--install')                 { $RunInstall = $true }
    elseif ($option -ceq '--clean')                   { $Clean = $true }
    elseif ($option -ceq '--configure-only')          { $ConfigureOnly = $true }
    elseif (@('-n', '--dry-run') -ccontains $option)  { $DryRun = $true }
    else                                              { $flag = $false }
    if ($flag) { $i += 1; continue }
    if ($ValueOptions -cnotcontains $option) { Stop-Build "unknown option '$option'; see --help" }
    if ($i + 1 -ge $Argv.Count) { Stop-Build "$option needs a value" }
    $value = [string]$Argv[$i + 1]
    switch -CaseSensitive ($option) {
        { @('-c', '--compiler') -ccontains $_ }   { $Family = $value.ToLowerInvariant() }
        { @('-t', '--type') -ccontains $_ }       { $Type = $value.ToLowerInvariant() }
        { @('-a', '--arch') -ccontains $_ }       { $Arch = $value.ToLowerInvariant() }
        { @('-S', '--source') -ccontains $_ }     { $Source = $value }
        { @('-B', '--build-dir') -ccontains $_ }  { $BuildDir = $value }
        { @('-G', '--generator') -ccontains $_ }  { $Generator = $value }
        { @('-j', '--jobs') -ccontains $_ }       { $Jobs = $value }
        '--target'                                { $Targets += $value }
    }
    $i += 2
}

$CMakeTypes = @{ 'release' = 'Release'; 'debug' = 'Debug'; 'relwithdebinfo' = 'RelWithDebInfo'; 'minsizerel' = 'MinSizeRel' }
if (-not $CMakeTypes.ContainsKey($Type)) {
    Stop-Build "unknown build type '$Type'; use release, debug, relwithdebinfo or minsizerel"
}
$CMakeType = $CMakeTypes[$Type]

if ($Jobs -and ($Jobs -notmatch '^[1-9][0-9]*$')) { Stop-Build "--jobs needs a positive number, not '$Jobs'" }
if (-not $Jobs) { $Jobs = [string]$env:NUMBER_OF_PROCESSORS }
if (-not $Jobs) { $Jobs = '4' }

if ($RunInstall -and $Targets.Count -gt 0) { Stop-Build '--install needs every target built; leave out --target' }

# A compiler or a toolchain file given to CMake decides the compiler instead of the family.
$CustomCompiler = [bool]($Extra | Where-Object { $_ -match '^-DCMAKE_(CXX_COMPILER|TOOLCHAIN_FILE)(:[A-Z]+)?=' })

switch ($Family) {
    ''       { if (-not $CustomCompiler) { $Family = 'msvc' } }
    'msvc'   { }
    'llvm'   { }
    'mingw'  { }
    'cygwin' { Stop-Build 'cygwin builds with tools/build.sh in a Cygwin shell' }
    'gnu'    { Stop-Build "gnu on Windows is 'mingw', or 'cygwin' with tools/build.sh" }
    default  { Stop-Build "unknown compiler family '$Family'; use msvc, llvm or mingw" }
}

$ArchNames = @{ 'x86' = 'x86'; 'i386' = 'x86'; 'i686' = 'x86'; 'x64' = 'x64'; 'x86_64' = 'x64'; 'amd64' = 'x64'
                'arm' = 'arm'; 'arm32' = 'arm'; 'armv7' = 'arm'; 'aarch64' = 'aarch64'; 'arm64' = 'aarch64' }
if ($Arch) {
    if (-not $ArchNames.ContainsKey($Arch)) { Stop-Build "unknown processor '$Arch'; use x86, x64, arm or aarch64" }
    $Arch = $ArchNames[$Arch]
}

$HostCpu = $env:PROCESSOR_ARCHITEW6432
if (-not $HostCpu) { $HostCpu = $env:PROCESSOR_ARCHITECTURE }
$HostArch = 'x64'
if ($HostCpu -and $ArchNames.ContainsKey($HostCpu.ToLowerInvariant())) { $HostArch = $ArchNames[$HostCpu.ToLowerInvariant()] }

$NameArch = $Arch
if (-not $NameArch) {
    $NameArch = $HostArch
    if ($Family -eq 'mingw' -and (Test-Tool 'g++')) {
        $machine = (& g++ -dumpmachine 2>$null | Out-String).Trim()
        if ($machine -match '^(i[3-6]86)') { $NameArch = 'x86' }
        elseif ($machine -match '^x86_64') { $NameArch = 'x64' }
        elseif ($machine -match '^aarch64') { $NameArch = 'aarch64' }
    }
}

if (-not $Generator -and $env:CMAKE_GENERATOR) { $Generator = $env:CMAKE_GENERATOR }
if (-not $Generator -and $Family -eq 'mingw') {
    if (Test-Tool 'ninja') { $Generator = 'Ninja' } else { $Generator = 'MinGW Makefiles' }
}
$VisualStudio = (-not $Generator) -or $Generator.StartsWith('Visual Studio')

if (-not $Source) {
    if (Test-Path -LiteralPath 'CMakeLists.txt' -PathType Leaf) {
        $Source = '.'
    } elseif ((Test-Path (Join-Path $Here '..\CMakeLists.txt') -PathType Leaf) -and
              (Test-Path (Join-Path $Here '..\conf\cmake\setup.cmake') -PathType Leaf)) {
        $Source = Join-Path $Here '..'
    } else {
        Stop-Build 'no CMakeLists.txt in the current directory; give the project with --source'
    }
}
if (-not (Test-Path (Join-Path $Source 'CMakeLists.txt') -PathType Leaf)) { Stop-Build "'$Source' has no CMakeLists.txt" }
$Source = (Resolve-Path -LiteralPath $Source).ProviderPath

$ExplicitDir = [bool]$BuildDir
if (-not $BuildDir) {
    $FamilyName = $Family
    if (-not $FamilyName) { $FamilyName = 'custom' }
    $Name = "$FamilyName-$Type-$NameArch"
    if ($Static) { $Name += '-static' }
    $BuildDir = Join-Path $Source "product\build\$Name"
}

if (-not $DryRun) {
    if (-not (Test-Tool 'cmake')) { Stop-Build 'cmake is not installed or not in PATH' 2 }
    if ($RunTests -and -not (Test-Tool 'ctest')) { Stop-Build 'ctest is not installed or not in PATH' 2 }
    if (-not $VisualStudio) {
        if ($Family -eq 'msvc' -and -not (Test-Tool 'cl')) { Stop-Build "cl is not in PATH; run from a Developer PowerShell for '$Generator'" 2 }
        if ($Family -eq 'llvm' -and -not (Test-Tool 'clang-cl')) { Stop-Build "clang-cl is not in PATH; run from a Developer PowerShell for '$Generator'" 2 }
        if ($Family -eq 'mingw' -and -not (Test-Tool 'g++')) { Stop-Build 'g++ of MinGW is not in PATH' 2 }
    }
}

function Invoke-Step([string]$Tool, [string[]]$Arguments) {
    $shown = ($Arguments | ForEach-Object { if ($_ -match '\s') { '"' + $_ + '"' } else { $_ } }) -join ' '
    Write-Output "+ $Tool $shown"
    if ($DryRun) { return }
    $ErrorActionPreference = 'Continue'
    & $Tool @Arguments
    if ($LASTEXITCODE -ne 0) { Stop-Build "'$Tool' failed with exit code $LASTEXITCODE" 3 }
}

$Configure = @('-S', $Source, '-B', $BuildDir)
if ($Generator) { $Configure += @('-G', $Generator) }
if ($VisualStudio) {
    $Platforms = @{ 'x86' = 'Win32'; 'x64' = 'x64'; 'arm' = 'ARM'; 'aarch64' = 'ARM64' }
    $Configure += @('-A', $Platforms[$NameArch])
    if ($Family -eq 'llvm') { $Configure += @('-T', 'ClangCL') }
}
$Configure += "-DCMAKE_BUILD_TYPE=$CMakeType"
if ($Family) { $Configure += "-DAREG_COMPILER_FAMILY=$Family" }
if ($Arch -and -not $VisualStudio) { $Configure += "-DAREG_ARCH=$Arch" }
if ($Static) { $Configure += '-DAREG_LIB_TYPE=static' }
$Configure += $Extra

# A configured default directory is configured again only for new arguments; the build re-runs it on a change.
$HasCache = Test-Path (Join-Path $BuildDir 'CMakeCache.txt') -PathType Leaf
if ($ConfigureOnly -or -not $HasCache -or $Extra.Count -gt 0 -or $ExplicitDir) {
    Invoke-Step 'cmake' $Configure
}
if ($ConfigureOnly) { exit 0 }

$Build = @('--build', $BuildDir, '--config', $CMakeType, '-j', $Jobs)
foreach ($target in $Targets) { $Build += @('--target', $target) }
if ($Clean) { $Build += '--clean-first' }
Invoke-Step 'cmake' $Build

if ($RunTests) {
    Invoke-Step 'ctest' @('--test-dir', $BuildDir, '-C', $CMakeType, '-j', $Jobs, '--output-on-failure', '--no-tests=error')
}

if ($RunInstall) {
    Invoke-Step 'cmake' @('--install', $BuildDir, '--config', $CMakeType)
}

Write-Output "Build directory: $BuildDir"
exit 0

<#
.SYNOPSIS
    Migrates <Constant> elements to the attribute form for Value and DeprecateHint.

.DESCRIPTION
    Two changes, applied to every .siml, .fsml and .dtml file under the chosen root:

      1. Inside a <Constant> element, a <Value>text</Value> child becomes a Value="text"
         attribute on the <Constant> start tag.
      2. Inside a <Constant> element, a <DeprecateHint>text</DeprecateHint> child becomes a
         DeprecateHint="text" attribute on the <Constant> start tag.

    Only <Constant> is touched. The other elements that carry a deprecation hint convert with
    their own editor page, so that each change ships with the code that reads and writes it.

    <Description> is never touched: it is free-form, possibly multi-line prose, and XML
    attribute-value normalization would collapse its newlines.

    <Value> children of Field, Parameter and EnumEntry elements are left alone. Those carry an
    IsDefault attribute of their own, so they are structural and are handled separately.

    The rewrite is line oriented, not a parse-and-serialize round trip. Only the lines that
    actually change are rewritten, so the XML declaration, the indentation, the line endings and
    every untouched byte of the file survive exactly as they were. Files that need no change are
    not written at all, so the script is safe to run twice.

    The shell script beside this one, migrate-siml-constants.sh, applies the same transformation
    and produces byte-identical results.

.PARAMETER Root
    Directory to search. If omitted, the script asks on the console and offers the parent of its
    own directory, which is the repository root when the script sits in tools/.

.PARAMETER DryRun
    Report what would change and write nothing. -WhatIf does the same.

.EXAMPLE
    # migrate the repository this script lives in
    pwsh -NoProfile -File ./tools/migrate-siml-constants.ps1 -Root ..

.EXAMPLE
    # ask for the root on the console
    pwsh -NoProfile -File ./tools/migrate-siml-constants.ps1

.EXAMPLE
    # list what would change, write nothing
    pwsh -NoProfile -File ./tools/migrate-siml-constants.ps1 -Root .. -DryRun
#>
[CmdletBinding(SupportsShouldProcess = $true)]
param(
    [string] $Root,
    [switch] $DryRun
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

$script:Pretend = $DryRun.IsPresent -or $WhatIfPreference

# ---------------------------------------------------------------------------------------------
# Root selection
# ---------------------------------------------------------------------------------------------

$scriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$defaultRoot = Split-Path -Parent $scriptDir

if ([string]::IsNullOrWhiteSpace($Root))
{
    $answer = ''
    try
    {
        $answer = Read-Host "Root directory of the project [$defaultRoot]"
    }
    catch
    {
        # No console attached. Take the default rather than block a scripted run.
        $answer = ''
    }

    $Root = if ([string]::IsNullOrWhiteSpace($answer)) { $defaultRoot } else { $answer.Trim().Trim('"') }
}

if (-not (Test-Path -LiteralPath $Root -PathType Container))
{
    Write-Error "Root not found or not a directory: $Root"
    exit 1
}

$Root = (Resolve-Path -LiteralPath $Root).Path
Write-Host "Root: $Root"

# ---------------------------------------------------------------------------------------------
# The transformation
# ---------------------------------------------------------------------------------------------

function ConvertTo-AttributeValue
{
    param([string] $Text)

    # The captured text is already escaped for element content, so & and < are intact. An
    # attribute delimited with double quotes needs the quote escaped as well, and a tab escaped
    # or attribute-value normalization would silently turn it into a space.
    $Text = $Text.Replace('"', '&quot;')
    $Text = $Text.Replace("`t", '&#9;')
    return $Text
}

function Get-StartTagEnd
{
    param([string] $Text, [int] $Start)

    # Index of the '>' that closes a start tag beginning at $Start, or -1 while it is unclosed.
    # Quoted attribute values may contain '>', so track the quote state.
    $quote = [char] 0
    for ($i = $Start; $i -lt $Text.Length; $i++)
    {
        $c = $Text[$i]
        if ($quote -ne [char] 0)
        {
            if ($c -eq $quote) { $quote = [char] 0 }
            continue
        }

        if ($c -eq '"' -or $c -eq "'") { $quote = $c; continue }
        if ($c -eq '>') { return $i }
    }

    return -1
}

function Convert-Document
{
    param([string] $Text, [string] $Path)

    # Splitting on the line feed alone leaves any carriage return at the end of the line content,
    # so CRLF, LF and mixed files all rejoin exactly as they came in.
    $lines = $Text.Split("`n")
    $out = New-Object System.Collections.Generic.List[string]
    $valueMoved = 0
    $hintMoved = 0
    $warnings = New-Object System.Collections.Generic.List[string]

    $i = 0
    while ($i -lt $lines.Count)
    {
        $line = $lines[$i]
        $trimmed = $line.Trim()

        # The trailing class keeps <ConstantList> from reading as a <Constant>.
        if ($trimmed -notmatch '^<Constant([\s/>]|$)')
        {
            if ($line -match '<Constant[\s/>]')
            {
                $warnings.Add("line $($i + 1): a <Constant> start tag that does not begin its line was left alone")
            }

            $out.Add($line)
            $i++
            continue
        }

        # Collect the start tag, which may in principle span lines.
        $tagLines = New-Object System.Collections.Generic.List[string]
        $j = $i
        $joined = ''
        $tagEnd = -1
        while ($j -lt $lines.Count)
        {
            $tagLines.Add($lines[$j])
            $joined = [string]::Join("`n", $tagLines)
            $tagEnd = Get-StartTagEnd -Text $joined -Start $joined.IndexOf('<Constant')
            if ($tagEnd -ge 0) { break }
            $j++
        }

        if ($tagEnd -lt 0)
        {
            $warnings.Add("line $($i + 1): unterminated <Constant> start tag, left alone")
            $out.Add($line)
            $i++
            continue
        }

        if ($joined[$tagEnd - 1] -eq '/')
        {
            # Self-closing: no children to move.
            foreach ($t in $tagLines) { $out.Add($t) }
            $i = $j + 1
            continue
        }

        # Walk the body to the matching close tag, noting where the two children sit. Every line
        # is kept, so any path that decides not to convert can emit the element untouched.
        $body = New-Object System.Collections.Generic.List[string]
        $valueText = $null
        $hintText = $null
        $valueIndex = -1
        $hintIndex = -1
        $k = $j + 1
        $closed = $false
        $malformed = $false
        $duplicate = $null

        while ($k -lt $lines.Count)
        {
            $t = $lines[$k].Trim()

            if ($t -eq '</Constant>') { $closed = $true; break }
            if ($t -match '^<Constant([\s/>]|$)') { $malformed = $true; break }

            $captured = $false

            if ($t -match '^<Value>(.*)</Value>$')
            {
                if ($valueIndex -ge 0) { $duplicate = 'Value' }
                $valueText = $Matches[1]
                $valueIndex = $body.Count
                $captured = $true
            }
            elseif ($t -match '^<Value\s*/>$')
            {
                if ($valueIndex -ge 0) { $duplicate = 'Value' }
                $valueText = ''
                $valueIndex = $body.Count
                $captured = $true
            }
            elseif ($t -match '^<DeprecateHint>(.*)</DeprecateHint>$')
            {
                if ($hintIndex -ge 0) { $duplicate = 'DeprecateHint' }
                $hintText = $Matches[1]
                $hintIndex = $body.Count
                $captured = $true
            }
            elseif ($t -match '^<DeprecateHint\s*/>$')
            {
                if ($hintIndex -ge 0) { $duplicate = 'DeprecateHint' }
                $hintText = ''
                $hintIndex = $body.Count
                $captured = $true
            }

            if (-not $captured -and ($t -match '^<(Value|DeprecateHint)[\s>]' -or $t -match '^<(Value|DeprecateHint)$'))
            {
                # Opens here and closes on a later line. Converting it would fold a newline into
                # an attribute, where normalization turns it into a space. Report, do not guess.
                $warnings.Add("line $($k + 1): a multi-line <$($Matches[1])> was left alone")
            }

            $body.Add($lines[$k])
            $k++
        }

        if ($malformed -or -not $closed)
        {
            $warnings.Add("line $($i + 1): <Constant> without a matching close tag, left alone")
            foreach ($t in $tagLines) { $out.Add($t) }
            $i = $j + 1
            continue
        }

        $startTag = $joined.Substring(0, $tagEnd)
        $refuse = $null
        if ($null -ne $duplicate)
        {
            $refuse = "carries more than one <$duplicate> child"
        }
        elseif ($valueIndex -ge 0 -and $startTag -match '\sValue\s*=')
        {
            $refuse = 'carries both a Value attribute and a <Value> child'
        }
        elseif ($hintIndex -ge 0 -and $startTag -match '\sDeprecateHint\s*=')
        {
            $refuse = 'carries both a DeprecateHint attribute and a <DeprecateHint> child'
        }

        if ($null -ne $refuse)
        {
            $warnings.Add("line $($i + 1): <Constant> $refuse, left alone")
        }

        if ($null -ne $refuse -or ($valueIndex -lt 0 -and $hintIndex -lt 0))
        {
            foreach ($t in $tagLines) { $out.Add($t) }
            foreach ($b in $body) { $out.Add($b) }
            $out.Add($lines[$k])
            $i = $k + 1
            continue
        }

        $attributes = ''
        if ($valueIndex -ge 0)
        {
            $attributes += ' Value="' + (ConvertTo-AttributeValue -Text $valueText) + '"'
            $valueMoved++
        }

        if ($hintIndex -ge 0)
        {
            $attributes += ' DeprecateHint="' + (ConvertTo-AttributeValue -Text $hintText) + '"'
            $hintMoved++
        }

        $rewritten = $joined.Insert($tagEnd, $attributes)
        foreach ($t in $rewritten.Split("`n")) { $out.Add($t) }
        for ($b = 0; $b -lt $body.Count; $b++)
        {
            if ($b -eq $valueIndex -or $b -eq $hintIndex) { continue }
            $out.Add($body[$b])
        }

        $out.Add($lines[$k])
        $i = $k + 1
    }

    return [pscustomobject] @{
        Text     = [string]::Join("`n", $out)
        Value    = $valueMoved
        Hint     = $hintMoved
        Warnings = $warnings
    }
}

# ---------------------------------------------------------------------------------------------
# Walk
# ---------------------------------------------------------------------------------------------

# Generated trees hold copies of the sources; rewriting those would be pointless and slow.
$wanted = @('.siml', '.fsml', '.dtml')
$skipDirs = @('build', 'product', '.git', 'out', 'node_modules')
$separators = [char[]] @('\', '/')

$files = @(Get-ChildItem -LiteralPath $Root -Recurse -File -ErrorAction SilentlyContinue |
           Where-Object {
               if ($wanted -notcontains $_.Extension.ToLowerInvariant()) { return $false }
               $parts = $_.FullName.Substring($Root.Length).Split($separators)
               foreach ($part in $parts)
               {
                   if ($skipDirs -contains $part.ToLowerInvariant()) { return $false }
               }
               return $true
           } | Sort-Object FullName)

$changedFiles = 0
$totalValue = 0
$totalHint = 0
$totalWarnings = 0
$utf8NoBom = New-Object System.Text.UTF8Encoding($false)

foreach ($file in $files)
{
    $bytes = [System.IO.File]::ReadAllBytes($file.FullName)
    $bom = ($bytes.Length -ge 3 -and $bytes[0] -eq 0xEF -and $bytes[1] -eq 0xBB -and $bytes[2] -eq 0xBF)
    $offset = if ($bom) { 3 } else { 0 }
    $text = $utf8NoBom.GetString($bytes, $offset, $bytes.Length - $offset)

    $result = Convert-Document -Text $text -Path $file.FullName

    foreach ($w in $result.Warnings)
    {
        Write-Warning "$($file.FullName): $w"
        $totalWarnings++
    }

    if (($result.Value + $result.Hint) -eq 0)
    {
        continue
    }

    $changedFiles++
    $totalValue += $result.Value
    $totalHint += $result.Hint

    $verb = if ($script:Pretend) { 'would change' } else { 'changed' }
    Write-Host ("{0}  {1}  (Value: {2}, DeprecateHint: {3})" -f $verb, $file.FullName, $result.Value, $result.Hint)

    if ($script:Pretend)
    {
        continue
    }

    $outBytes = $utf8NoBom.GetBytes($result.Text)
    if ($bom)
    {
        $outBytes = ([byte[]] @(0xEF, 0xBB, 0xBF)) + $outBytes
    }

    [System.IO.File]::WriteAllBytes($file.FullName, $outBytes)
}

Write-Host ''
Write-Host ("Files scanned: {0}" -f $files.Count)
Write-Host ("Files changed: {0}" -f $changedFiles)
Write-Host ("Value attributes written: {0}" -f $totalValue)
Write-Host ("DeprecateHint attributes written: {0}" -f $totalHint)
if ($totalWarnings -gt 0)
{
    Write-Host ("Warnings: {0}" -f $totalWarnings)
}

if ($script:Pretend)
{
    Write-Host ''
    Write-Host '*** DRY RUN: nothing was written. Run again without -DryRun to apply. ***'
}

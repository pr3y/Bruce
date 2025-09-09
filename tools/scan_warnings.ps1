$ErrorActionPreference = 'Continue'

# Find the most recent build log for the lilygo-t-embed-cc1101 env
$log = Get-ChildItem -Path "logs" -Filter "build-lilygo-t-embed-cc1101-*.log" -File |
    Sort-Object LastWriteTime -Descending |
    Select-Object -First 1

if (-not $log) {
    Write-Error "No build logs found in 'logs' folder."
    exit 1
}

Write-Output ("Log: {0}" -f $log.FullName)

# Collect all 'warning:' lines
$warns = Select-String -Path $log.FullName -Pattern 'warning:'
Write-Output ("Total warnings: {0}" -f $warns.Count)

# Filter to project files (src, lib, boards, include under repo), excluding external paths
$excludePatterns = @('.pio\', '.platformio\', 'packages\', 'framework-arduino', '\\.pio\\', '\\framework-arduino')
$vendorLibExcludes = @('lib\\TFT_eSPI\\', 'lib/TFT_eSPI/', 'lib\\TFT_eSPI_QRcode\\', 'lib/TFT_eSPI_QRcode/')

$projWarns = foreach ($w in $warns) {
    $line = $w.Line
    # Quick skip of obvious external paths
    if ($excludePatterns | Where-Object { $line -like "*$_*" }) { continue }
    # Keep only lines that reference typical project folders
    if ($line -like '*\src\*' -or $line -like '*/src/*' -or
        $line -like '*\lib\*' -or $line -like '*/lib/*' -or
        $line -like '*\boards\*' -or $line -like '*/boards/*' -or
        $line -like '*\include\*' -or $line -like '*/include/*' -or
        $line -like 'src/*' -or $line -like 'lib/*' -or
        $line -like 'boards/*' -or $line -like 'include/*') {
        # Exclude vendor libs (optional)
        if ($vendorLibExcludes | Where-Object { $line -like "*$_*" }) { continue }
        $w
    }
}

Write-Output ("Project warnings: {0}" -f $projWarns.Count)
if ($projWarns.Count -gt 0) {
    "--- Project warning lines ---"
    $projWarns | Select-Object -First 50 -ExpandProperty Line

    # Group by file (best effort: parse path before :line:col:)
    $regex = '^(?<path>.*?):\d+:\d+:\s*warning:'
    $files = @{}
    foreach ($w in $projWarns) {
        if ($w.Line -match $regex) {
            $p = $Matches['path']
            if (-not $files.ContainsKey($p)) { $files[$p] = 0 }
            $files[$p]++
        }
    }
    if ($files.Count -gt 0) {
        "--- Project warning counts by file (top 20) ---"
        $files.GetEnumerator() | Sort-Object -Property Value -Descending | Select-Object -First 20 | ForEach-Object { "{0}  =>  {1}" -f $_.Key, $_.Value }
    }
}

exit 0

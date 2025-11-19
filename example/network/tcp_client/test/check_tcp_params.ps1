# PowerShell script to print major Windows TCP parameters and their descriptions

function Print-ParamWithDesc {
    param(
        [string]$Label,
        [string]$Value,
        [string]$Desc
    )
    "{0,-30} : {1}" -f $Label, $Value
    "  Description: $Desc"
    ""
}

Write-Host "========================================"
Write-Host " Check TCP Parameter Status (Windows)"
Write-Host "========================================"
Write-Host ""

# 1) TcpTimedWaitDelay (similar to tcp_fin_timeout)
$tcpTimedWaitDelay = Get-ItemProperty -Path "HKLM:\SYSTEM\CurrentControlSet\Services\Tcpip\Parameters" -Name "TcpTimedWaitDelay" -ErrorAction SilentlyContinue | Select-Object -ExpandProperty TcpTimedWaitDelay -ErrorAction SilentlyContinue
if (-not $tcpTimedWaitDelay) { $tcpTimedWaitDelay = "Default (240 seconds)" }
Print-ParamWithDesc "TcpTimedWaitDelay" $tcpTimedWaitDelay "TIME_WAIT duration after socket close (default 240 seconds). Similar to Linux tcp_fin_timeout."

# 2) TcpNumConnections (similar to tcp_max_tw_buckets)
$tcpNumConnections = Get-ItemProperty -Path "HKLM:\SYSTEM\CurrentControlSet\Services\Tcpip\Parameters" -Name "TcpNumConnections" -ErrorAction SilentlyContinue | Select-Object -ExpandProperty TcpNumConnections -ErrorAction SilentlyContinue
if (-not $tcpNumConnections) { $tcpNumConnections = "Default (dynamic)" }
Print-ParamWithDesc "TcpNumConnections" $tcpNumConnections "Maximum number of concurrent TCP connections. Similar to Linux tcp_max_tw_buckets."

# 3) MaxUserPort (similar to ip_local_port_range)
$maxUserPort = Get-ItemProperty -Path "HKLM:\SYSTEM\CurrentControlSet\Services\Tcpip\Parameters" -Name "MaxUserPort" -ErrorAction SilentlyContinue | Select-Object -ExpandProperty MaxUserPort -ErrorAction SilentlyContinue
if (-not $maxUserPort) { $maxUserPort = "Default (5000-65534)" }
Print-ParamWithDesc "MaxUserPort" $maxUserPort "Highest port number for ephemeral ports. Similar to Linux ip_local_port_range."

# 4) TcpMaxSynBacklog (not directly available, but can show SynAttackProtect)
$synAttackProtect = Get-ItemProperty -Path "HKLM:\SYSTEM\CurrentControlSet\Services\Tcpip\Parameters" -Name "SynAttackProtect" -ErrorAction SilentlyContinue | Select-Object -ExpandProperty SynAttackProtect -ErrorAction SilentlyContinue
if (-not $synAttackProtect) { $synAttackProtect = "Default (0)" }
Print-ParamWithDesc "SynAttackProtect" $synAttackProtect "Enables SYN attack protection. No direct equivalent to tcp_max_syn_backlog in Windows."

# 5) Listen backlog (not directly configurable globally, but can show for current TCP settings)
$listenBacklog = (Get-NetTCPSetting | Select-Object -First 1).MaxSynBacklog
if (-not $listenBacklog) { $listenBacklog = "Default (varies by version)" }
Print-ParamWithDesc "MaxSynBacklog" $listenBacklog "Maximum SYN backlog for listen(). Similar to Linux somaxconn/tcp_max_syn_backlog."

Write-Host "========================================"
Write-Host " Check Current Number of TIME_WAIT Sockets"
Write-Host "========================================"
Write-Host ""

# Check number of TIME_WAIT sockets
$timeWaitCount = netstat -an | Select-String "TIME_WAIT" | Measure-Object | Select-Object -ExpandProperty Count
Write-Host "Current number of TIME_WAIT sockets: $timeWaitCount"
Write-Host ""
Write-Host "========================================"
Write-Host " Note:"
Write-Host "  - This script only queries values."
Write-Host "  - To change settings, use regedit or PowerShell with administrator privileges."
Write-Host "  - Some parameters may not exist if defaults are used."
Write-Host "========================================"



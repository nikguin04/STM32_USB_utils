$usbDevices = Get-PnpDevice -PresentOnly | Where-Object { $_.InstanceId -match '^USB' }

$usbDevices | ForEach-Object {
    $deviceID = $_.InstanceId
    if ($deviceID -match 'VID_([0-9A-F]{4})') {
        $vendorID = $matches[1]
    } else {
        $vendorID = 'Unknown'
    }
    
    if ($vendorID -eq '0483') {
        [PSCustomObject]@{
            Name = $_.FriendlyName
            VendorID = $vendorID
            DeviceID = $deviceID
            Status = $_.Status
        }
    }
} | Format-Table -AutoSize

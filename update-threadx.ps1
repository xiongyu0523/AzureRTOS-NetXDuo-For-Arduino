# script to update Azure RTOS ThreadX from Microsoft official repo

$Response = Invoke-WebRequest https://api.github.com/repos/azure-rtos/netxduo/releases/latest
$vesion = ($Response | ConvertFrom-Json).tag_name

Write-Host "The latest Azure RTOS NetX Duo version is $vesion"

#git clone https://github.com/azure-rtos/netxduo --recurse-submodules
#Set-Location netxduo
#git checkout -b $vesion $vesion
#Set-Location ..

Write-Host "Copying files..."

Remove-Item -Force -Path "src/az_http*"

#Remove-Item -Force -Recurse -Path "netxduo" 

Write-Host "Updatge done!"
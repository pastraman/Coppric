Write-Host "Building Coppric..."
gcc src/*.c -o coppric.exe

Write-Host "Installing syntax highlighting..."
$nvimConfig = "$env:LOCALAPPDATA\nvim"
New-Item -ItemType Directory -Force -Path "$nvimConfig\syntax" | Out-Null
New-Item -ItemType Directory -Force -Path "$nvimConfig\ftdetect" | Out-Null

Copy-Item "tools\cpr.vim" "$nvimConfig\syntax\cpr.vim"
Copy-Item "tools\ftdetect-cpr.vim" "$nvimConfig\ftdetect\cpr.vim"

Write-Host "Done. Move coppric.exe to a directory in your PATH to use globally."

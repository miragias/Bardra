# Step 1: Build the solution
Write-Host "Building the solution..."
msbuild "C:\Users\ioann\source\repos\Ogre2\Ogre2.sln" /p:Configuration=Release

if ($LASTEXITCODE -ne 0) {
    Write-Host "Build failed. Exiting." -ForegroundColor Red
    exit 1
}

# Step 2: Run the executable
Write-Host "Running the application..."
Start-Process -FilePath "C:\Users\ioann\source\repos\Ogre2\x64\Release\Ogre2.exe"
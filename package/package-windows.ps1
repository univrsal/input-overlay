# Automatic packaging for windows builds

param([string]$version)
[System.Reflection.Assembly]::LoadWithPartialName("System.Windows.Forms")

$base_dir = "C:\Users\usr\Documents\git\"
$data_dir = "../data"
$project = "input-overlay"
$arch_both = "win32.64"
$arch_64 = "win64"
$msvc = "2017"
$qt = "5_12_0"

$build_location_x32 = $base_dir + "build\x32\rundir\Release\obs-plugins\32bit"
$qtc_build_location_x32 = $base_dir + "build-obs-studio-Desktop_Qt_" + $qt + "_MSVC" + $msvc + "_32bit-Release\rundir\Release\obs-plugins\32bit"

$build_location_x64 = $base_dir + "build\x64\rundir\Release\obs-plugins\64bit"
$qtc_build_location_x64 = $base_dir + "build-obs-studio-Desktop_Qt_" + $qt + "_MSVC" + $msvc + "_64bit-Release\rundir\Release\obs-plugins\64bit"

$uiohook="../libuiohook/bin"
$zip="C:/Program Files/7-Zip/7z.exe"
Set-Alias sz $zip

function replace($file, $what, $with)
{
    ((Get-Content $file) -replace $what, $with) | Set-Content -Path $file
}

while ($version.length -lt 1) {
    $version = Read-Host -Prompt 'Enter a version string'
    $build_dir="./" + $project + ".v" + $version + "." + $arch
}

$Result = [System.Windows.Forms.MessageBox]::Show("Use Qt Creator builds?", "Build package script",3,[System.Windows.Forms.MessageBoxIcon]::Question) 

If ($result -eq "Yes") {
    echo("Using Qtc builds...")
    $build_location_x32 = $qtc_build_location_x32
    $build_location_x64 = $qtc_build_location_x64
} else {
    echo("Using VS builds...")
}

$Result = [System.Windows.Forms.MessageBox]::Show("64bit only build?", "Build package script",3,[System.Windows.Forms.MessageBoxIcon]::Question) 

If ($result -eq "Yes") {
    $x86 = $false
    $arch = $arch_64
    echo("64bit only build...")
    $build_location_x32 = $qtc_build_location_x32
    $build_location_x64 = $qtc_build_location_x64
} else {
    $x86 = $true
    $arch = $arch_both
    echo("Packaging 32/64bit...")
}

while ($version.length -lt 1) {
    
    $version = Read-Host -Prompt 'Enter a version string'
    $build_dir="./" + $project + ".v" + $version + "." + $arch
}
$build_dir = "./" + $project + ".v" + $version + "." + $arch

echo("Creating build directory")
New-Item $build_dir/plugin/data/obs-plugins/input-overlay -itemtype directory
if ($x86) {
    New-Item $build_dir/plugin/obs-plugins/32bit -itemtype directory
}
New-Item $build_dir/plugin/obs-plugins/64bit -itemtype directory

if ($x86) {
    echo("Fetching build from $build_location_x32")
    Copy-Item $build_location_x32/$project.dll -Destination $build_dir/plugin/obs-plugins/32bit/
}

echo("Fetching build from $build_location_x64")
Copy-Item $build_location_x64/$project.dll -Destination $build_dir/plugin/obs-plugins/64bit/

echo("Fetching dependencies from $uiohook")
Copy-Item $uiohook/win64/uiohook.dll -Destination $build_dir/plugin/obs-plugins/64bit/
if ($x86) {
    Copy-Item $uiohook/win32/uiohook.dll -Destination $build_dir/plugin/obs-plugins/32bit/
}

echo("Copying presets")
Copy-Item ../presets -Destination $build_dir -Recurse

echo("Fetching data")
Copy-Item $data_dir/* -Destination $build_dir/plugin/data/obs-plugins/input-overlay/ -Recurse
Copy-Item ../LICENSE -Destination $build_dir/LICENSE.txt
Copy-Item ./README_WINDOWS.txt $build_dir/README.txt
replace $build_dir/README.txt "@VERSION" $version

echo("Making archive")
cd $build_dir
sz a -r "../$build_dir.zip" "./*"
cd ..

echo("Cleaning up")
Remove-Item $build_dir/ -Recurse
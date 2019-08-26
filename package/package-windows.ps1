# Automatic packaging for windows builds

param([string]$version)
$data_dir="../data"
$project="input-overlay"
$arch="win32.64"
$build_location_x32="C:\Users\usr\Documents\git\build\x32\rundir\Release\obs-plugins\32bit"
$build_location_x64="C:\Users\usr\Documents\git\build\x64\rundir\Release\obs-plugins\64bit"
$build_dir="./" + $project + ".v" + $version + "." + $arch
$uiohook="../libuiohook/bin"
$zip="C:/Program Files/7-Zip/7z.exe"
Set-Alias sz $zip

function replace($file, $what, $with)
{
    ((Get-Content $file) -replace $what, $with) | Set-Content -Path $file
}

if ($version.length -lt 1) {
    echo("Please provide a version string")
    exit
}

echo("Creating build directory")
New-Item $build_dir/plugin/data/obs-plugins/input-overlay -itemtype directory
New-Item $build_dir/plugin/obs-plugins/32bit -itemtype directory
New-Item $build_dir/plugin/obs-plugins/64bit -itemtype directory

echo("Fetching build from $build_location_x32")
Copy-Item $build_location_x32/$project.dll -Destination $build_dir/plugin/obs-plugins/32bit/


echo("Fetching build from $build_location_x64")
Copy-Item $build_location_x64/$project.dll -Destination $build_dir/plugin/obs-plugins/64bit/

echo("Fetching dependencies from $uiohook")
Copy-Item $uiohook/win64/uiohook.dll -Destination $build_dir/plugin/obs-plugins/64bit/
Copy-Item $uiohook/win32/uiohook.dll -Destination $build_dir/plugin/obs-plugins/32bit/

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
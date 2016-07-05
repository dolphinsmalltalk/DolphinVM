# DolphinVM

[![Build status](https://ci.appveyor.com/api/projects/status/6ru55e8y9huog899/branch/master?svg=true)](https://ci.appveyor.com/project/dolphinsmalltalk/dolphinvm-nf68c/branch/master)

The [Dolphin Smalltalk](http://object-arts.com/) virtual machine. 

This is a VS2015 Solution to build the Virtual Machine (VM) elements of Dolphin Smalltalk. This is not required to run the Dolphin IDE, which is available in the [Dolphin](https://github.com/objectarts/Dolphin) GitHub repository, since the latter comes with the executables already pre-built. You'll only need to build DolphinVM if you need to explicitly change the VM itself. 

Note: if you are just looking to install Dolphin and get going as quickly as possible you may prefer to start with the release build and and follow the directions [here](http://www.object-arts.com/gettingstarted.html). That way you won't have to sully yourself with either the Dolphin or the DolphinVM repositories. However, if you really want to build the Dolphin VM from scratch, read on.

## Building the Virtual Machine

* First clone the [Dolphin](https://github.com/dolphinsmalltalk/Dolphin) repo to a `\Dolphin` directory on your machine. It can actually be any location but for convenience we'll call it `\Dolphin`.

* You should clone this repository (DolphinVM) into a `DolphinVM\` subdirectory of `\Dolphin\Core\`.

* Install VS2015 Community Edition on your machne. You can use the Pro edition if you have it. Later versions (when they appear) may work too but we no longer support VS2013, sorry. The Dolphin VM is a set of C++ projects so make sure to install this option (it's not the default) or you'll end up only being able to compile C#.

* Load the DolphinVM solution into Visual Studio. Choose the **Release** profile (**Debug** will compile but will run slowly) and then _Build Solution_. A bunch of DLLs and `Dolphin7.exe` will have been copied to the `\Dolphin` root folder.

Now go to the Dolphin [Getting Started](http://www.object-arts.com/gettingstarted.html) page and follow the instructions there to build the image file with your new VM. You can then launch Dolphin and continue with the tutorials.

## Releasing the VM

If sufficient changes have been made to the VM such that a new release is warranted, you should push a new tag of the form _v.7.x.y.z_ (eg: **v7.0.2.1**). When the tag is eventually pushed to the GitHub master branch this will cause AppVeyor to build and generate a new [Release](https://github.com/dolphinsmalltalk/DolphinVM/releases). Each release consists of the full set of VM binaries wrapped up as a zip called `DolphinVM.zip`. The VM binaries are not (now) included in the Dolphin repository but can be fetched by executing the `FetchVM.CMD` script from the image directory. Whenever, a new VM is released the version tag should be edited into `FetchVM.ps1` to make sure that the images will be booted with the new version.

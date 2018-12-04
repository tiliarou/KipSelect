# Kip Select
![screenshot](https://github.com/Sciguy429/KipSelect/raw/master/icon/ExampleScreenshot.jpg)
This is a WIP kip manager for atmosphere's fusée launcher, this homebrew will not work with Kosmos, or any other pack that boots from hekate, while it will run it wont do anything as hekate controls what kips load in those packs. This hombrew is intended only for use with 'vanilla' atmosphere, luanched with fusée.

## How To Use
This howmebrew enables and disables kips by moving them between the /atmosphere/kips/ folder and an added folder called /atmosphere/kips_disabled/. Since fusée only checks the kips folder on boot, anything in kips_disabled gets ignored. The homebrew automatically creates the required folders (/atmosphere/kips/ & /atmosphere/kips_disabled/) if they are missing, all you need to do is make sure your kips are in one of the two folders before launching. The .nro has no other special requirements and should be launchable from anywhere (although simply placing it in /switch/ is recommended).

## Notes
* This homebrew only supports a maximum of 32 kips, attempting to use more will result in a error and possibly even a console hard crash.
* Try and name your kips something reasonable, the program should be able to work with any file name the sdcard fs supports but naming your kip with emoji's isn't going to help.
* I have no idea how safe this is to use on exFat, it shouldn't corrupt anything as the only write operation it uses is rename(), but should isn't won't, please be careful.

## Credits
* [libnx](https://github.com/switchbrew/libnx)
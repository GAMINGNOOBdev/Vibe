# Vibe
This is a psp rhythm game project that randomly came into my mind while i was pressing random buttons (and triggers) to the music.

## Currently working parts of the game
- Actual hit inputs & judgement
- Proper hitobject positioning/movment
- Scroll velocity application
- Actual accuracy&scoring&combo gaming
- Actual results screen

## Work in progress areas of the project
- Ingame pause menu (perhaps, idk, maybe)
- Replays/Saving scores (and displaying best)
- Health system
- Combo effecting accuracy and score
- Text rendering system improvements (batch rendering perhaps?)
- Scroll direction change (optional setting) + judgement line
- Performance optimizations for running the game at default clock speed on psp
- Better emulation of SV changes in osu!mania
- Actual hit effects
- Better LN Skinning
- Skin system

## Known issues
- Audio engine on PC is borked (very experimental, 100% needs rewrite) ((maybe a custom library perhaps with openal/psp compat...  :3))

## Cancelled features
- Hitsounds (cannot get working consistently across psp and desktop, openal causes too many issues for no apparent reason)

## Default Keybinds
The default keybinds on the "desktop" version are as follows:
| Bind Name | Key |
|-----------|-----|
|Confirm|Z|
|Start|Enter|
|Select|Space|
|Back|Escape|
You can change these keybinds in the settings.
The non-modifyable keys are the "shoulder buttons" and the directional buttons (mapped to arrow keys).

## How to build
Ensure you have the [pspdev toolchain](https://pspdev.github.io/installation.html) installed.<br>
> Currently building only works for the psp and linux/macos (no windows support because f*ck microsoft).
After that just run
```bash
bash compile.sh
```
You should see a `build` folder appear, inside will be a folder called `psp` and a folder called `desktop`.
Both folders will contain the assets copied over, and should be run from within the build directory.
> [!NOTE]
> `compile.sh` clean builds the entire project

## How to game
Set your preferred keybinds, volumes and scroll speed in the settings menu while on the settings screen.
After that (if you have any beatmaps added) select any beatmap, then a difficulty and start <b>gaming.</b>
<br/>
And most importantly....<br/>
<b>H A V E   F U N</b>

## How to add beatmaps
Currently just create a `Songs` folder at the root of the repo and extract and put .osz files into it.<br>
Ensure that you extract to a folder with the name of the osz file.<br>
Folders have to be in the format `<id> <artist> - <title>` in order to be detected by the game.<br>
It should theoretically be possible to just copy your osu beatmaps folder into this game.<br>
If you are using the desktop version, you can drag and drop the .osz files onto the game window to add them to the game.

## How to contribute
message me on my [discord server](https://discord.gg/8R5YyM3MEn) (or dm me: `__bocchi_chan__`) and suggest stuff
or make a pull request/issue and tell me stuff

# Vibe
This is a psp rhythm game project that randomly came into my mind while i was pressing random buttons (and triggers) to the music.

## Download
Make sure you use linux or windows.
Fuck you, mac for not building properly, idrc tbh to fix it.
Then just check the [actions](https://github.com/GAMINGNOOBdev/Vibe/actions) of this repo and just download the latest builds from there.

## Currently working parts of the game
- Actual hit inputs & judgement
- Proper hitobject positioning/movment
- Scroll velocity application
- Actual accuracy&scoring&combo gaming
- Actual results screen
- Ingame pause menu
- Replay and score saving
- Displaying replays and scores
- Playing replays

## Work in progress areas of the project
- Optimizing textures/skins (and added skin template/sheet for how skin image files should be layed out)
- Better input and visuals/skins on desktop
- Better font
- Fixing inaccuracy of replay playback
- displaying options when pressing select on beatmap mayhaps?
- Health system
- Key overlay
- Text rendering system improvements (batch rendering perhaps?)
- Combo effecting accuracy and score
- Beatmap Search?
- Star rating display
- Scroll direction change (optional setting) + judgement line
- Performance optimizations for running the game at default clock speed on psp
- Better emulation of SV changes in osu!mania
- Actual hit effects
- Better LN Skinning
- Skin system
- Mods (very unlikely with current audio engine and psp hardware limitations)

## Known issues
- Audio engine on PC is borked for hitsounds, psp likely too. No real fix found so far (i tried :/)

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
The non-modifyable keys are the emulated "shoulder buttons" and the directional buttons (mapped to arrow keys).

## How to build
To build for linux just run
```bash
bash compile.sh
```

Now ensure you have the [pspdev toolchain](https://pspdev.github.io/installation.html) installed, and then proceed to build the project for the psp by running:
```bash
bash compile.sh psp
```

You should now see a `build` folder appear.<br/>
Inside it will be a folder called `psp` and a folder called `desktop`.<br/>
Both folders will contain the assets and the game should be run from within the respective target build directory.
> [!NOTE]
> `compile.sh` clean builds the entire project

If you are using windows:<br/>
a) why.<br/>
b) make a build directory, open a command promp in that directory, run `cmake ..` then `cmake --build .` and enjoy your game executable.<br/>

If you are using macos:<br/>
¯\\\_(ツ)\_/¯

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
message me on discord (`__bocchi_chan__`) or [twitter](https://x.com/gamingnoobdev) and suggest stuff
or make a pull request/issue and tell me stuff

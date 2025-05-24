# Vibe
This is a psp rhythm game project that randomly came into my mind while i was pressing random buttons (and triggers) to the music.

## How to build
Ensure you have the [pspdev toolchain](https://pspdev.github.io/installation.html) installed.<br>
After that just run
```bash
bash compile.sh
```
> [!NOTE]
> `compile.sh` clean builds the entire project for the psp

If you want to build for linux/macos (no windows support because f*ck microsoft) run
```bash
make -f MakefilePC
```

## How to game
Set your preferred keybinds, volumes and scroll speed in the settings menu while on the settings screen.
After that (if you have any beatmaps added) select any beatmap, then a difficulty and start <b>gaming.</b>
<br/>
And most importantly....<br/>
<b>H A V E   F U N</b>

## How to add beatmaps
Currently just create a `Songs` folder at the root of the repo and extract and put .osz files into it.<br>
Ensure that you extract to a folder with the name of the osz file.<br>
Folders have to be in the format `<id> <artist> - <title>` in order to be detected by the game.
It should theoretically be possible to just copy your osu beatmaps folder into this game.

## How to contribute
message me on my [discord](https://discord.gg/8R5YyM3MEn) and suggest stuff
or make a pull request/issue and tell me stuff

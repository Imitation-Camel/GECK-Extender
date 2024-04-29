# GECK Extender

The GECK Extender is an [NVSE](https://github.com/xNVSE/NVSE) plugin that 
enhances the [GECK](https://geckwiki.com/index.php?title=Garden_of_Eden_Creation_Kit) by fixing various bugs and adding new tools.

## Download
[Nexus Mods (Fallout New Vegas)](https://www.nexusmods.com/newvegas/mods/64888)


## Build

### Dependencies
- [CrashRpt](https://crashrpt.sourceforge.net/)
- [DirectX 9 SDK](https://www.microsoft.com/en-us/download/details.aspx?id=6812)
- [NVSE](https://github.com/xNVSE/NVSE)
- [SME Sundries](https://github.com/shadeMe/SME-Sundries)

Look into the `EnvVars.prop` property sheet to see how the required environment variables are set up.
The artifacts in the `[Depends]` folder should be copied (or symlinked) to the root game directory. The
`BuildIncrement.jse` file from SME Sundries is to be copied into the working directory and executed manually
once to generate the `BuildInfo.h` file.

### Directory Structure
- Root
	- NVSE
		- Contents of the NVSE repo go here.
	- Geck Extender
		- Contents of this repo go here.

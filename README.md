# v2s-player
A searchable media player uses VLC media player libraires

### Build
To succesfully build this project you need the following:
1. Qt 5
2. VLC installed on your computer with the SDK.

Important build notes:
1. Edit the MediaPlayer.pro file, set the path to VLC include and lib folders.
2. Add VLC's plugins folder to system path or next to the built binary.
3. Add libvlc.dll, libvlccore.dll in the same build path

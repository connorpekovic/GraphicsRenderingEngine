# This is a test/example rd file

Display "Double Cube"  "PNM" "rgb"
Format 640 480

FrameBegin 1

CameraEye  0 0 3
CameraAt   0 0 -1
CameraUp   0 1 0 

WorldBegin

Scale 1.2 1.2 1.2

Color 1.0 0.5 0.7

Cube

Scale 0.8 0.8 0.8
Color 0.5 1.0 0.7

Cube
WorldEnd
FrameEnd







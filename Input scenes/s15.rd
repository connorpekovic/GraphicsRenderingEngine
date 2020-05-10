# This is a test/example rd file

Display "Wireframe Cube"  "PNM" "rgb"
Format 640 480

##

FrameBegin 1

CameraEye  0 0 2
CameraAt   0 0 -1
CameraUp   0 1 0 

WorldBegin

#Scale 1.2 1.2 1.2

Color 1.0 0.5 0.7

Cube

WorldEnd
FrameEnd




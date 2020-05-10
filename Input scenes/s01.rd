# This is a test/example rd file

Display "Point Test"  "Screen" "rgbsingle"

# Testing default size, background color and foreground color for single point

# The next three lines will be ignored by your renderer
CameraEye 320 240 -240
CameraAt  320 240 0
CameraUp  0.0 -1.0 0.0

FrameBegin 1
WorldBegin

#50 added to test point pipeline
Point 320 240 50

WorldEnd
FrameEnd

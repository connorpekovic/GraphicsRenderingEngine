# This is a test/example rd file

Display "PersonalScene" "PNM" "rgb"
Format 500 500 

# Testing filling at edges

Background .5 .4 1

# The next three lines will be ignored by your renderer
CameraEye 200 200 -200
CameraAt  200 200 0
CameraUp  0.0 -1.0 0.0

FrameBegin 1

WorldBegin

#Circles 

Color 1 1 1

#The box
Line 100 100 0 100 300 0
Line 100 300 0 300 300 0
Line 300 300 0 300 100 0
Line 301 100 0 100 100 0

#The box
Line 200 200 0 200 400 0
Line 200 400 0 400 400 0
Line 400 400 0 400 200 0
Line 401 200 0 200 200 0

#The Connectors
Line 300 300 0 400 400 0
#Line 100 300 0 200 400 0
Line 100 100 0 200 200 0
#Line 300 100 0 400 200 0

#The Fills

#Sand
Color .8 .8 .2
Fill 350 250 0
Fill 160 230 0

#Darker gold
Color .5 .5 .1
Fill 250 250 0

#Pinkish tan
Color 1 .9 .7
Fill 240 150 0
Fill 105 105 0
Fill 203 303 0
Fill 338 382 0
Fill 388 397 0

WorldEnd
FrameEnd

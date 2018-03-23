#!/bin/bash

# get rid of old output
#rm -f combined.png
#rm -rf ./out
#rm -rf ./out16
#mkdir -p out
#mkdir -p out16

# download video
#youtube-dl oe87rrr8rO0 -o video.mp4

# extract the audio
ffmpeg -y -ss 00:00:00.200 -i video.mp4 -to 00:01:35.000 \
	-acodec pcm_s8 -f s8 -ac 1 -ar 22050 audio.pcm

# resize & split the video
#ffmpeg -ss 00:00:02.000 -i video.mp4 -to 00:01:34.000 \
#	-vf fps=12,scale=128x96 out/%04d.png

# combine into one big vertically stacked image
#montage -geometry +0+0 out/*.png -tile 8x142 combined.png

# generate the best common 16 color palette
#mogrify -path out16 out/*.png -remap ../colormap.png -colors 16 +dither

#convert combined.png +dither -remap ../colormap.png -colors 16 combined_16.png

# Finally split them all back out -- retaining the 16 color palette
#convert combined_16.png -crop 128x96 out16/%04d.png

#!/bin/bash

# get rid of old output
rm -f audio.wav
rm -f combined.png
rm -f combined_16.png
rm -rf ./out
rm -rf ./out16
mkdir -p out
mkdir -p out16

# download video
#youtube-dl oe87rrr8rO0 -o video.mp4

# extract the audio
ffmpeg -i video.mp4 -acodec pcm_u8 -ac 1 -ar 14000 audio.wav

# resize & split the video
ffmpeg -ss 00:00:02.000 -i video.mp4 -to 00:01:34.200 \
	-vf fps=15,scale=144x112 out/%04d.png

# combine into one big vertically stacked image
montage -geometry +0+0 out/*.png -tile 33x43 combined.png

# generate the best common 16 color palette
#mogrify -path out16 out/*.png -remap ../colormap.png -colors 16 +dither
convert combined.png -remap ../colormap.png -colors 16 +dither combined_16.png

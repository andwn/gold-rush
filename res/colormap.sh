convert -size 32x16 xc: \
	-channel R colormap.txt -fx "v.p{i&7,0}/47*255" \
	-channel G colormap.txt -fx "v.p{j&7,0}/47*255" \
	-channel B colormap.txt -fx "v.p{(i>>3)|((j>>3)<<2),0}/47*255" \
	-scale 600%% -filter point colormap.png

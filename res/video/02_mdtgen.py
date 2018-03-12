import os, os.path

def bytes_to_int(bytes):
    result = 0
    for b in bytes:
        result = (result << 8) + int(b)
    return result

def gen_mdt(fn):
	with open(fn, "rb") as f:
		f.read(16)
		width = bytes_to_int(f.read(4)) >> 3
		height = bytes_to_int(f.read(4)) >> 3
	outfn = fn[:-4] + ".mdt"
	with open(outfn, "w") as f:
		f.write("input " + fn + "\n")
		f.write("output " + fn[:-4] + ".pat" + "\n")
		f.write("tiles 0 0 " + str(width) + " " + str(height) + "\n")

if __name__ == '__main__':
	num_frames = 0
	folder = "out16/"
	# Make a .res file for rescomp
	res_pat = open("video.res", "w")
	# And a C array for all the frames
	res_arr = open("video_arr.c", "w")
	res_arr.write("#include \"common.h\"" + "\n")
	res_arr.write("#include \"video_arr.h\"" + "\n")
	res_arr.write("\n")
	res_arr.write("const uint32_t* frames[NUM_FRAMES] = {" + "\n")
	# Need to sort the file listing before iterating
	file_list = os.listdir(folder)
	file_list.sort()
	for fn in file_list:
		if fn.endswith(".png"):
			gen_mdt(folder + fn)
			res_pat.write("BIN PAT_" + fn[:-4] + " \"" + folder + fn[:-4] + ".cpat\"" + "\n")
			res_arr.write("\t" + "(const uint32_t*) " + "PAT_" + fn[:-4] + "," + "\n")
			num_frames += 1
	# Close the res files
	res_arr.write("};" + "\n")
	res_pat.close()
	res_arr.close()
	# Write a C header so the array can be accessed
	with open("video_arr.h", "w") as f:
		f.write("#define NUM_FRAMES " + str(num_frames) + "\n")
		f.write("\n")
		f.write("extern const uint32_t* frames[NUM_FRAMES];" + "\n")
	

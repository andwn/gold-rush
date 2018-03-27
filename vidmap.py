print('{{')

for y in range(14):
	print('\t{ ', end='')
	for x in range(18):
		print(hex(0x4100 + x + y * 18) +  ', ', end='')
	print(' },')

print('},{')

for y in range(14):
	print('\t{ ', end='')
	for x in range(18):
		print(hex(0x6200 + x + y * 18) +  ', ', end='')
	print(' },')

print('}};')

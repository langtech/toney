import sys

L = []
flag = False
for line in open(sys.argv[1]):
    if 'name = "target"' in line:
        flag = True
    elif 'item [' in line:
        flag = False
    elif flag:
        if 'xmin' in line or 'xmax' in line or 'text' in line:
            L.append(line.strip().split()[-1])

for i in range(2, len(L), 3):
    if L[i+2] != '""':
        print L[i], L[i+1], L[i+2]
                

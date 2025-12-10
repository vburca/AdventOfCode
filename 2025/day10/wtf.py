TEST = 0
import collections

path = "D:\\Programming\\AdventOfCode\\2025\\day10\\test2.in"

file = open(path)
ret = 0
coords = []


for line in file.readlines():
    parts = line.strip().split(' ')
    lights = parts[0]
    wiring = parts[1:-1]
    joltage = parts[-1]
    lights = [x == '#' for x in lights[1:-1]]
    n = len(lights)
    start = []
    for i in range(n):
        if lights[i]:
            start.append(i)

    def to_bitmask(x):
        y = 0
        for i in x:
            y += 2 ** i
        return y

    def from_bitmask(x):
        y = []
        for i in range(n):
            if x % 2 == 1:
                y.append(i)
            x //= 2
        return y

    buttons = [[int(x) for x in wire[1:-1].split(',')] for wire in wiring]

    start = to_bitmask(start)
    buttons = [to_bitmask(x) for x in buttons]
    # end = to_bitmask(list(range(n)))
    end = 0

    print(start, buttons, end)

    current = {start}
    iterations = 0
    while end not in current:
        nx = set()
        for c in current:
            for button in buttons:
                nx.add(c ^ button)
                # print(c, button, c ^ button)
                # print(from_bitmask(c), from_bitmask(button), from_bitmask(c ^ button))
        current = nx
        # print(current)
        iterations += 1
        # if iterations == 4:
        #     break
    print(iterations)
    ret += iterations

print(ret)
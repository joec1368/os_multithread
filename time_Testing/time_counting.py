from sys import stdin
 
full_lines = stdin.read().split('\n')
 
p0_sum = 0.0
p0_count = 0
for line in full_lines:
    if line.startswith('time'):
        time = float(line.split(' ')[-1])
        p0_sum += time
        p0_count += 1
  
print(f"avg p0 time: {p0_sum / p0_count}")
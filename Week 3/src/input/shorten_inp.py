# randomly shuffle and delete rows of "facebook_combined.txt"
# to create "facebook_combined_short.txt"
# 90% of the original rows are deleted

import random

with open("facebook_combined.txt", "r") as f:
    lines = f.readlines()
random.shuffle(lines)
# lines = lines[:int(len(lines) * 0.1)]
# constrain the number of unique nodes to 1000
lines = [line for line in lines if int(line.split()[0]) < 100 and int(line.split()[1]) < 100]
with open("facebook_combined_short.txt", "w") as f2:
    for line in lines:
        f2.write(line)
            
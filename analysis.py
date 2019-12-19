import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

tree = "Range"
bs = "Block_size/B"
bc = "Block_count"
init = "Oram_initialization_time/usec"
cm = "Client_memory/KiB"
ds = "Disk_storage/B"
rt = "Running_time_per_block/usec"

rand_rw = pd.read_table("log/rand_rw")
rand_r = pd.read_table("log/rand_r")
rand_w = pd.read_table("log/rand_w")

seq_rw = pd.read_table("log/seq_rw")
seq_r = pd.read_table("log/seq_r")
seq_w = pd.read_table("log/seq_w")

block_size = [4, 8, 16, 32, 64, 128, 256]
# Single
count = []
time = []
client_memory = []
disk_storage = []
init_time = []
for b in block_size:
    b_rand_rw = rand_rw[rand_rw[bs] == b]
    count.append(b_rand_rw[bc])
    time.append(b_rand_rw[rt])
    client_memory.append(b_rand_rw[cm])
    disk_storage.append(b_rand_rw[ds])
    init_time.append(b_rand_rw[init])

for i in range(7):
    plt.scatter(count[i], time[i], label='Block size %dB' % block_size[i], marker='x')
plt.xlabel('Number of blocks')
plt.ylabel('Time ber block/usec')
plt.title('Single ORAM Random Read-Write Runtime')
plt.legend()
plt.savefig('single_rand_rw_runtime.jpeg')
plt.close()

for i in range(7):
    plt.scatter(count[i], client_memory[i], label='Block size %dB' % block_size[i], marker='x')
plt.xlabel('Number of blocks')
plt.ylabel('Client memory/KiB')
plt.title('Single ORAM Random Read-Write Client Memory')
plt.legend()
plt.savefig('single_rand_rw_client_memory.jpeg')
plt.close()

for i in range(7):
    plt.scatter(count[i], disk_storage[i], label='Block size %dB' % block_size[i], marker='x')
plt.xlabel('Number of blocks')
plt.ylabel('Disk storage/B')
plt.title('Single ORAM Random Read-Write Disk Storage')
plt.legend()
plt.savefig('single_rand_rw_disk_storage.jpeg')
plt.close()

for i in range(7):
    plt.scatter(count[i], init_time[i], label='Block size %dB' % block_size[i], marker='x')
plt.xlabel('Number of blocks')
plt.ylabel('Initialization time/usec')
plt.title('Single ORAM Random Read-Write Initialization Time')
plt.legend()
plt.savefig('single_rand_rw_init_time.jpeg')
plt.close()

# Range
count = []
time = []
client_memory = []
disk_storage = []
init_time = []
for b in block_size:
    b_rand_r = rand_r[(rand_r[bs] == b) & (rand_r[tree] == 1)]
    count.append(b_rand_r[bc])
    time.append(b_rand_r[rt])
    client_memory.append(b_rand_r[cm])
    disk_storage.append(b_rand_r[ds])
    init_time.append(b_rand_r[init])

for i in range(7):
    plt.scatter(count[i], time[i], label='Block size %dB' % block_size[i], marker='x')
plt.xlabel('Number of blocks')
plt.ylabel('Time ber block/usec')
plt.title('Range ORAM Random Read Runtime')
plt.yscale('log')
plt.legend()
plt.savefig('range_rand_r_runtime.jpeg')
plt.close()

for i in range(7):
    plt.scatter(count[i], client_memory[i], label='Block size %dB' % block_size[i], marker='x')
plt.xlabel('Number of blocks')
plt.ylabel('Client memory/KiB')
plt.title('Range ORAM Random Read Client Memory')
plt.legend()
plt.savefig('range_rand_r_client_memory.jpeg')
plt.close()

for i in range(7):
    plt.scatter(count[i], disk_storage[i], label='Block size %dB' % block_size[i], marker='x')
plt.xlabel('Number of blocks')
plt.ylabel('Disk storage/B')
plt.title('Range ORAM Random Read Disk Storage')
plt.legend()
plt.savefig('range_rand_r_disk_storage.jpeg')
plt.close()

for i in range(7):
    plt.scatter(count[i], init_time[i], label='Block size %dB' % block_size[i], marker='x')
plt.xlabel('Number of blocks')
plt.ylabel('Initialization time/usec')
plt.title('Range ORAM Random Read Initialization Time')
plt.legend()
plt.savefig('range_rand_r_init_time.jpeg')
plt.close()

# compare across different access modes
rand_rw_4 = rand_rw[(rand_rw[bs] == 4) & (rand_rw[tree] == 0)]
rand_r_4 = rand_r[(rand_r[bs] == 4) & (rand_r[tree] == 0)]
rand_w_4 = rand_w[(rand_w[bs] == 4) & (rand_w[tree] == 0)]

seq_rw_4 = seq_rw[(seq_rw[bs] == 4) & (seq_rw[tree] == 0)]
seq_r_4 = seq_r[(seq_r[bs] == 4) & (seq_r[tree] == 0)]
seq_w_4 = seq_w[(seq_w[bs] == 4) & (seq_w[tree] == 0)]

plt.scatter(rand_rw_4[bc], rand_rw_4[rt], label='Random read-write', marker='x')
plt.scatter(rand_r_4[bc], rand_r_4[rt], label='Random read', marker='x')
plt.scatter(rand_w_4[bc], rand_w_4[rt], label='Random write', marker='x')

plt.scatter(seq_rw_4[bc], seq_rw_4[rt], label='Sequential read-write', marker='x')
plt.scatter(seq_r_4[bc], seq_r_4[rt], label='Sequential read', marker='x')
plt.scatter(seq_w_4[bc], seq_w_4[rt], label='Sequential write', marker='x')

plt.legend()
plt.xlabel('Block count')
plt.ylabel('Time per block/usec')
plt.title('Single ORAM 4B Block Runtime')
plt.savefig('single_4b_runtime.jpeg')
plt.close()

rand_r_4 = rand_r[(rand_r[bs] == 4) & (rand_r[tree] == 1)]
seq_r_4 = seq_r[(seq_r[bs] == 4) & (seq_r[tree] == 1)]

plt.scatter(rand_r_4[bc], rand_r_4[rt], label='Random read', marker='x')

plt.scatter(seq_r_4[bc], seq_r_4[rt], label='Sequential read', marker='x')

plt.legend()
plt.xlabel('Block count')
plt.ylabel('Time per block/usec')
plt.yscale('log')
plt.title('Range ORAM 4B Block Runtime')
plt.savefig('range_4b_runtime.jpeg')
plt.close()

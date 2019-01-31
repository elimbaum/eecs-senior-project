# gem5 tutorial - simple sytem config
# syscall mode

from argparse import ArgumentParser

parser = ArgumentParser("Hello world on a simple CPU with two-level cache")
parser.add_argument('--l1i_size', help="L1 instruction cache size")
parser.add_argument('--l1d_size', help="L1 data cache size")
parser.add_argument('--l2_size', help="unified L2 cache size")

args = parser.parse_args()

## start program ##

import m5
from m5.objects import *
from caches import *

system = System()

# create clock domain
system.clk_domain = SrcClockDomain()
system.clk_domain.clock = '1GHz'
system.clk_domain.voltage_domain = VoltageDomain() # default

# create memory
system.mem_mode = 'timing'
system.mem_ranges = [AddrRange('512MB')]

# cpu
# one ck/instr
system.cpu = TimingSimpleCPU()

# memory bus
system.membus = SystemXBar()

# connect caches
system.cpu.icache = L1ICache(args)
system.cpu.icache.connectCPU(system.cpu)

system.cpu.dcache = L1DCache(args)
system.cpu.dcache.connectCPU(system.cpu)

# L2
system.l2bus = L2XBar()

system.cpu.icache.connectBus(system.l2bus)
system.cpu.dcache.connectBus(system.l2bus)

# create
system.l2cache = L2Cache(args)
system.l2cache.connectCPUSideBus(system.l2bus)
system.l2cache.connectMemSideBus(system.membus)

# these membus connections are x86 requirements (may differ on other ISA)
# master/slave ports connect to each other (requests from master to slave)
system.cpu.createInterruptController()
system.cpu.interrupts[0].pio = system.membus.master
system.cpu.interrupts[0].int_master = system.membus.slave
system.cpu.interrupts[0].int_slave = system.membus.master

system.system_port = system.membus.slave

# memory control
system.mem_ctrl = DDR3_1600_8x8()
system.mem_ctrl.range = system.mem_ranges[0] # as specified above
system.mem_ctrl.port = system.membus.master # connect to bus

# using example for tutorial, but any statically-compiled x86 exec would do!
process = Process()
process.cmd = ['/home/fas/manohar/emb99/project/gem5/tests/test-progs/hello/bin/x86/linux/hello']
system.cpu.workload = process
system.cpu.createThreads()

# start it up!
root = Root(full_system = False, system = system)
m5.instantiate()

print("Beginning simulation!")
exit_event = m5.simulate()

print("Exiting @ tick {} because {}"
      .format(m5.curTick(), exit_event.getCause()))


# gem5 tutorial - simple sytem config
# syscall mode

import m5
from m5.objects import *

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

# connect cache
system.cpu.icache_port = system.membus.slave
system.cpu.dcache_port = system.membus.slave

# these membus connections are x86 requirements (may differ on other ISA)
# master/slave ports connect to each other (requests from master to slave)
system.cpu.createInterruptController()
system.cpu.interrupts[0].pio = system.membus.master
system.cpu.interrupts[0].int_master = system.membus.slave
system.cpu.interrupts[0].int_slave = system.membus.master

system.system_port = system.membus.slave

# memory control
system.mem_ctrl = DDR3_2133_8x8()
system.mem_ctrl.range = system.mem_ranges[0] # as specified above
system.mem_ctrl.port = system.membus.master # connect to bus

# using example for tutorial, but any statically-compiled x86 exec would do!
process = Process()
# process.cmd = ['/home/eli/gem5/tests/test-progs/hello/bin/x86/linux/hello']
# process.cmd = ['sieve', '10000']
process.cmd = ['memtest', '0x7fffffffed03']
system.cpu.workload = process
system.cpu.createThreads()

# start it up!
root = Root(full_system = False, system = system)
m5.instantiate()

print("Beginning simulation!")
exit_event = m5.simulate()

print("Exiting @ tick {} because {}"
      .format(m5.curTick(), exit_event.getCause()))


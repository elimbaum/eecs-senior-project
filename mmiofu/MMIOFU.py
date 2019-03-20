from m5.params import *
from m5.proxy import *
from MemObject import MemObject

class MMIOFU(MemObject):
    type = 'MMIOFU'
    cxx_header = "mmiofu/mmio_fu.hh"

    cpu_side = SlavePort("CPU side port")
    mem_side = MasterPort("Memory side port")

#include "mmiofu/mmio_fu.hh"
#include "debug/MMIOFU.hh"

#include "blas_operation.h"

extern blasop operations[];

MMIOFU::MMIOFU(MMIOFUParams * params) :
  MemObject(params),
  cpuPort(params->name + ".cpu_side", this),
  memPort(params->name + ".mem_side", this),
  blocked(false)
{
  DPRINTF(MMIOFU, "MMIOFU init\n");
}

BaseMasterPort&
MMIOFU::getMasterPort(const std::string& if_name, PortID idx)
{
  panic_if(idx != InvalidPortID, "This object doesn't support vector ports.");

  if (if_name == "mem_side") {
    return memPort;
  } else {
    return MemObject::getMasterPort(if_name, idx);
  }
}

BaseSlavePort&
MMIOFU::getSlavePort(const std::string& if_name, PortID idx)
{
  panic_if(idx != InvalidPortID, "This object doesn't support vector ports.");

  if (if_name == "cpu_side") {
    return cpuPort;
  } else {
    return MemObject::getSlavePort(if_name, idx);
  }
}

void
MMIOFU::CPUSidePort::sendPacket(PacketPtr pkt)
{
  panic_if(blockedPacket != nullptr, "Should never try to send if blocked!");

  DPRINTF(MMIOFU, "Sending %s to CPU\n", pkt->print());
  if (! sendTimingResp(pkt)) {
    blockedPacket = pkt;
    DPRINTF(MMIOFU, "Couldn't send\n");
  } else {
    // DPRINTF(MMIOFU, "Sent!\n");
  }
}

AddrRangeList
MMIOFU::CPUSidePort::getAddrRanges() const
{
  return owner->getAddrRanges();
}

void
MMIOFU::CPUSidePort::trySendRetry()
{
  if (needRetry && blockedPacket == nullptr) {
    needRetry = false;
    DPRINTF(MMIOFU, "Sending retry req for %d\n", id);
    sendRetryReq();
  }
}

void
MMIOFU::CPUSidePort::recvFunctional(PacketPtr pkt)
{
  return owner->handleFunctional(pkt);
}

bool
MMIOFU::CPUSidePort::recvTimingReq(PacketPtr pkt)
{
  if (! owner->handleRequest(pkt)) {
    needRetry = true;
    return false;
  } else {
    return true;
  }
}

void
MMIOFU::CPUSidePort::recvRespRetry()
{
  assert(blockedPacket != nullptr);
  PacketPtr pkt = blockedPacket;
  blockedPacket = nullptr;

  sendPacket(pkt);
}

void
MMIOFU::MemSidePort::sendPacket(PacketPtr pkt)
{
  panic_if(blockedPacket != nullptr, "Should never try to send if blocked!");

  if (! sendTimingReq(pkt)) {
    blockedPacket = pkt;
  }
}

bool
MMIOFU::MemSidePort::recvTimingResp(PacketPtr pkt)
{
  return owner->handleResponse(pkt);
}

void
MMIOFU::MemSidePort::recvReqRetry()
{
  assert(blockedPacket != nullptr);

  PacketPtr pkt = blockedPacket;
  blockedPacket = nullptr;

  sendPacket(pkt);
}

void
MMIOFU::MemSidePort::recvRangeChange()
{
  owner->sendRangeChange();
}

bool
MMIOFU::handleRequest(PacketPtr pkt)
{
  if (blocked) {
    return false;
  }

  // DPRINTF(MMIOFU, "Request for addr  %#x\n", pkt->getAddr());
  blocked = true;

  // all communication operations take 1 cycle except for function execute
  int latency = 1;
  
  // do computation here
  auto addr = pkt->getAddr();
  switch (addr) {
    case GET_ADDR(IDX_FUNCTION):
      {
        if (pkt->isWrite()) {
          double _func_idx;
          pkt->writeDataToBlock((uint8_t *)&_func_idx, (int)sizeof(double));
          int func_idx = (int)_func_idx;
          DPRINTF(MMIOFU, "Request for function %d\n", func_idx);
         
          panic_if(func_idx >= NUM_FUNCS, "Invalid function number");
          op = operations[(int) func_idx];
          DPRINTF(MMIOFU, "... %s\n", op.cblas_name.c_str());

          // if function returns, return value. otherwise ignore it (probably garbage)
          // TODO: is this ok? segfault risk?
          // can't actually do this: responding to a WRITE. needs to be next call, or put it in alpha.
          // TODO: need distinction between READ and WRITE for alpha/X/Y; maybe all.
          if (op.returns) {
            _alpha = op.func(_N, _alpha, _X, _Y, &latency);
            DPRINTF(MMIOFU, "ret: %d\n", _alpha);
          } else {
            op.func(_N, _alpha, _X, _Y, &latency);
          }
          DPRINTF(MMIOFU, "latency: %d\n", latency);
        } else if (pkt->isRead()) {
          DPRINTF(MMIOFU, "cannot read function index");
        }
      }
      break;

    case GET_ADDR(IDX_ALPHA):
      if (pkt->isWrite()) {
        // TODO these are communication latencies, not register latencies.
        // how to represent?
        pkt->writeDataToBlock((uint8_t *)&_alpha, (int)sizeof(double));
        DPRINTF(MMIOFU, "  Received alpha: %f\n", _alpha);
      } else if (pkt->isRead()) {
        pkt->setDataFromBlock((uint8_t *)&_alpha, (int)sizeof(double));
        DPRINTF(MMIOFU, "  Sent alpha: %f\n", _alpha);
      }
      break;

    case GET_ADDR(IDX_N):
      double _dN;
      pkt->writeDataToBlock((uint8_t *)&_dN, (int)sizeof(double));
      _N = (int)_dN;
      DPRINTF(MMIOFU, "  Setting N %d (new size: %d)\n", _N, _N * sizeof(double));
      _X = (double *)realloc(_X, _N * sizeof(double));
      _Y = (double *)realloc(_Y, _N * sizeof(double));

      // TODO bounds check on N
      break;

    default:
      // invalid, or X or Y
      uint8_t * addr_p;
      if (GET_INDEX(addr) < IDX_START_X + _N) {
        DPRINTF(MMIOFU, "X: %x => %d\n", addr, GET_INDEX(addr) - IDX_START_X);
        addr_p = (uint8_t *)&(_X[GET_INDEX(addr) - IDX_START_X]);
      } else if (GET_INDEX(addr) < IDX_START_X + 2 * _N) {
        DPRINTF(MMIOFU, "Y: %x => %d\n", addr, GET_INDEX(addr) - IDX_START_X - _N);
        addr_p = (uint8_t *)&(_Y[GET_INDEX(addr) - IDX_START_X - _N]);
      } else {
        panic("Invalid address");
      }

      if (pkt->isWrite()) {
        pkt->writeData(addr_p);
        DPRINTF(MMIOFU, "WRITE\n");
      } else if (pkt->isRead()) {
        pkt->setData(addr_p);
        DPRINTF(MMIOFU, "READ\n");
      } else {
        panic("Invalid packet type");
      }
      break;
  }
  
  pkt->makeResponse();

  // Schedule the response based on 
  schedule(new EventFunctionWrapper([this, pkt]{ sendResponse(pkt); },
                                    name() + ".responseEvent", true),
           clockEdge(Cycles(latency)));

  return true;
}

bool
MMIOFU::handleResponse(PacketPtr pkt)
{
  assert(blocked);
  DPRINTF(MMIOFU, "Response for addr %#x\n", pkt->getAddr());
  // blocked = false;

  // cpuPort.sendPacket(pkt);
  // cpuPort.trySendRetry(); // might need retry
  sendResponse(pkt);
  return true;
}

void
MMIOFU::sendResponse(PacketPtr pkt)
{
  assert(blocked);
  blocked = false;
  cpuPort.sendPacket(pkt);
  cpuPort.trySendRetry();
}

void
MMIOFU::handleFunctional(PacketPtr pkt)
{
  if (accessFunctional(pkt)) {
    pkt->makeResponse();
  } else {
    memPort.sendFunctional(pkt);
  }
}

bool
MMIOFU::accessFunctional(PacketPtr pkt)
{
  panic("Functional unimplemented!");
}

AddrRangeList
MMIOFU::getAddrRanges() const
{
  DPRINTF(MMIOFU, "Sending new ranges\n");
  return memPort.getAddrRanges();
}

void
MMIOFU::sendRangeChange()
{
  cpuPort.sendRangeChange();
}

MMIOFU * MMIOFUParams::create()
{
  return new MMIOFU(this);
}


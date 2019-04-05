#include "mmiofu/mmio_fu.hh"
#include "debug/MMIOFU.hh"

#include "blas_operation.h"

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
    DPRINTF(MMIOFU, "Sent!\n");
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

  DPRINTF(MMIOFU, "Request for addr  %#x\n", pkt->getAddr());
  blocked = true;
  
  // memPort.sendPacket(pkt);
  schedule(new EventFunctionWrapper([this, pkt]{ accessTiming(pkt); },
                                   name() + ".accessEvent", true),
           clockEdge(Cycles(0)));
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

#define BASE_ADDR 0xFFFEF000
double A, B;

void
MMIOFU::accessTiming(PacketPtr pkt)
{
  DPRINTF(MMIOFU, "AT / packet: %s\n", pkt->print());

  if (pkt->getAddr() == BASE_ADDR + 0 * sizeof(double)) {
    pkt->writeDataToBlock((uint8_t *) &A, (int)sizeof(double));
    DPRINTF(MMIOFU, "Got A: %d\n", A);
  } else if (pkt->getAddr() == BASE_ADDR + 1 * sizeof(double)) {
    pkt->writeDataToBlock((uint8_t *) &B, (int)sizeof(double));
    DPRINTF(MMIOFU, "Got B: %d\n", B);
  } else if (pkt->getAddr() == BASE_ADDR + 2 * sizeof(double)) {
    // do the computation!
    // TODO have some kind of flag so we don't accidentally return old results
    double C = sqrt(A * A + B * B);
    pkt->setDataFromBlock((uint8_t *) &C, (int)sizeof(double));
    DPRINTF(MMIOFU, "Sent C: %d\n", C);
  }
    
  pkt->makeResponse();
  sendResponse(pkt);
  DPRINTF(MMIOFU, "end of AT\n");
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


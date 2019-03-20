#include "mmiofu/mmio_fu.hh"
#include "debug/MMIOFU.hh"

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

  if (! sendTimingResp(pkt)) {
    blockedPacket = pkt;
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

  memPort.sendPacket(pkt);
  return true;
}

bool
MMIOFU::handleResponse(PacketPtr pkt)
{
  assert(blocked);
  DPRINTF(MMIOFU, "Response for addr %#x\n", pkt->getAddr());
  blocked = false;

  cpuPort.sendPacket(pkt);
  cpuPort.trySendRetry(); // might need retry

  return true;
}

void
MMIOFU::handleFunctional(PacketPtr pkt)
{
  memPort.sendFunctional(pkt);
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


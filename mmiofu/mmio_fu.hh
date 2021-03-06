#include "mem/mem_object.hh"
#include "params/MMIOFU.hh"

#include "blop.hh"

class MMIOFU : public MemObject
{
  private:
    class CPUSidePort : public SlavePort
    {
      private:
        MMIOFU * owner;
        bool needRetry;
        PacketPtr blockedPacket;

      public:
        CPUSidePort(const std::string& name, MMIOFU * owner) :
          SlavePort(name, owner), owner(owner), needRetry(false),
          blockedPacket(nullptr)
        { }

        void sendPacket(PacketPtr pkt);
        AddrRangeList getAddrRanges() const override;
        void trySendRetry();

      protected:
        Tick recvAtomic(PacketPtr pkt) override
        { panic("recvAtomic unimplemented"); }

        void recvFunctional(PacketPtr pkt) override;
        bool recvTimingReq(PacketPtr pkt) override;
        void recvRespRetry() override;
    };

    class MemSidePort : public MasterPort
    {
      private:
        MMIOFU * owner;
        PacketPtr blockedPacket;

      public:
        MemSidePort(const std::string& name, MMIOFU * owner) :
          MasterPort(name, owner), owner(owner), blockedPacket(nullptr)
        { }

        void sendPacket(PacketPtr pkt);

      protected:
        bool recvTimingResp(PacketPtr pkt) override;
        void recvReqRetry() override;
        void recvRangeChange() override;
    };

    bool handleRequest(PacketPtr pkt);
    bool handleResponse(PacketPtr pkt);
    void sendResponse(PacketPtr pkt);

    void handleFunctional(PacketPtr pkt);
    void accessTiming(PacketPtr pkt);
    bool accessFunctional(PacketPtr pkt);
    AddrRangeList getAddrRanges() const;
    void sendRangeChange();

    CPUSidePort cpuPort;
    MemSidePort memPort;

    bool blocked;

    BlasOperation * op;
    // TODO look in to this? might need more state variables
    bool dirty;

    int _N;
    double _alpha;
    // TODO these can be vectors, probably? then no need for alloc
    double * _X = nullptr;
    double * _Y = nullptr;

  public:
    MMIOFU(MMIOFUParams * params); 

    BaseMasterPort& getMasterPort(const std::string& if_name,
                                  PortID idx = InvalidPortID) override;

    BaseSlavePort& getSlavePort(const std::string& if_name,
                                PortID idx = InvalidPortID) override;
};

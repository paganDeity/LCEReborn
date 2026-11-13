#include "platform/pc/PcP2pStart.hpp"

#include "net/NetP2p.hpp"
#include "platform/pc/PcLaunchOptions.hpp"

void PcP2pStart()
{
    const PcLaunchOptions& options = PcGetLaunchOptions();
    if (options.p2pServer.empty() || options.p2pServerKey.empty() || options.p2pDir.empty())
        return;
    lcenet::P2pService::Get().Start(options.p2pServer, options.p2pServerKey, options.p2pDir);
}

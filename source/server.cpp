/**
**    Hatchit Engine
**    Copyright(c) 2015-2016 Third-Degree
**
**    GNU Lesser General Public License
**    This file may be used under the terms of the GNU Lesser
**    General Public License version 3 as published by the Free
**    Software Foundation and appearing in the file LICENSE.LGPLv3 included
**    in the packaging of this file. Please review the following information
**    to ensure the GNU Lesser General Public License requirements
**    will be met: https://www.gnu.org/licenses/lgpl.html
**
**/

#include <ht_network.h>
#include <ht_debug.h>
#include <ht_socketutil.h>
#include <cstdlib>
#include <cstring>
#include <thread>

using namespace Hatchit;
using namespace Hatchit::Core;
using namespace Hatchit::Network;

int main(int argc, char* argv[])
{
    HT_DEBUG_PRINTF("Server.\n");

    int result = std::atexit(Network::Shutdown);
    if (result != 0) {
#ifdef _DEBUG
        HT_DEBUG_PRINTF("atexit registration failed.\n");
#endif
        std::exit(EXIT_FAILURE);
    }

    Network::Initialize();
    
    if (argc < 2) {
        HT_ERROR_PRINTF("usage %s port\n", argv[0]);
        std::exit(EXIT_FAILURE);
    }

    TCPSocketPtr socket = SocketUtil::CreateTCPSocket(SocketAddressFamily::INET);
    
    int port = atoi(argv[1]);
    SocketAddressPtr address = std::make_shared<SocketAddress>(port);

    socket->Bind(*address);


    while (true)
    {
        socket->Listen();

        SocketAddressPtr clientAddress = std::make_shared<SocketAddress>();
        TCPSocketPtr client = socket->Accept(*clientAddress);
        if(client)
        {
            std::thread t([](TCPSocketPtr s){

                while (true)
                {
                    char buffer[256] = { 0 };
                    int n = s->Receive(buffer, 255);
                    if (n < 0 || strlen(buffer) <= 0)
                        std::exit(EXIT_FAILURE);

                    HT_DEBUG_PRINTF("Message: %s\n", buffer);

                    const char* msg = "Server received message";
                    n = s->Send(msg, static_cast<int>(strlen(msg)));
                    if (n < 0)
                        std::exit(EXIT_FAILURE);
                }

            }, client);
            t.detach();
        }
    }
   
    return 0;
}

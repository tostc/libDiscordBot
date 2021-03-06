#ifndef BOTCONTROLLER_HPP
#define BOTCONTROLLER_HPP

#include <controller/IController.hpp>

using namespace DiscordBot;

class CBotController : public IController
{
    public:
        CBotController(IDiscordClient *client) : IController(client) {}

        void OnReady() override;

        virtual ~CBotController() {}
};

#endif
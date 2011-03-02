#include "../include/Global.h"


void Global::set_BotNick(std::string _BotNick)
{
    m_BotNick = _BotNick;
}
std::string Global::get_BotNick()
{
    return m_BotNick;
}

void Global::set_Users(Users& _Users)
{
    m_Users = _Users;
}
Users& Global::get_Users()
{
    return m_Users;
}

void Global::set_Channels(Channels& _Channels)
{
    m_Channels = _Channels;
}
Channels& Global::get_Channels()
{
    return m_Channels;
}

void Global::set_IrcData(IrcData& _IrcData)
{
    //m_IrcData = _IrcData;
}
IrcData& Global::get_IrcData()
{
    return m_IrcData;
}

void Global::set_Reply(Reply& _Reply)
{
    m_Reply = _Reply;
}
Reply& Global::get_Reply()
{
    return m_Reply;
}

void Global::set_ConfigReader(ConfigReader& _ConfigReader)
{
    m_ConfigReader = _ConfigReader;
}
ConfigReader& Global::get_ConfigReader()
{
    return m_ConfigReader;
}

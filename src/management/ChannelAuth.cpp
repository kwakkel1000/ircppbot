#include "../include/management/ChannelAuth.h"

ChannelAuth::ChannelAuth()
{
	access = -1;
}

ChannelAuth::~ChannelAuth()
{
}

void ChannelAuth::SetAccess(int data)
{
	access = data;
}

int ChannelAuth::GetAccess()
{
	return access;
}

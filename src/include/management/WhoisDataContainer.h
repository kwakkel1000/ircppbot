//
//
//  @ Project : ircppbot
//  @ File Name : WhoisDataContainer.h
//  @ Date : 4/18/2011
//  @ Author : Gijs Kwakkel
//
//
// Copyright (c) 2011 Gijs Kwakkel
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Softwarea
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
//

#ifndef SRC_INCLUDE_WHOISDATACONTAINER_H_
#define SRC_INCLUDE_WHOISDATACONTAINER_H_

#include <boost/bind.hpp>
#include <boost/thread/condition.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/shared_ptr.hpp>
#include "../interfaces/WhoisDataContainerInterface.h"

#include <queue>
#include <string>
#include <map>

class WhoisDataContainer : public WhoisDataContainerInterface
{
    public:
        // functions
        WhoisDataContainer();
        ~WhoisDataContainer();
        void Init();
        void stop();

        void AddWhoisQueue(std::pair< std::string, std::string > data);
        std::pair< std::string, std::string > GetWhoisQueue();

    private:
        // vars
        bool mRun;

        // irc data queues
        std::multimap< std::string, std::string > WhoisQueue;

        // threadvars
        boost::condition WhoisAvailable;

        boost::mutex WhoisMutex;
};

#endif  // SRC_INCLUDE_WHOISDATACONTAINER_H_


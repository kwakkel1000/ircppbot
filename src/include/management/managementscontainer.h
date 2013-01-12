//
//
//  @ Project : ircppbot
//  @ File Name : managementscontainer.h
//  @ Date : 11-01-2013
//  @ Author : Gijs Kwakkel
//
//
// Copyright (c) 2013 Gijs Kwakkel
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
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
//


#ifndef SRC_INCLUDE_MANAGEMENT_MANAGEMENTSCONTAINER_H
#define SRC_INCLUDE_MANAGEMENT_MANAGEMENTSCONTAINER_H

#include <string>
#include <map>
#include <mutex>
#include <memory>

#include <gframe/output.h>

#include "channel.h"
#include "user.h"
#include "auth.h"

template <class managementstemplate>
class managementscontainer
{
    public:
        static managementscontainer& instance()
        {
            static managementscontainer obj;
            return obj;
        }
        std::shared_ptr< managementstemplate > add(std::string name);
        bool del(std::string name);
        bool rename(std::string oldName, std::string newName);

        bool find(std::string name);
        std::map< std::string, std::shared_ptr< managementstemplate > >& get();
        std::shared_ptr< managementstemplate > get(std::string name);


    private:
        managementscontainer();
        ~managementscontainer();

        typename std::map< std::string, std::shared_ptr< managementstemplate > > m_Objects;
        std::mutex m_managementscontainerMutex;
};


template <class managementstemplate>
managementscontainer<managementstemplate>::managementscontainer() :
m_Objects()
{
    m_Objects.clear();
}

template <class managementstemplate>
managementscontainer<managementstemplate>::~managementscontainer()
{
}

template <class managementstemplate>
std::shared_ptr< managementstemplate > managementscontainer<managementstemplate>::add(std::string name)
{
    std::lock_guard< std::mutex > lock(m_managementscontainerMutex);
    std::pair< typename std::map< std::string, std::shared_ptr< managementstemplate > >::iterator, bool > ret;
    ret = m_Objects.insert (std::pair< std::string, std::shared_ptr< managementstemplate > >(name, std::make_shared< managementstemplate >()));
    output::instance().addStatus(ret.second, "bool managementscontainer<" + std::string(typeid(managementstemplate).name()) + ">::add(std::string name) name added: " + name);
    return ret.first->second;
}


template <class managementstemplate>
bool managementscontainer<managementstemplate>::del(std::string name)
{
    std::lock_guard< std::mutex > lock(m_managementscontainerMutex);
    size_t ret = m_Objects.erase(name);
    if (ret == 1)
    {
        output::instance().addStatus(true, "bool managementscontainer<" + std::string(typeid(managementstemplate).name()) + ">::del(std::string name) name found, erase succesfull: " + name);
        return true;
    }
    output::instance().addStatus(false, "bool managementscontainer<" + std::string(typeid(managementstemplate).name()) + ">::del(std::string name) name found, erase failed: " + name);
    return false;
}


template <class managementstemplate>
bool managementscontainer<managementstemplate>::rename(std::string oldName, std::string newName)
{
    //std::lock_guard< std::mutex > lock(m_UsersMutex);
    std::pair< typename std::map< std::string, std::shared_ptr< managementstemplate > >::iterator, bool > ret;
    ret = m_Objects.insert (std::pair< std::string, std::shared_ptr< managementstemplate > >(newName, get(oldName)));
    if (ret.second)
    {
        del(oldName);
        return true;
    }
    output::instance().addStatus(false, "bool managementscontainer<" + std::string(typeid(managementstemplate).name()) + ">::rename(std::string oldName, std::string newName) newName already exists " + newName);
    return false;
}


template <class managementstemplate>
bool managementscontainer<managementstemplate>::find(std::string name)
{
    std::lock_guard< std::mutex > lock(m_managementscontainerMutex);
    typename std::map< std::string, std::shared_ptr< managementstemplate > >::iterator l_managementclassListIterator;
    l_managementclassListIterator = m_Objects.find(name);
    if (l_managementclassListIterator == m_Objects.end())
    {
        output::instance().addStatus(false, "bool managementscontainer<" + std::string(typeid(managementstemplate).name()) + ">::find(std::string name) name not found: " + name);
        return false;
    }
    return true;
}


template <class managementstemplate>
std::map< std::string, std::shared_ptr< managementstemplate > >& managementscontainer<managementstemplate>::get()
{
    std::lock_guard< std::mutex > lock(m_managementscontainerMutex);
    return m_Objects;
}


template <class managementstemplate>
std::shared_ptr< managementstemplate > managementscontainer<managementstemplate>::get(std::string name)
{
    std::lock_guard< std::mutex > lock(m_managementscontainerMutex);
    typename std::map< std::string, std::shared_ptr< managementstemplate > >::iterator l_managementclassListIterator;
    l_managementclassListIterator = m_Objects.find(name);
    if (l_managementclassListIterator == m_Objects.end())
    {
        output::instance().addStatus(false, "std::shared_ptr< " + std::string(typeid(managementstemplate).name()) + " > managementscontainer<managementstemplate>::get(std::string name) name not found " + name);
        return nullptr;
    }
    return (*l_managementclassListIterator).second;
}

class channels : public managementscontainer<channel>{};
class users : public managementscontainer<user>{};
class auths : public managementscontainer<auth>{};


#endif // SRC_INCLUDE_MANAGEMENT_MANAGEMENTSCONTAINER_H

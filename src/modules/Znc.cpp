#include "../../include/Znc.h"
#include <iostream>
#include <algorithm>
#include <iostream>
#include <fstream>
#include "../../include/Global.h"
#include <boost/algorithm/string.hpp>

extern "C" ModuleInterface* create()
{
    return new Znc;
}

extern "C" void destroy(ModuleInterface* x)
{
    delete x;
}

Znc::Znc()
{
}

Znc::~Znc()
{
    stop();
	Global::Instance().get_IrcData().DelConsumer(D);
    delete D;
}
void Znc::Init()
{
    D = new Data();
    D->Init(true, false, false, true);
    Global::Instance().get_IrcData().AddConsumer(D);
	ReadFile("znc.conf");
    timerlong();
}


void Znc::stop()
{
    run = false;
    D->stop();
    std::cout << "Znc::stop" << std::endl;
    raw_parse_thread->join();
    std::cout << "raw_parse_thread stopped" << std::endl;
    privmsg_parse_thread->join();
    std::cout << "privmsg_parse_thread stopped" << std::endl;
}

void Znc::read()
{
    run = true;
    assert(!raw_parse_thread);
    raw_parse_thread = boost::shared_ptr<boost::thread>(new boost::thread(boost::bind(&Znc::parse_raw, this)));
    assert(!privmsg_parse_thread);
    privmsg_parse_thread = boost::shared_ptr<boost::thread>(new boost::thread(boost::bind(&Znc::parse_privmsg, this)));
}

void Znc::parse_raw()
{
    std::vector< std::string > data;
    while(run)
    {
        data = D->GetRawQueue();
        ParseData(data);
    }
}

void Znc::parse_privmsg()
{
    /*std::vector< std::string > data;
    while(run)
    {
        data = D->GetPrivmsgQueue();
        //PRIVMSG(data, channelbottrigger);
    }*/
}


void Znc::ParseData(std::vector< std::string > data)
{
    /*cout << "Test" << endl;
    if (data.size() >= 1)
    {
		std::string returnstr = "PRIVMSG #blubs :" + data[0];
		if (data.size() >= 2)
		{
			for (unsigned int i = 1; i < data.size(); i++)
			{
				returnstr = returnstr + " " + data[i];
			}
		}
		returnstr = returnstr + "\r\n";
		SendLowPriority(returnstr);
    }*/
}


void Znc::ParsePrivmsg(std::vector<std::string> data, std::string command, std::string chan, std::vector< std::string > args, int chantrigger)
{
}


void Znc::timerrun()
{
    //cout << "channelbot::timerrun()" << endl;
    int Tijd;
    time_t t= time(0);
    Tijd = t;
    longtime++;
    if (longtime >= 100)
    {
        timerlong();
        longtime = 0;
    }
    for (int i = timer_sec.size() - 1; i >= 0; i--)
    {
        if (timer_sec[i] < Tijd)
        {
            cout << timer_command[i] << endl;
            timer_sec.erase(timer_sec.begin()+i);
            timer_command.erase(timer_command.begin()+i);
        }
    }
}

void Znc::timerlong()
{
    int Tijd;
    time_t t= time(0);
    Tijd = t;
    Tijd = Tijd + 100;
    for (int i = timer_long_sec.size() - 1; i >= 0; i--)
    {
        if (timer_long_sec[i] < Tijd)
        {
            cout << "timer_long to timer " << timer_long_command[i] << endl;
            timer_sec.push_back(timer_long_sec[i]);
            timer_command.push_back(timer_long_command[i]);
            timer_long_sec.erase(timer_long_sec.begin()+i);
            timer_long_command.erase(timer_long_command.begin()+i);
        }
    }
}


bool Znc::ReadFile( std::string filename )
{
    cout << "readfile: " << filename << endl;
	std::string tmp_user_str;
	bool in_user_section = false;
	std::map< std::string, std::string > setting;
    std::string line;
    ifstream configfile;
    int linenr = 0;

    // Start fresh :)
    setting.clear();
    znc_user_setting_map.clear();
    znc_user_nick.clear();

    configfile.open(filename.c_str());
    if (configfile.is_open())
    {
        while (configfile.good())
        {
            getline(configfile,line);
            linenr++;

            boost::trim(line);

            // Filter empty and comment lines
            if ((line.length()>0) && (line[0] != '#'))
            {
            	std::vector< std::string > vector_line;
				boost::split( vector_line, line, boost::is_any_of(" "), boost::token_compress_on );
                // Debug
                //cout << "Line " << linenr << ": " << line << endl;

                if (vector_line.size() == 2)
                {
					if (boost::iequals(vector_line[0], "<User"))
					{
                        tmp_user_str = vector_line[1];
                        boost::erase_all(tmp_user_str, ">");
						boost::trim(tmp_user_str);
						in_user_section = true;
						setting.clear();
					}
                }
                if (in_user_section)
                {
					std::vector< std::string > setting_vector;
					boost::split( setting_vector, line, boost::is_any_of("="), boost::token_compress_on );
					if (setting_vector.size() >= 2)
					{
						//std::cout << "line:" << line << std::endl;
						boost::trim(setting_vector[0]);
						boost::trim(setting_vector[1]);
						setting[setting_vector[0]] = setting_vector[1];
					}
                }

                if (vector_line.size() == 1)
                {
					if (boost::iequals(vector_line[0], "</User>"))
					{
						std::pair< std::map< std::string, std::map< std::string, std::string > >::iterator, bool > znc_user_setting_ret;
						znc_user_setting_ret = znc_user_setting_map.insert (pair< std::string, std::map< std::string, std::string > > (tmp_user_str, setting));
						if (znc_user_setting_ret.second == true)
						{
							std::cout << "new user: " << tmp_user_str << std::endl;
							znc_user_nick.push_back(tmp_user_str);
						}
						else
						{
							std::cout << "user more thene once in config :/" << std::endl;
						}
						in_user_section = false;
					}
                }
            }

        }
        configfile.close();
        for (unsigned int it_i = 0; it_i < znc_user_nick.size(); it_i++)
        {
			std::cout << "user " << znc_user_nick[it_i] << std::endl;
			std::cout << "value " << znc_user_setting_map[znc_user_nick[it_i]]["Pass"] << std::endl;
        }
        return true;
    }
    else
    {
        cout << "Could not open file '" << filename << "'" << endl;
    }

    return false;
}



#ifndef AppelBot_ChannelAuth_h
#define AppelBot_ChannelAuth_h

#include <string>
#include <vector>
#include <iostream>
#include <algorithm>

using namespace std;

//namespace Bot {

	class ChannelAuth {

		public:

					ChannelAuth();

					virtual ~ChannelAuth();

					virtual void SetAccess(int);

					virtual int GetAccess();

		private:

					bool caseInsensitiveStringCompare( const std::string& str1, const std::string& str2 );

                    int access;
	};

//} /* End of namespace Bot */

#endif // AppelBot_ChannelAuth_h

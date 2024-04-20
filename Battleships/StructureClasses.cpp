#include "StructureClasses.h"
#include <iostream>

using namespace std;


TimeManager::TimeManager() : time_lapsed(sf::Time::Zero)
{
	//cout << "a TimeManager created" << endl;
}


void TimeManager::setTimeChange(sf::Time new_time_lapsed)
{
	time_lapsed += new_time_lapsed;
	for (int i = 0; i < all_holders.size(); i++)
	{
		if (times_remaining[i] <= time_lapsed)
		{
			all_holders[i]->run();
			all_holders.erase(all_holders.begin() + i);
			times_remaining.erase(times_remaining.begin() + i);
		}
	}
}

TimeManager::~TimeManager()
{
	cout << "destroying a TimeManager" << endl;
}

							////////////////////////
							//ResourceHolder Class//
							////////////////////////




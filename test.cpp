/*
 * test.cpp
 *
 *  Created on: 2016年10月9日
 *      Author: havesnag
 */

#include <iostream>
using namespace std;

#include "src/date.h"
using namespace ec;

int main(int argc, char *argv[])
{
	Date d(2000, 1, 1);
	cout << "d = " << d.format() << endl;

	Time t = d.toTime().addHour(1);
	cout << "t = " << t.toDate().format() << endl;

	return 0;
}



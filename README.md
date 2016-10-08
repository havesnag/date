# overview
a c++ date class

the namespace is ec

```
using namespace ec;
```


include three class

- Duration:  a period of time
- Date: convenient for date of operation
- Time: the precise time

[The API Documentation](http://www.baiyy.com/public/project/ecdate/index.html)

# detail
Please directly add the source code to your project
 
The source code in the "src" directory.

example:

---

```
#include <iostream>
#include "date.h"

int main(int argc, char *argv[])
{
    // The current time
    ec::Time now;
    
    // 10 hours
    ec::Duration d(10, ec::Duration::Hour);
    
    // After 10 hours
    now += d;
    
    // output like 2016-01-01 12:00:00
    std::cout << now.toString() << std::endl;
    return 0;
}
```
## Duration

```
// 10 days
ec::Duration d0(10, ec::Duration::Day);

// 240 hours
ec::Duration d1 = d0.down();

// 864000 seconds
int64 seconds = d0.valueAs(ec::Duration::Second);

```

## Date

```
// 2016-01-01 00:00:00
ec::Date d0(2016, 1, 1, 0, 0, 0);

// 2016-01-02  00:00:00
d0.add(ec::Duration(1, ec::Duration::Hour);

// 2015-01-02  00:00:00
d0.setYear(2015);
```

## Time

```
// The current time 
ec::Time t0;

// 2016-01-01 00:00:00
ec::Time t1(ec::Date(2016, 1, 1));

// seconds
int64 seconds = t1.diff(t0, ec::Duration::Second);
```

# 中文简介
这是C++简单对时间操作的封装，命名空间为ec

包含三个类：
Duration: 时间段
Date: 日期类
Time: 时间类

由于代码比较简单，没有提供编辑成链接库的makefile，请直接将源代码（src目录 ）加入你的工程。


[完整API参考文档](http://www.baiyy.com/public/project/ecdate/index.html)
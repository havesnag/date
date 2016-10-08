# date
c++ date class

the namespace is ec::
include three class Duration Date Time

##example


	ec::Time now;
	cout << now.clone().zeroSet(ec::Duration::Year).toDate().format("%Y-%m-%d %H:%M:%S") << endl;


# date
c++ date class

the namespace is ec::
include three class Duration Date Time

##example

{
	ec::Time now;

	//xxxx-01-01 00:00:00
	cout << now.clone().zeroSet(ec::Duration::Year).toDate().format("%Y-%m-%d %H:%M:%S") << endl;
  
	return 0;
}

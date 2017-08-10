This program allocates pseudo-ttys (PTYs) in a loop, with the intent of snarfing all the available ones to ensure that the next requestor will allocate a new one. The user can specify the number of PTYs to allocate; the default is one.


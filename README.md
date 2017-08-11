This program allocates pseudo-ttys (PTYs) for debugging and other purposes. It uses both means of getting a master PTY:

* `posix_openpt()`, which gets any available PTY
* `open("/dev/ptyxxxx")`, which goes after a specific PTY

The latter option can be useful if something is wrong witha specific PTY and you need to block it from being allocated. An example of this can be read about [here](http://www.stuhenderson.com/RUGNEW85.pdf). z/OS can be strange.
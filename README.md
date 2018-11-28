# Automatic Multicast Tunneling Relay

This builds an executable that hopefully one day will
conform to RFC 7450.

This project is a fork of the relay portion of the code
from <https://github.com/GrumpyOldTroll/amt>, which itself
was a fork of prior work by Juniper and the mboned working
group at IETF.

The intent is to separate the relay and gateway
implementations into 2 separate projects, and develop
them further separately. Also to try to clean up the
build process a bit, with thanks to libabc.
(<https://git.kernel.org/pub/scm/linux/kernel/git/kay/libabc.git)


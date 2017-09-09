This project aims to use a Raspberry PI 3 board to expose a web server
by which one or more users can control one or more of these boards:

http://www.futurashop.it/unit%C3%A0-can-slave-a-rel%C3%A8-7100-ft1130m?search=can%20slave

This board is a CAN Slave unit with 4 relays mounted on it. The board can be initially
configured by CAN Bus using a dedicated CAN Master unit.

This project let the Raspi to receive and send CAN messages reading each board status
and letting the user to change the relays status using a web page exposed by a web server
from the Raspi.

There is a specific configuration file which is read by the Raspi on startup, by which
the user can tell which control message ID has to be used to control the relays on a given
CAN Slave having a specific CAN status message ID. On each entry of the file, the user can
also tell which is the HeartBeat threshold after which the node can be considered out of date.

For example, if the CAN status message has been designed to appear every 1000ms, the user
can choose to set this HB threshold to 5 times, i.e. 5000ms. This can be adjusted according
to the actual load of the bus: with many messages and slaves, you'd better keeping this threshold
quite high, due to possible message collisions. In this case, if that node will be unplugged from
the net, the user will be able to notice it after 5 seconds only. The threshold must then be
adapted to the real dynamic the user needs from each slave.


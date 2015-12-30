### CLI commands

#### 1. show commands:
no | command | description|
---|---------|------------|
1  | show interface `nw_if` |             shows network interfaces |
2  | show interface brief `nw_if` |       shows brief information about the interface |
3  | show interface status `nw_if` |      shows the network interface status (up / down) |
4  | show interface mac `nw_if` |         shows the network interface mac |
5  | show arp_list |                    shows arp table entries |
6  | show ip |                          shows ip of all the interfaces |
7  | show ip `nw_if` |                  shows ip of the network interface |
8  | show netmask `nw_if` |            shows ip netmask of the network interface |
9  | show hist |                        shows all the history |
10 | show hist last `number` |         displays the last number of commands |
11 | show hist first `number` |        displays the first number of commands |
12 | show hist `numberA`:`numberB` |   displays the records between numberA and numberB |
13 | show syslog |                     shows entire system log (last recorded) |
14 | show syslog last `number` |       shows last number of system logs |
15 | show syslog fist `number` |       shows first number of system logs |
16 | show syslog `numberA`:`numberB` | shows the records between numberA and numberB |
17 | show gps fix |                    shows gps fix mode |
18 | show gps brief |                  shows brief information about gps |
19 | show gps position |              shows gps position |
20 | show gps satellites |             shows number of gps satellites |
21 | show users |                      shows list of users (needs privileges) |

#### 2. help:
no | description |
---|-------------|
1  | generic help to display all the main commands
2  | command followed by ? and a return should display the help for that command

#### 3. net: (needs privileges)
no | command | description |
---|---------|-------------|
1  | net ip <interface name>   |        sets the ip to the interface |
2  | net netmask <interface name> |     sets the netmask to the interface |
3  | net ping <ip address>  |          sends a ping request to the ip address |

#### 4. clear: (needs privileges)
no | command | description |
---|---------|-------------|
1  | clear hist |                       clears all the history |
2  | clear hist last `number` |         clears the last number of commands from history |
3  | clear hist first `number` |        clears the first number of commands from history |

#### 5. enter:
no | command | description |
---|---------|------------|
1  | enter |                            enters into privileged execution mode |

#### 6. leave:
no | command | description |
---|---------|-------------|
1  | leave |                            leaves from privileged execution mode |

#### 7. user: (needs privileges)
no | command | description |
---|---------|-------------|
1  | user add `username` `password` |   adds the user with given name and password |
2  | user del `username` |              deletes the user with given name |
3  | user login `username` |            attempt a login with given username |

#### 8. exit:
no | command | description |
---|---------|-------------|
1  | exit |                             exit from the CLI |


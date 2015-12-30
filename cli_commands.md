### CLI commands

#### 1. show commands:
no | command | description|
---|---------|------------|
1  | show interfaces |             shows network interfaces |
2  | show interface brief `nw_if` |       shows brief information about the interface |
3  | show interface status `nw_if` |      shows the network interface status (up / down) |
4  | show interface mac `nw_if` |         shows the network interface mac |
5  | show interface stats `nw_if` |       shows the network interface statistics |
6  | show arp_list |                    shows arp table entries |
7  | show routes `nw_protocol` |        shows network routes (nw_protocol: `IPv4 / IPv6`) |
8  | show ip |                          shows ip of all the interfaces |
9  | show ip `nw_if` |                  shows ip of the network interface |
10  | show netmask `nw_if` |            shows ip netmask of the network interface |
11  | show hist |                        shows all the history |
12 | show hist last `number` |         displays the last number of commands |
13 | show hist first `number` |        displays the first number of commands |
14 | show hist `numberA`:`numberB` |   displays the records between numberA and numberB |
15 | show syslog |                     shows entire system log (last recorded) |
16 | show syslog last `number` |       shows last number of system logs |
17 | show syslog fist `number` |       shows first number of system logs |
18 | show syslog `numberA`:`numberB` | shows the records between numberA and numberB |
19 | show gps fix |                    shows gps fix mode |
20 | show gps brief |                  shows brief information about gps |
21 | show gps position |              shows gps position |
22 | show gps satellites |             shows number of gps satellites |
23 | show users |                      shows list of users (needs privileges) |
24 | show hw_version |                 shows hardware version |
25 | show hw_revision |                shows hardware revision |
26 | show hostname |  displays `hostname` of the device |
27 | show firewall |  display firewall |

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

#### 9. load:
no | command | description |
---|---------|-------------|
1  | load file `ip` `port` | loads the file from a given `ip` and `port` via `ssh` protocol

#### 10. diagnose:
no | command | description |
---|---------|-------------|
1  | diagnose collect `file` | collects the diagnostic information about all the processes in the system into the `file` |
2  | diagnose upload `ip` `port` | uploads the diagnostic information to the service at `ip` and `port` over `ssh` |
3  | diagnose upload | uploads the diagnostic information to the configured `ip` and `port` |

#### 11. configure:
no | command | description |
---|---------|-------------|
1  | configure diagnose_ip `ip` | configures the diagnose `ip`, if present and when `diagnose upload` is only run, then the ip configured will be used |
2  | configure diagnose_port `port` | configures the diagnose `port`, if present and when `diagnose upload` is only run, then the port configured will be used |
3  | configure view | show the recent configuration changes that are stored in-memory |
4  | configure hostname `hostname` | configure the `hostname` of the device

#### 12. commit:
no | command | description |
---|---------|-------------|
1  | commit  | commits the in-memory configuration |
2  | commit `config number` | commits the displayed configuration item via `configure view` |

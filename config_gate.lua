shaco = {
host_loglevel="DEBUG",
host_connmax=11000,
host_service="node,centerc,cmdctl,gate,forward",

node_type="gate",
node_sid=0,
node_ip="127.0.0.1",
node_port=8100,
node_sub="world,load",
center_ip="127.0.0.1",
center_port=8000,
--gate_ip="127.0.0.1",
gate_ip="192.168.1.140",
gate_port=18100,
gate_clientmax=10000,
gate_clientlive=6000,
gate_handler="forward",
}

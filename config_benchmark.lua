sc_loglevel="DEBUG"
sc_connmax=11000
sc_service="benchmark"

--echo_ip="127.0.0.1"
echo_ip="192.168.1.145"
--echo_port=10001
echo_port=18999
benchmark_client_max=1000 -- active client
benchmark_query=100000     -- query times
--benchmark_query_first=10000
benchmark_packet_size=16 -- packet size in bytes
benchmark_packet_split=2 -- packet split to count, then send one after another by interval 10ms

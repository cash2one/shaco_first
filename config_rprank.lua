require "config_base"
def_node("rprank", 202)

sh_module=sh_module..",redisproxy:rprank"
cmd_handle="rprank"

redis_ip="127.0.0.1" 
redis_port=6379 
redis_auth="shaco@1986#0621"
redis_requester="hall"

require "config_base"
def_node("rpuser", 201)

sh_module=sh_module..",redisproxy:rpuser"
redis_ip="127.0.0.1" 
redis_port=6379 
redis_auth="shaco@1986#0621"
redis_requester="hall"

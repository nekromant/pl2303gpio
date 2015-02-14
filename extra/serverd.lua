#!/usr/bin/lua

-- Read the config
dofile("/etc/serverd.conf")

function gpio(host, n,o)
   local str="cp2103gpio --serial="..host.." --gpio="..n.." --out="..o
   os.execute(str)
   --print(str)
end

function green(host, state)
   if (state) then
      gpio(host, 0, 0);
   else
      gpio(host, 0, 1);
   end
end

function ylw(host, state)
   if (state) then
      gpio(host, 1, 0);
   else
      gpio(host, 1, 1);
   end
end

function red(host, state)
   if (state) then
      gpio(host, 2, 0);
   else
      gpio(host, 2, 1);
   end
end

function psu(host, state)
   if (state) then
      gpio(host, 2, 1);
   else
      gpio(host, 2, 0);
   end
end


function is_alive(host, name)
   ylw(name, true)
   result = os.execute("ping -w 1 -c 1 "..host.." > /dev/null") 
   ylw(name, false)
   if (0==result) then 
      return true;
   else
      return false;   
   end

end

function sleep(n)
   ret = os.execute("sleep "..n);
   if (ret > 0) then
      error("Interrupted!");
   end
end


function check_host_status(host, name)
   if (is_alive(host, name)) then
      green(name, true)
      red(name, false)
   else
      green(name, false)
      red(name, true)
   end
end

while true do 
   for i,j in pairs(hosts) do
      check_host_status(j[1],j[2])
      sleep(sleep_interval)
   end

end
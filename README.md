# DistributedMonitor
DistributedMonitor using Ricard - Agrawala algorithm

# Build
Build the solution in Visual Studio (tested on VS2019). 

# Run

After building the solution you can run the program from Visual Studio or project \x64\Debug directory.  
Running the program equals 1 instance of a "Site" in Ricard-Agrawala algorithm.  
run with either $DistributedMonitor run ownPort  
where ownPort is your portnumber on localhost but then you have to connect to each other Site individually  
OR run with $DistributedMonitor run ownPort port1 port2 port3 etc.. 
where ownPort is your portnumber on localhost that is followed by a list of ports of other Sites that will be connected to during initialization  
# After running
You can run tests on each "Site" by typing  
"TESTA"  
"TESTB"  
"TESTC"  

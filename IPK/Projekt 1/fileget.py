import sys
import re
import argparse
import socket

def get_tcp(server_ip, server_port, file_path, hostname):
    #tcp connection (to own function)
    try:
        fs_tcp_client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        fs_tcp_client.connect((server_ip, server_port))                                                                         #connect
        fs_tcp_client.send(bytes("GET "+file_path+" FSP/1.0\r\nHostname: "+hostname+"\r\nAgent: xbasty00\r\n\r\n", "utf-8"))    #send request
        fs_tcp_client.settimeout(5)                                                                                             #set timout
        response = fs_tcp_client.recv(4096)                                                                                     #get response

        #on invalid response
        if response[0:15] != b"FSP/1.0 Success":
            sys.exit("File '{}' not found".format(file_path))
        else:
            data = bytearray()
            
            #load entire response
            while True:
                new = fs_tcp_client.recv(4096)  #get next data part
                if not new:                     #exit loop on end
                    break
                data.extend(new)                #otherwise keep adding the data

            fs_tcp_client.close()               #close socket
            return data
    except socket.timeout:                      #on timeout
        sys.exit("Fileserver '{}' timed out".format(file_server[0]+":"+file_server[1]))

def download_file(file_path, data):
    file_name = re.search(r'((?:.(?!\/))+$)', file_path).group(1)                   #get file name (for file creation/check)
    file_name = file_name.replace('/', '')                                          #remove / if there is any left
    file_name_edited = file_name                                                    #save it for later use
    file_name = file_name.rsplit('.', 1)                                            #split it for renaming
    name_i = 1

    #downloading and renaming files
    while True:
        try:
            file = open(file_name_edited, "x")                                      #test file existance
            file = open(file_name_edited, "wb")                                     #create file in write mode
            file.write(data)                                                        #write to it
            file.close()                                                            #close file
            return                                                                  #exit
        except IOError:                                                             #on error
            if len(file_name) == 1:                                                 #check if extension
                file_name_edited = file_name[0]+"("+str(name_i)+")"
            else:
                file_name_edited = file_name[0]+"("+str(name_i)+")."+file_name[1]   #rename file like windows do (add '(x)', where x is number of the copy)
        name_i += 1


get_all = False

#argument parsing
parser = argparse.ArgumentParser()
parser.add_argument('-n', metavar='NAMESERVER', required=True, help='Server ip & port address')
parser.add_argument('-f', metavar='SURL', required=True, help='File to be downloaded')
args = parser.parse_args()

#regex match for protocol, hostname and file name/path
path_reg = re.search(r'(.*):\/\/(.*?)\/(.+)', args.f)

nameserver = args.n.split(":")                                  #save server as ip and port
protocol = path_reg.group(1)                                    #save protocol
if protocol != "fsp":                                           #protocol check
    sys.exit("Protocol '{}' is invalid".format(protocol))
hostname = path_reg.group(2)                                    #save hostname
if not re.match(r'^[-|_|\w|\.]*$', hostname):                   #hostname check
    sys.exit("File server name '{}' is invalid".format(hostname))
file_path = path_reg.group(3)                                   #save file path
if file_path == "*":                                            #file path check for 'GET ALL'
    get_all = True
    file_path = "index"

#try udp connection
try:
    ns_udp_client = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)                                #set socket
    ns_udp_client.sendto(bytes("WHEREIS "+hostname, "utf-8"), (nameserver[0], int(nameserver[1])))  #ask
    ns_udp_client.settimeout(5)                                                                     #set timout
    file_server, address = ns_udp_client.recvfrom(4096)                                             #get response
    if file_server[0:2] != b"OK":                                                                   #check response
        sys.exit("File server '{}' not found".format(hostname))
    else:
        ns_udp_client.close()                                                                       #close client
except socket.timeout:                                                                              #error on timeout
    sys.exit("Nameserver '{}' timed out".format(nameserver[0]+":"+nameserver[1]))

file_server = str(file_server[3:], 'utf-8') #convert byte-string to string
file_server = file_server.split(":")        #save ip as ip and port

data = get_tcp(file_server[0], int(file_server[1]), file_path, hostname)    #get file data

#download all or just the current file
if get_all:
    file_list = str(data, "utf-8").split('\r\n')                            #split index data by "lines" and save it as array (so we can iterate)
    for i in file_list[:-1]:                                                #iterate through index
        data = get_tcp(file_server[0], int(file_server[1]), i, hostname)    #get file data
        download_file(i, data)                                              #"download" the file
else:
    download_file(file_path, data)                                          #"download" the file

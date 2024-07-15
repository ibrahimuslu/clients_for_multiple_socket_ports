# a script to increase the size of property one by one and send data as hex to nc command with piping to xxd -r -p

import os
import sys
import time

def send_data(data):
    os.system(f"echo {data} | xxd -r -p | nc -u localhost 4000 -w 0")
    print(f"Data sent: {data}")
    
def main():
    # initial data should be 6 bytes in int format
    data = "000100010000"
    
    for i in range(1, 65536):
        send_data(data)
        # increase the data as 1 bit in hex format
        data = hex(int(data, 16) + 1)[2:].zfill(12)
        print(f"Data: {data}")
        time.sleep(0.1)
        
if __name__ == "__main__":
    main()

# run this script and see the output of the server
# it will print the data sent by the client
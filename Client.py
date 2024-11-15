import socket
from random import randint
from time import sleep
import pickle
import struct
import serial
import socket

from struct import *


if __name__=="__main__":
    # Specify IP Host and Port
    HOST = '127.0.0.1' # Testing
    # HOST = '192.168.7.1'
    PORT = 12345

    # UART port and serial communication
    UART_PORT = "/dev/ttyO4" # UART4 device on BeagleBone
    BAUD_RATE = 115200
    ser = serial.Serial(UART_PORT, BAUD_RATE)

    # Connect to socket
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as client_socket:
        client_socket.connect((HOST, PORT))
        print("Connected to server")

        client_socket.settimeout(30.0)  # Timeout set to 30 seconds

        # Receive message from server
        def receive_data(conn):
            raw_msglen = conn.recv(4)  # Receive message length
            if not raw_msglen:
                return None
            msglen = struct.unpack('>I', raw_msglen)[0]
            data = conn.recv(msglen)  # Receive message data
            return pickle.loads(data)

        message = receive_data(client_socket)
        print("Message received from server:", message)


    while True:
        pass

        # Create random set of values for packaging
        random_data = {"random_value": randint(0, 100)}

        # Package and send data
        serialized_data = pickle.dumps(random_data)
        # ser.write(serialized_data)  # Send serialized data over UART
        print("Mock UART Output:", serialized_data.decode()) # Testing
        print("Random data sent to Tiva Launchpad:", random_data)

        # Add delay if needed
        sleep(1)
